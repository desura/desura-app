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

#ifndef DESURA_USERSERVICEI_H
#define DESURA_USERSERVICEI_H
#ifdef _WIN32
#pragma once
#endif

#include "webcore/WebCoreI.h"
#include "usercore/UserCoreI.h"
#include "usercore/VerifyComplete.h"
#include "usercore/GuiDownloadProvider.h"
#include "mcfcore/ProgressInfo.h"

namespace UserCore
{
namespace Thread
{

class UserServiceI
{
public:
	virtual ~UserServiceI(){;}

	virtual void setThreadManager(UserCore::UserThreadManagerI* tm)=0;
	virtual void setWebCore(WebCore::WebCoreI *wc)=0;
	virtual void setUserCore(UserCore::UserI *uc)=0;

	//events
	virtual Event<uint32>* getCompleteEvent()=0;
	virtual Event<gcException>* getErrorEvent()=0;
	virtual Event<WCSpecialInfo>* getNeedWCEvent()=0;

	//BaseThread
	virtual void start()=0;
	virtual void stop()=0;

	virtual void nonBlockStop()=0;

	virtual void unpause()=0;
	virtual void pause()=0;

	virtual Event<MCFCore::Misc::ProgressInfo>* getMcfProgressEvent()=0;
	virtual Event<UserCore::Misc::VerifyComplete>* getVerifyCompleteEvent()=0;
	virtual Event<UserCore::Misc::GuiDownloadProvider>*	getDownloadProviderEvent()=0;
};

}
}

#endif //DESURA_USERSERVICEI_H
