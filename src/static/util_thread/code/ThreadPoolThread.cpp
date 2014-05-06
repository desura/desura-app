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
#include "ThreadPoolThread.h"
#include "util_thread/ThreadPool.h"

using namespace Thread;


ThreadPoolThread::ThreadPoolThread(gcRefPtr<ThreadPoolTaskSourceI> &pTaskSource, bool forced) 
	: BaseThread("Thread Pool Worker")
	, m_bForced(forced)
	, m_pTaskSource(pTaskSource)
{
}

ThreadPoolThread::~ThreadPoolThread()
{
	nonBlockStop();
	m_WaitCondition.notify();
	stop();
}

void ThreadPoolThread::newTask()
{
	m_WaitCondition.notify();
}

void ThreadPoolThread::run()
{
	if (m_bForced)
	{
		doTask();
	}
	else
	{
		while (!isStopped())
		{
			doPause();

			if (!doTask() && !isStopped())
				m_WaitCondition.wait();
		}
	}

	m_bCompTask = true;
}

bool ThreadPoolThread::doTask()
{
	auto task = m_pTaskSource->getTask();

	if (!task)
		return false;

	std::string name = task->getName();

	{
		std::lock_guard<std::mutex> guard(m_TaskLock);
		m_pTask = task;
	}
	
	task->doTask();

	{
		std::lock_guard<std::mutex> guard(m_TaskLock);
		m_pTask = nullptr;
	}
	
	onCompleteEvent();
	return true;
}

void ThreadPoolThread::onStop()
{
	std::lock_guard<std::mutex> guard(m_TaskLock);

	if (m_pTask)
		m_pTask->onStop();

	m_WaitCondition.notify();
}

void ThreadPoolThread::stopTask()
{
	std::lock_guard<std::mutex> guard(m_TaskLock);

	if (m_pTask)
		m_pTask->onStop();
}
