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

#ifndef DESURA_BASETHREAD_H
#define DESURA_BASETHREAD_H
#ifdef _WIN32
#pragma once
#endif

namespace Thread
{

class WaitCondition
{
public:
	WaitCondition();
	~WaitCondition();

	void wait();

	//! returns false if the call is returning because the time specified was reached, true otherwise
	bool wait(int sec, int msec = 0);

	void notify();

private:
	class WaitConditionPrivates;

	WaitConditionPrivates* m_pPrivates;
};


//! Repersents a basic thread class
//!
class BaseThread
{
public:
	//! Constuctor
	//!
	//! @param name Thread name
	//!
	BaseThread(const char* name);
	virtual ~BaseThread();


	enum PRIORITY
	{
		REALTIME = 0,
		HIGH,
		ABOVE_NORMAL,
		NORMAL,
		BELOW_NORMAL,
		IDLE,
	};

	//! Start the thread
	//!
	void start();

	//! Stop the thread. Waits to thread is stopped
	//!
	void stop();

	//! Stop the thread. Doesnt wait to stopped
	//!
	void nonBlockStop();

	//! Unpause the thread
	//!
	void unpause();

	//! Pauses the thread
	//!
	void pause();

	//! Joins the thread. (Waits until thread is stopped)
	//!
	void join();

	//! Set thread priority
	//!
	//! @param priority Thread priority level
	//!
	void setPriority(PRIORITY priority);

	//! Is thread currently running
	//!
	//! @return True for running, false if not
	//!
	bool isRunning();

	//! Is thread currently stopped
	//!
	//! @return True for stopped, false if not
	//!
	volatile bool isStopped();

	//! Is thread currently paused
	//!
	//! @return True for paused, false if not
	//!
	volatile bool isPaused();

	//! This checks to see if the thread is paused and waits till its unpaused
	//! Should be called in every run loop
	//!
	void doPause();

	//! Gets the thread name
	//!
	//! @return Thread name
	//!
	const char* getName();

	//! Gets this current thread id
	//!
	static uint64 GetCurrentThreadId();

protected:
	//! Overload run to implement what the thread does when its started
	//!
	virtual void run() = 0;

	//! Gets called when the thread is paused
	//!
	virtual void onPause(){;}

	//! Gets called when the thread is unpaused
	//!
	virtual void onUnpause(){;}

	//! Gets cakked when the thread is stopped
	//!
	virtual void onStop(){;}


	//! Applys priority to the thread after its started
	//!
	void applyPriority();

	//! Sets the thread name for the debugger after its started
	//!
	void setThreadName(const char* nameOveride = nullptr);

private:
	class ThreadPrivates;
	ThreadPrivates* m_pPrivates;
};


}


#endif // DESURA_BASETHREAD_H
