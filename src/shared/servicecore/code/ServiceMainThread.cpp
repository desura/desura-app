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
#include "ServiceMainThread.h"


ServiceMainThread::ServiceMainThread() 
	: Thread::BaseThread("ServiceMain Worker")
{
}

ServiceMainThread::~ServiceMainThread()
{
	stop();

	std::lock_guard<std::mutex> guard(m_vLock);
	m_vJobList.clear();
}

void ServiceMainThread::addTask(TaskI* task)
{
	if (!task)
		return;

	gcTrace("");

	if (isStopped())
	{
		safe_delete(task);
		return;
	}

	std::lock_guard<std::mutex> guard(m_vLock);
	m_vJobList.push_back(std::shared_ptr<TaskI>(task));
	m_WaitCond.notify();
}


void ServiceMainThread::run()
{
	while (!isStopped())
	{
		std::shared_ptr<TaskI> task = popTask();

		if (task)
		{
			TraceT("ServiceMainThread::doTask", this, "");
			task->doTask();
			continue;
		}

		m_WaitCond.wait();
	}
}

std::shared_ptr<TaskI> ServiceMainThread::popTask()
{
	std::lock_guard<std::mutex> guard(m_vLock);

	if (m_vJobList.empty())
		return std::shared_ptr<TaskI>();

	auto task = m_vJobList.front();
	m_vJobList.pop_front();

	return task;
}

void ServiceMainThread::onStop()
{
	m_WaitCond.notify();
}