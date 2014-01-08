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
class BaseTask
{
public: 
	virtual ~BaseTask()=0;

	virtual const char* getName()=0;

	//! Override doTask to implement the functionality for the task
	//!
	virtual void doTask()=0;

	//! Gets called when the task is stopped
	//!
	virtual void onStop(){;};

	Event<uint32> onCompleteEvent;
};

class ThreadPoolTaskSourceI
{
public:
	virtual BaseTask* getTask()=0;
};

//! Basic thread pool
//!
class ThreadPool : public Thread::BaseThread, public Thread::ThreadPoolTaskSourceI
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
	void queueTask(BaseTask *task);

	//! Add a task to the pool and start it strait away
	//!
	//! @param task Task to add
	//!
	void forceTask(BaseTask *task);

	//! Remove all tasks from pool
	//!
	void purgeTasks();

	//! Stop new tasks from being added
	void blockTasks();

	//! Enable new tasks to be added
	void unBlockTasks();

protected:
	void run();

	void onPause();
	void onUnpause();
	void onStop();

	uint8 activeThreads();
	void startNewTasks();

	void removedForced();
	void onThreadComplete();

	virtual BaseTask* getTask();

private:
	uint8 m_uiCount;

	volatile bool m_bIsTaskBlocked;


	std::vector<ThreadPoolThread*> m_vForcedList;
	std::vector<ThreadPoolThread*> m_vThreadList;
	std::deque<BaseTask*> m_vTaskList;

	std::mutex m_TaskMutex;
	std::mutex m_ForcedMutex;
	std::mutex m_ThreadMutex;

	WaitCondition m_WaitCondition;
};

}

#endif //DESURA_THREADPOOL_H
