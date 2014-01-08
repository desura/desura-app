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
#include "UpdateThread.h"
#include "User.h"

#include "UpdateThread_Old.h"



namespace UserCore
{
namespace Thread
{

UpdateThread::UpdateThread(EventV *onForcePollEvent, bool loadLoginItems) : BaseUserThread<UserThreadI, ::Thread::BaseThread>( "Update poll Thread", DesuraId() )
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
