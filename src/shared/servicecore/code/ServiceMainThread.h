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

#ifndef DESURA_SERVICEMAINTHREAD_H
#define DESURA_SERVICEMAINTHREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "util_thread/BaseThread.h"


class TaskI
{
public:
	virtual void doTask()=0;
	virtual ~TaskI(){}
};

class ServiceMainThread : public ::Thread::BaseThread
{
public:
	ServiceMainThread();
	~ServiceMainThread();

	void addTask(TaskI* task);

protected:
	void run() override;
	void onStop() override;

	std::shared_ptr<TaskI> popTask();

private:
	std::mutex m_vLock;
	std::deque<std::shared_ptr<TaskI>> m_vJobList;

	::Thread::WaitCondition m_WaitCond;
};

#endif //DESURA_SERVICEMAINTHREAD_H
