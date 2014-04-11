/*
Desura is the leading indie game distribution platform
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)

$LicenseInfo:firstyear=2014&license=lgpl$
Copyright (C) 2014, Linden Research, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation;
version 2.1 of the License only.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, see <http://www.gnu.org/licenses/>
or write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
$/LicenseInfo$
*/

#include "Common.h"
#include "ItemThread.h"

#include "ItemHandle.h"
#include "ItemInfo.h"
#include "webcore/WebCoreI.h"
#include "usercore/UserCoreI.h"

#include "UserThreadManager.h"

using namespace UserCore::Item;

ItemThread::ItemThread(UserCore::Item::ItemHandle *handle) : ::Thread::BaseThread(gcString("{0} Thread", handle->getItemInfo()->getShortName()).c_str())
{
	m_szBaseName = gcString("{0}", handle->getItemInfo()->getShortName());
	m_bRunningTask = false;
	start();

	m_pThreadManager = nullptr;	
	m_pWebCore = nullptr;
	m_pUserCore = nullptr;

	m_pCurrentTask = nullptr;
	m_bDeleteCurrentTask = false;
}

ItemThread::~ItemThread()
{
	purge();

	//for some reason it blocks on stop. 

	nonBlockStop();
	m_WaitCond.notify();
	join();

	if (m_pThreadManager)
		m_pThreadManager->delist(this);
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

		if (m_pCurrentTask)
			m_pCurrentTask->onStop();
		else if (size > 0) //could be getting a task now
			m_bDeleteCurrentTask = true;
	}
}

void ItemThread::setThreadManager(UserCore::UserThreadManagerI* tm)
{
	gcAssert(tm);
	m_pThreadManager = tm;

	if (m_pThreadManager)
		m_pThreadManager->enlist(this);
}

void ItemThread::setWebCore(WebCore::WebCoreI *wc)
{
	m_pWebCore = wc;
}

void ItemThread::setUserCore(UserCore::UserI *uc)
{
	m_pUserCore = uc;
}

void ItemThread::queueTask(UserCore::ItemTask::BaseItemTask *task)
{
	if (isStopped())
	{
		safe_delete(task);
		return;
	}

	if (!task)
		return;

	task->setUserCore(m_pUserCore);
	task->setWebCore(m_pWebCore);

	{
		std::lock_guard<std::mutex> guard(m_TaskMutex);
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

	UserCore::ItemTask::BaseItemTask* task = getNewTask();

	if (!task)
		return false;

	ITEM_STAGE taskType;

	{
		std::lock_guard<std::mutex> guard(m_DeleteMutex);
		m_bDeleteCurrentTask = false;
		taskType = task->getTaskType();
		m_pCurrentTask = task;
	}

	if (!m_bDeleteCurrentTask)
	{
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
	}

	safe_delete(task);
	setThreadName(m_szBaseName.c_str());
	
	return true;
}

UserCore::ItemTask::BaseItemTask* ItemThread::getNewTask()
{
	if (isPaused())
		return nullptr;

	UserCore::ItemTask::BaseItemTask* task = nullptr;

	std::lock_guard<std::mutex> guard(m_TaskMutex);
	
	if (m_vTaskList.size() != 0)
	{
		task = m_vTaskList.front();
		m_vTaskList.pop_front();

		gcString name = m_szBaseName;
		name += " - ";
		name += task->getTaskName();

		this->setThreadName(name.c_str());
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
