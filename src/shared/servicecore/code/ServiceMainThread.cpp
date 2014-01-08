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


ServiceMainThread::ServiceMainThread() : Thread::BaseThread("ServiceMain Worker")
{
}

ServiceMainThread::~ServiceMainThread()
{
	m_vLock.lock();

	for (size_t x=0; x<m_vJobList.size(); x++)
	{
		m_vJobList[x]->destroy();
	}

	m_vJobList.clear();
	m_vLock.unlock();
}

void ServiceMainThread::addTask(TaskI* task)
{
	if (!task)
		return;

	if (isStopped())
	{
		task->destroy();
		return;
	}

	m_vLock.lock();
	m_vJobList.push_back(task);
	m_vLock.unlock();

	m_WaitCond.notify();
}


void ServiceMainThread::run()
{
	while (!isStopped())
	{
		TaskI* task = nullptr;

		m_vLock.lock();

		if (m_vJobList.size() > 0)
		{
			task = m_vJobList.front();
			m_vJobList.pop_front();
		}

		m_vLock.unlock();

		if (task)
		{
			task->doTask();
			task->destroy();
		}
		else
		{
			m_WaitCond.wait();
		}
	}
}

void ServiceMainThread::onStop()
{
	m_WaitCond.notify();
}