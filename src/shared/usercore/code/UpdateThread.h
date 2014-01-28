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

#ifndef DESURA_UPDATETHREAD_H
#define DESURA_UPDATETHREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "usercore/UserThreadI.h"
#include "util_thread/BaseThread.h"
#include "BaseUserThread.h"

namespace XML
{
	class gcXMLElement;
}

class UpdateThreadI
{
public:
	virtual void setInfo(UserCore::UserI* user, WebCore::WebCoreI* webcore)=0;
	virtual void doRun()=0;
	virtual void onStop()=0;
	virtual bool onMessageReceived(const char* resource, const XML::gcXMLElement &root)=0;
	virtual ~UpdateThreadI(){}

	Event<bool> isStoppedEvent;

protected:
	bool isStopped()
	{
		bool stopped = false;
		isStoppedEvent(stopped);
		return stopped;
	}
};



namespace UserCore
{
namespace Thread
{


//! Polls for Desura updates
//!
class UpdateThread : public BaseUserThread<UserThreadI, ::Thread::BaseThread>
{
public:
	UpdateThread(Event<std::tuple<gcOptional<bool>, gcOptional<bool>, gcOptional<bool>>> *onForcePollEvent, bool loadLoginItems);
	~UpdateThread();

protected:
	void doRun() override;
	void onStop() override;

	virtual bool onMessageReceived(const char* resource, const XML::gcXMLElement &root);

	void isThreadStopped(bool &stopped);

private:
	UpdateThreadI *m_pBaseTask;
};

}
}

#endif //DESURA_UPDATETHREAD_H
