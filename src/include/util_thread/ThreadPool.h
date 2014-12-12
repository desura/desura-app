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

#ifndef DESURA_THREADPOOL_H
#define DESURA_THREADPOOL_H
#ifdef _WIN32
#pragma once
#endif

#include "BaseThread.h"

namespace Thread
{

	class ThreadPoolThread;

	//! Basic functions required for a task for the thread pool
	//!
	class BaseTask : public gcRefBase
	{
	public:
		virtual ~BaseTask(){}

		virtual const char* getName() = 0;

		//! Override doTask to implement the functionality for the task
		//!
		virtual void doTask() = 0;

		//! Gets called when the task is stopped
		//!
		virtual void onStop(){}

		Event<uint32> onCompleteEvent;

		gc_IMPLEMENT_REFCOUNTING(BaseTask)
	};

	class ThreadPoolTaskSourceI : public gcRefBase
	{
	public:
		virtual gcRefPtr<BaseTask> getTask() = 0;
	};

	class ThreadPoolI : public gcRefBase
	{
	public:
		virtual void queueTask(gcRefPtr<BaseTask> pTask)=0;
		virtual void forceTask(gcRefPtr<BaseTask> pTask)=0;
	};

	//! Basic thread pool
	//!
	class ThreadPool : public ThreadPoolI, public Thread::BaseThread, public Thread::ThreadPoolTaskSourceI
	{
	public:
		//! Constuctor
		//!
		//! @param num Number of threads in pool
		//!
		ThreadPool(uint8 num);
		~ThreadPool();

		//! Add a task to the queue
		//!
		//! @param task Task to add
		//!
		void queueTask(gcRefPtr<BaseTask> pTask) override;

		//! Add a task to the pool and start it strait away
		//!
		//! @param task Task to add
		//!
		void forceTask(gcRefPtr<BaseTask> pTask) override;

		//! Remove all tasks from pool
		//!
		void purgeTasks();

		//! Stop new tasks from being added
		void blockTasks();

		//! Enable new tasks to be added
		void unBlockTasks();

		void cleanup();

	protected:
		void run();

		void onPause();
		void onUnpause();
		void onStop();

		uint8 activeThreads();
		void startNewTasks();

		void removedForced();
		void onThreadComplete();

		virtual gcRefPtr<BaseTask> getTask();

	private:
		uint8 m_uiCount;

		volatile bool m_bIsTaskBlocked;


		std::vector<gcRefPtr<ThreadPoolThread>> m_vForcedList;
		std::vector<gcRefPtr<ThreadPoolThread>> m_vThreadList;
		std::deque<gcRefPtr<BaseTask>> m_vTaskList;

		std::mutex m_TaskMutex;
		std::mutex m_ForcedMutex;
		std::mutex m_ThreadMutex;

		WaitCondition m_WaitCondition;

		gc_IMPLEMENT_REFCOUNTING(ThreadPool)
	};

}

#endif //DESURA_THREADPOOL_H
