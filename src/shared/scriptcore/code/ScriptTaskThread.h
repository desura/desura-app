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

#ifndef DESURA_SCRIPTTASKTHREAD_H
#define DESURA_SCRIPTTASKTHREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "ScriptCoreI.h"
#include "util_thread/BaseThread.h"

#include <deque>

class ScriptTaskThread : public ::Thread::BaseThread, public ScriptTaskRunnerI
{
public:
	ScriptTaskThread();
	~ScriptTaskThread();

	virtual void queTask(ScriptTaskI* task);
	virtual void setLastTask(ScriptTaskI* task);

protected:
	virtual void run();
	virtual void onStop();

private:
	ScriptTaskI* m_pLastTask = nullptr;

	::Thread::WaitCondition m_WaitCond;
	std::mutex m_LockMutex;

	std::deque<ScriptTaskI*> m_TaskQue;
};

#endif //DESURA_SCRIPTTASKTHREAD_H
