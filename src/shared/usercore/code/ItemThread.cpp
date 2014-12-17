/*
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)
Copyright (C) 2014 Bad Juju Games, Inc.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA.

Contact us at legal@badjuju.com.

*/

#include "Common.h"
#include "ItemThread.h"

#include "ItemHandle.h"
#include "ItemInfo.h"
#include "webcore/WebCoreI.h"
#include "usercore/UserCoreI.h"

#include "UserThreadManager.h"

using namespace UserCore::Item;

ItemThread::ItemThread(gcRefPtr<UserCore::Item::ItemHandle> handle)
	: ::Thread::BaseThread(gcString("{0} Thread", handle->getItemInfo()->getShortName()).c_str())
	, m_szBaseName("{0}", handle->getItemInfo()->getShortName())
{
	start();
}

ItemThread::~ItemThread()
{
	//Should have this reset before we get here
	gcAssert(!m_pThreadManager);

	purge();

	//for some reason it blocks on stop.

	nonBlockStop();
	m_WaitCond.notify();
	join();
}

void ItemThread::purge()
{
	size_t size;

	{
		std::lock_guard<std::mutex> guard(m_TaskMutex);
		size = m_vTaskList.size();
		safe_delete(m_vTaskList);
	}

	{
		std::lock_guard<std::mutex> guard(m_DeleteMutex);

		m_bDeleteCurrentTask = true;

		if (m_pCurrentTask)
			m_pCurrentTask->onStop();
	}
}

void ItemThread::setThreadManager(const gcRefPtr<UserCore::UserThreadManagerI> &tm)
{
	std::lock_guard<std::mutex> guard(m_TaskMutex);

	if (m_pThreadManager)
		m_pThreadManager->delist(this);

	m_pThreadManager = tm;

	if (m_pThreadManager)
		m_pThreadManager->enlist(this);
}

void ItemThread::setWebCore(gcRefPtr<WebCore::WebCoreI> wc)
{
	std::lock_guard<std::mutex> guard(m_TaskMutex);
	m_pWebCore = wc;
}

void ItemThread::setUserCore(gcRefPtr<UserCore::UserI> uc)
{
	std::lock_guard<std::mutex> guard(m_TaskMutex);
	m_pUserCore = uc;
}

void ItemThread::queueTask(const gcRefPtr<UserCore::ItemTask::BaseItemTask> &task)
{
	if (isStopped())
		return;

	if (!task)
		return;

	{
		std::lock_guard<std::mutex> guard(m_TaskMutex);

		task->setUserCore(m_pUserCore);
		task->setWebCore(m_pWebCore);

		m_vTaskList.push_back(task);
	}

	//get thread running again.
	m_WaitCond.notify();
}

void ItemThread::run()
{
	while (!isStopped())
	{
		doPause();

		if (!performTask())
			m_WaitCond.wait(2);
	}
}

bool ItemThread::performTask()
{
	if (isStopped())
		return true;

	auto task = getNewTask();

	if (!task)
		return false;

	ITEM_STAGE taskType;

	{
		std::lock_guard<std::mutex> guard(m_DeleteMutex);

		if (!m_bDeleteCurrentTask)
		{
			taskType = task->getTaskType();
			m_pCurrentTask = task;
		}
	}

	if (!m_bDeleteCurrentTask)
	{
		gcString name = m_szBaseName;
		name += " - ";
		name += task->getTaskName();

		setThreadName(name.c_str());

		m_bRunningTask = true;
		onTaskStartEvent(taskType);

		gcTrace("Task {0}", task->getTaskName());
		task->doTask();

		onTaskCompleteEvent(taskType);
		m_bRunningTask = false;
	}

	{
		std::lock_guard<std::mutex> guard(m_DeleteMutex);
		m_pCurrentTask = nullptr;
		m_bDeleteCurrentTask = false;
	}

	safe_delete(task);
	setThreadName(m_szBaseName.c_str());

	return true;
}

gcRefPtr<UserCore::ItemTask::BaseItemTask> ItemThread::getNewTask()
{
	if (isPaused())
		return nullptr;

	{
		std::lock_guard<std::mutex> guard(m_DeleteMutex);
		m_bDeleteCurrentTask = false;
	}

	gcRefPtr<UserCore::ItemTask::BaseItemTask> task;

	std::lock_guard<std::mutex> guard(m_TaskMutex);

	if (m_vTaskList.size() != 0)
	{
		task = m_vTaskList.front();
		m_vTaskList.pop_front();
	}

	return task;
}

void ItemThread::onPause()
{
	std::lock_guard<std::mutex> guard(m_DeleteMutex);

	if (m_pCurrentTask)
		m_pCurrentTask->onPause();
}

void ItemThread::onUnpause()
{
	{
		std::lock_guard<std::mutex> guard(m_DeleteMutex);

		if (m_pCurrentTask)
			m_pCurrentTask->onUnpause();
	}

	m_WaitCond.notify();
}

void ItemThread::onStop()
{
	{
		std::lock_guard<std::mutex> guard(m_DeleteMutex);

		if (m_pCurrentTask)
			m_pCurrentTask->onStop();
	}

	m_WaitCond.notify();
}

void ItemThread::cancelCurrentTask()
{
	std::lock_guard<std::mutex> guard(m_DeleteMutex);

	if (m_pCurrentTask)
		m_pCurrentTask->cancel();
}

bool ItemThread::hasTaskToRun()
{
	if (isRunningTask())
		return true;

	std::lock_guard<std::mutex> guard(m_TaskMutex);
	return (m_vTaskList.size() > 0);
}
