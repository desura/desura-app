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
#include "BaseItemServiceTask.h"
#include "User.h"

using namespace UserCore::ItemTask;

BaseItemServiceTask::BaseItemServiceTask(UserCore::Item::ITEM_STAGE type, const char* name, UserCore::Item::ItemHandle* handle, MCFBranch branch, MCFBuild build) 
	: BaseItemTask(type, name, handle, branch, build)
{
	m_bFinished = false;
	m_bStarted = false;
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

	bool shouldWait = initService();

	if (shouldWait && !m_bFinished && !isStopped())
		m_WaitCond.wait();

	setFinished();
}

void BaseItemServiceTask::onStop()
{
	BaseItemTask::onStop();
	m_WaitCond.notify();
}

IPC::ServiceMainI* BaseItemServiceTask::getServiceMain()
{
	return getUserCore()->getServiceMain();
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
