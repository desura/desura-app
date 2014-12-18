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
#include "ScriptTaskThread.h"
#include <v8.h>

ScriptTaskThread::ScriptTaskThread()
	: BaseThread("ScriptCore Thread")
{
}

ScriptTaskThread::~ScriptTaskThread()
{
	stop();

	std::lock_guard<std::mutex> guard(m_LockMutex);

	for (size_t x=0; x<m_TaskQue.size(); x++)
		m_TaskQue[x]->destory();

	m_TaskQue.clear();
}

void ScriptTaskThread::queTask(ScriptTaskI* task)
{
	std::lock_guard<std::mutex> guard(m_LockMutex);
	m_TaskQue.push_back(task);

	m_WaitCond.notify();
}

void ScriptTaskThread::setLastTask(ScriptTaskI* task)
{
	m_pLastTask = task;
}

void ScriptTaskThread::run()
{
	auto pIsolator = v8::Isolate::New();

	while (!isStopped())
	{
		ScriptTaskI* curTask = nullptr;

		{
			std::lock_guard<std::mutex> guard(m_LockMutex);

			if (m_TaskQue.size() > 0)
			{
				curTask = m_TaskQue.front();
				m_TaskQue.pop_front();
			}
		}

		if (curTask)
		{
			v8::Isolate::Scope scope(pIsolator);
			curTask->doTask();

			curTask->destory();
			curTask = nullptr;
		}
		else
		{
			m_WaitCond.wait(2);
		}
	}

	if (m_pLastTask)
	{
		v8::Isolate::Scope scope(pIsolator);
		m_pLastTask->doTask();
		m_pLastTask->destory();
	}

	pIsolator->Dispose();
}

void ScriptTaskThread::onStop()
{
	m_WaitCond.notify();
}
