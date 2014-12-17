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

#ifndef DESURA_THREADPOOL_THREAD_H
#define DESURA_THREADPOOL_THREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "util_thread/BaseThread.h"
#include "util_thread/ThreadPool.h"

namespace Thread
{
	//! Thread pool thread that runs tasks
	//!
	class ThreadPoolThread : public Thread::BaseThread, public gcRefBase
	{
	public:
		//! Constructor
		//!
		//! @param id Thread id
		//! @param forced Is a forced task
		//!
		ThreadPoolThread(const gcRefPtr<ThreadPoolTaskSourceI> &pTaskSource, bool isForced = false);
		~ThreadPoolThread();

		//! Is current task forced
		//!
		//! @return True if forced, false if not
		//!
		bool isForced(){ return m_bForced; }

		//! Is current task completed
		//!
		//! @return True if completed, false if not
		//!
		bool hasCompletedTask(){ return m_bCompTask; }

		//! Has a task to run
		//!
		//! @return True if has task, false if not
		//!
		bool hasTask(){ return m_pTask; }

		//! ALerts thread of a new task
		//!
		void newTask();

		//! Stop the current task
		//!
		void stopTask();

		//! Event that triggers when a task is completed
		//!
		EventV onCompleteEvent;

	protected:
		void run();
		void onStop();
		bool doTask();

	private:
		bool m_bForced;
		bool m_bCompTask = false;

		std::mutex m_TaskLock;

		WaitCondition m_WaitCondition;
		gcRefPtr<ThreadPoolTaskSourceI> m_pTaskSource;
		gcRefPtr<BaseTask> m_pTask;


		gc_IMPLEMENT_REFCOUNTING(ThreadPoolThread)
	};
}


#endif
