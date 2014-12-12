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
#include "ServiceMainThread.h"


ServiceMainThread::ServiceMainThread()
	: Thread::BaseThread("ServiceMain Worker")
{
}

ServiceMainThread::~ServiceMainThread()
{
	stop();

	std::lock_guard<std::mutex> guard(m_vLock);
	m_vJobList.clear();
}

void ServiceMainThread::addTask(TaskI* task)
{
	if (!task)
		return;

	gcTrace("");

	if (isStopped())
	{
		safe_delete(task);
		return;
	}

	std::lock_guard<std::mutex> guard(m_vLock);
	m_vJobList.push_back(std::shared_ptr<TaskI>(task));
	m_WaitCond.notify();
}


void ServiceMainThread::run()
{
	while (!isStopped())
	{
		std::shared_ptr<TaskI> task = popTask();

		if (task)
		{
			TraceT("ServiceMainThread::doTask", this, "");
			task->doTask();
			continue;
		}

		m_WaitCond.wait();
	}
}

std::shared_ptr<TaskI> ServiceMainThread::popTask()
{
	std::lock_guard<std::mutex> guard(m_vLock);

	if (m_vJobList.empty())
		return std::shared_ptr<TaskI>();

	auto task = m_vJobList.front();
	m_vJobList.pop_front();

	return task;
}

void ServiceMainThread::onStop()
{
	m_WaitCond.notify();
}
