/*
Desura is the leading indie game distribution platform
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include "Common.h"
#include "util_thread/ThreadPool.h"
#include "ThreadPoolThread.h"

class ThreadPoolTaskSource : public Thread::ThreadPoolTaskSourceI
{
public:
	ThreadPoolTaskSource(Thread::BaseTask* task)
	{
		m_pTask = task;
	}

	virtual Thread::BaseTask* getTask()
	{
		Thread::BaseTask* task = m_pTask;
		delete this;
		return task;
	}

	Thread::BaseTask* m_pTask;
};

using namespace Thread;



BaseTask::~BaseTask()
{

}

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
	nonBlockStop();
	m_WaitCondition.notify();

	join();

	m_TaskMutex.lock();
	safe_delete(m_vTaskList);
	m_TaskMutex.unlock();

	{
		std::lock_guard<std::mutex> guardThread(m_ThreadMutex);

		for (auto thread : m_vThreadList)
		{
			thread->onCompleteEvent -= delegate(this, &ThreadPool::onThreadComplete);
			thread->stop();
		}

		safe_delete(m_vThreadList);
	}

	{
		std::lock_guard<std::mutex> guardForced(m_ForcedMutex);

		for (auto forced : m_vForcedList)
		{
			forced->onCompleteEvent -= delegate(this, &ThreadPool::onThreadComplete);
			forced->stop();
		}

		safe_delete(m_vForcedList);	
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

			safe_delete(m_vForcedList);
		}

		safe_delete(m_vTaskList);
	}

	while (activeThreads() > 0)
		gcSleep(50);
}

void ThreadPool::queueTask(BaseTask *task)
{
	if (!task)
		return;

	if (m_bIsTaskBlocked)
	{
		Warning("Thread pool task blocking active and new task was added.\n");
		safe_delete(task);
		return;
	}

	{
		std::lock_guard<std::mutex> guardTask(m_TaskMutex);
		m_vTaskList.push_back(task);
	}

	//get thread running again.
	m_WaitCondition.notify();
}


void ThreadPool::forceTask(BaseTask *task)
{
	if (!task)
		return;

	std::lock_guard<std::mutex> guardForced(m_ForcedMutex);

	ThreadPoolThread *thread = new ThreadPoolThread(new ThreadPoolTaskSource(task), true);
	thread->onCompleteEvent += delegate(this, &ThreadPool::onThreadComplete);

	m_vForcedList.push_back( thread );

	thread->start();
}

void ThreadPool::run()
{
	for (uint8 x=0; x<m_uiCount; x++)
	{
		ThreadPoolThread *thread = new ThreadPoolThread(this, false);
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
				safe_delete(m_vForcedList[x]);
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

BaseTask* ThreadPool::getTask()
{
	BaseTask* task = nullptr;
	std::lock_guard<std::mutex> guardTask(m_TaskMutex);

	if (m_vTaskList.size() > 0)
	{
		task = m_vTaskList.front();
		m_vTaskList.pop_front();
	}

	return task;
}

