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

#include "Common.h"
#include "BaseItemServiceTask.h"
#include "User.h"

using namespace UserCore::ItemTask;

BaseItemServiceTask::BaseItemServiceTask(UserCore::Item::ITEM_STAGE type, const char* name, gcRefPtr<UserCore::Item::ItemHandleI> &handle, MCFBranch branch, MCFBuild build)
	: BaseItemTask(type, name, handle, branch, build)
{
}

BaseItemServiceTask::~BaseItemServiceTask()
{
	waitForFinish();
}

void BaseItemServiceTask::setFinished()
{
	m_bFinished = true;

	//some one might be waitForFinish on us
	m_WaitCond.notify();
}

void BaseItemServiceTask::resetFinish()
{
	m_bFinished = false;
}

void BaseItemServiceTask::waitForFinish()
{
	if (hasStarted() && !m_bFinished)
		m_WaitCond.wait();

	m_bFinished = true;
}

void BaseItemServiceTask::doRun()
{
	m_bStarted = true;
	resetFinish();

	try
	{
		bool shouldWait = initService();

		if (shouldWait && !m_bFinished && !isStopped())
			m_WaitCond.wait();

		setFinished();
	}
	catch (gcException &e)
	{
		onError(e);
		setFinished();
	}
}

void BaseItemServiceTask::onStop()
{
	BaseItemTask::onStop();
	m_WaitCond.notify();
}

std::shared_ptr<IPC::ServiceMainI> BaseItemServiceTask::getServiceMain()
{
	return getUserCore()->getInternal()->getServiceMain();
}

void BaseItemServiceTask::onFinish()
{
	setFinished();
	m_WaitCond.notify();
}

bool BaseItemServiceTask::hasStarted()
{
	return m_bStarted;
}
