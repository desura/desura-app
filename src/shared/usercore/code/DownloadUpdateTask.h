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

#ifndef DESURA_USERHELPERTHREAD_H
#define DESURA_USERHELPERTHREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "util_thread/ThreadPool.h"
#include "mcfcore/MCFMain.h"
#include "mcfcore/ProgressInfo.h"
#include "User.h"

#include "webcore/WebCoreI.h"

#include "UserTask.h"

namespace UserCore
{
namespace Task
{

class DownloadUpdateTask : public UserTask
{
public:
	DownloadUpdateTask(UserCore::User* user, uint32 appver, uint32 build, bool bForced);

	void doTask();

	//i overload the ptr as a boolean value to test if we need to display the update window
	Event<uint32> onDownloadProgressEvent;
	Event<UserCore::Misc::update_s> onDownloadCompleteEvent;
	Event<UserCore::Misc::update_s> onDownloadStartEvent;

	const char* getName(){return "DownloadUpdateTask";}

protected:
	void onStop();
	void downloadUpdate();

	void onDownloadProgress(MCFCore::Misc::ProgressInfo& p);

private:
	McfHandle m_hMcfHandle;

	volatile bool m_bCompleted = false;
	volatile bool m_bStopped = false;
	gcString m_szPath;

	const bool m_bForced;
	const uint32 m_uiAppVer;
	const uint32 m_uiAppBuild;
	
	uint32 m_uiLastPercent = -1;
};

}
}

#endif //DESURA_USERHELPERTHREAD_H
