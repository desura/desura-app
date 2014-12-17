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
#include "UpdateThread.h"
#include "User.h"

#include "UpdateThread_Old.h"



namespace UserCore
{
namespace Thread
{

UpdateThread::UpdateThread(Event<std::tuple<gcOptional<bool>, gcOptional<bool>, gcOptional<bool>>> *onForcePollEvent, bool loadLoginItems)
	: BaseUserThread<UserThreadI, ::Thread::BaseThread>( "Update poll Thread", DesuraId() )
{
	m_pBaseTask = new UpdateThreadOld(onForcePollEvent, loadLoginItems);
	m_pBaseTask->isStoppedEvent += delegate(this, &UpdateThread::isThreadStopped);
}

UpdateThread::~UpdateThread()
{
	stop();
	safe_delete(m_pBaseTask);
}

void UpdateThread::isThreadStopped(bool &stopped)
{
	stopped = isStopped();
}

void UpdateThread::doRun()
{
	m_pBaseTask->setInfo(getUserCore(), getWebCore());
	m_pBaseTask->doRun();
}

void UpdateThread::onStop()
{
	m_pBaseTask->onStop();
}

bool UpdateThread::onMessageReceived(const char* resource, const XML::gcXMLElement &root)
{
	return m_pBaseTask->onMessageReceived(resource, root);
}



}
}
