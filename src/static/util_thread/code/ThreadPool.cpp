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
#include "util_thread/ThreadPool.h"
#include "ThreadPoolThread.h"

class ThreadPoolTaskSource : public Thread::ThreadPoolTaskSourceI
{
public:
	ThreadPoolTaskSource(gcRefPtr<Thread::BaseTask> &pTask)
		: m_pTask(pTask)
	{
	}

	gcRefPtr<Thread::BaseTask> getTask() override
	{
		return m_pTask;
	}

	gcRefPtr<Thread::BaseTask> m_pTask;
	gc_IMPLEMENT_REFCOUNTING(ThreadPoolTaskSource)
};

using namespace Thread;


ThreadPool::ThreadPool(uint8 num) 
	: BaseThread( "Thread Pool" )
{
	if (num == 0)
		m_uiCount = 2;
	else
		m_uiCount = num;

	m_bIsTaskBlocked = false;
	start();
}

ThreadPool::~ThreadPool()
{
	cleanup();
}

void ThreadPool::cleanup()
{
	nonBlockStop();
	m_WaitCondition.notify();

	join();

	{
		std::lock_guard<std::mutex> guard(m_TaskMutex);
		m_vTaskList.clear();
	}

	{
		std::lock_guard<std::mutex> guardThread(m_ThreadMutex);

		for (auto & thread : m_vThreadList)
		{
			thread->onCompleteEvent -= delegate(this, &ThreadPool::onThreadComplete);
			thread->stop();

			safe_delete(thread);
		}

		m_vThreadList.clear();
	}

	{
		std::lock_guard<std::mutex> guardForced(m_ForcedMutex);

		for (auto & forced : m_vForcedList)
		{
			forced->onCompleteEvent -= delegate(this, &ThreadPool::onThreadComplete);
			forced->stop();

			safe_delete(forced);
		}

		m_vForcedList.clear();
	}
}

void ThreadPool::blockTasks()
{
	m_bIsTaskBlocked = true;
}

void ThreadPool::unBlockTasks()
{
	m_bIsTaskBlocked = false;
}

void ThreadPool::purgeTasks()
{
	{
		std::lock_guard<std::mutex> guardTask(m_TaskMutex);

		{
			std::lock_guard<std::mutex> guardThread(m_ThreadMutex);

			for (auto thread : m_vThreadList)
				thread->stopTask();
		}

		{
			std::lock_guard<std::mutex> guardForced(m_ForcedMutex);
			for (auto forced : m_vForcedList)
			{
				forced->stop();
				forced->onCompleteEvent -= delegate(this, &ThreadPool::onThreadComplete);
			}

			m_vForcedList.clear();
		}

		m_vTaskList.clear();
	}

	while (activeThreads() > 0)
		gcSleep(50);
}

void ThreadPool::queueTask(gcRefPtr<BaseTask> pTask)
{
	if (!pTask)
		return;

	if (m_bIsTaskBlocked)
	{
		Warning("Thread pool task blocking active and new task was added.\n");
		return;
	}

	{
		std::lock_guard<std::mutex> guardTask(m_TaskMutex);
		m_vTaskList.push_back(pTask);
	}

	//get thread running again.
	m_WaitCondition.notify();
}


void ThreadPool::forceTask(gcRefPtr<BaseTask> pTask)
{
	if (!pTask)
		return;

	std::lock_guard<std::mutex> guardForced(m_ForcedMutex);


	gcRefPtr<ThreadPoolTaskSourceI> pTPTS(new ThreadPoolTaskSource(pTask));
	auto thread = gcRefPtr<ThreadPoolThread>::create(pTPTS, true);
	thread->onCompleteEvent += delegate(this, &ThreadPool::onThreadComplete);

	m_vForcedList.push_back( thread );

	thread->start();
}

void ThreadPool::run()
{
	for (uint8 x=0; x<m_uiCount; x++)
	{
		auto thread = gcRefPtr<ThreadPoolThread>::create(gcRefPtr<ThreadPoolTaskSourceI>(this), false);
		thread->onCompleteEvent += delegate(this, &ThreadPool::onThreadComplete);
		m_vThreadList.push_back( thread );


		thread->start();
	}

	while (!isStopped())
	{
		doPause();

		removedForced();

		uint32 runTaskCount = activeThreads();

		if (runTaskCount < m_uiCount)
			startNewTasks();

		runTaskCount = activeThreads();

		if ((runTaskCount == m_uiCount || m_vTaskList.size() == 0) && !isStopped())
			m_WaitCondition.wait();
	}
}

uint8 ThreadPool::activeThreads()
{
	uint8 count = 0;
	std::lock_guard<std::mutex> guardThread(m_ThreadMutex);

	for (auto thread : m_vThreadList)
	{
		if (!thread)
			continue;

		if (thread->hasTask())
			count++;
	}

	return count;
}

void ThreadPool::startNewTasks()
{
	std::lock_guard<std::mutex> guardThread(m_ThreadMutex);

	for (auto thread : m_vThreadList)
	{
		if (!thread->hasTask())
			thread->newTask();
	}
}

void ThreadPool::onPause()
{
	{
		std::lock_guard<std::mutex> guardThread(m_ThreadMutex);
		for (auto thread : m_vThreadList)
			thread->pause();
	}

	{
		std::lock_guard<std::mutex> guardForced(m_ForcedMutex);
		for (auto forced : m_vForcedList)
			forced->pause();
	}

	//get thread running again.
	m_WaitCondition.notify();
}

void ThreadPool::onUnpause()
{
	{
		std::lock_guard<std::mutex> guardThread(m_ThreadMutex);
		for (auto thread : m_vThreadList)
			thread->unpause();
	}

	{
		std::lock_guard<std::mutex> guardForced(m_ForcedMutex);
		for (auto forced : m_vForcedList)
			forced->unpause();
	}

	//get thread running again.
	m_WaitCondition.notify();
}

void ThreadPool::onStop()
{
	{
		std::lock_guard<std::mutex> guardThread(m_ThreadMutex);
		for (auto thread : m_vThreadList)
			thread->stop();
	}

	{
		std::lock_guard<std::mutex> guardForced(m_ForcedMutex);
		for (auto forced : m_vForcedList)
			forced->stop();
	}

	//get thread running again.
	m_WaitCondition.notify();
}

void ThreadPool::removedForced()
{
	std::lock_guard<std::mutex> guardForced(m_ForcedMutex);

	if (m_vForcedList.size() > 0)
	{
		std::vector<size_t> delVect;

		for (size_t x=0; x<m_vForcedList.size(); x++)
		{
			if (m_vForcedList[x]->hasCompletedTask())
			{
				m_vForcedList[x]->onCompleteEvent -= delegate(this, &ThreadPool::onThreadComplete);
				delVect.push_back(x);
			}
		}

		std::vector<size_t>::reverse_iterator it = delVect.rbegin();
		while (it != delVect.rend())
		{
			m_vForcedList.erase(m_vForcedList.begin()+*it);
			it++;
		}
	}
}

void ThreadPool::onThreadComplete()
{
	if (isStopped())
		return;

	//get thread running again.
	m_WaitCondition.notify();
}

gcRefPtr<BaseTask> ThreadPool::getTask()
{
	gcRefPtr<BaseTask> task;
	std::lock_guard<std::mutex> guardTask(m_TaskMutex);

	if (m_vTaskList.size() > 0)
	{
		task = m_vTaskList.front();
		m_vTaskList.pop_front();
	}

	return task;
}

