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

#ifndef DESURA_UNINSTALLALLTHREAD_H
#define DESURA_UNINSTALLALLTHREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "UserCore\UserCoreI.h"
#include "util_thread\BaseThread.h"
#include "UserCore\ItemHelpersI.h"

class UninstallAllThread : public Thread::BaseThread, UserCore::Item::Helper::ItemUninstallHelperI, UserCore::Item::Helper::ItemHandleHelperI
{
public:
	UninstallAllThread(uint32 flags, gcRefPtr<UserCore::UserI> user);

	EventV onCompleteEvent;
	Event<gcString> onLogEvent;
	Event<std::pair<uint32, uint32> > onProgressEvent;

	gc_IMPLEMENT_REFCOUNTING(UninstallAllThread);

protected:
	virtual void run();
	virtual void onStop();

	virtual bool stopStagePrompt();

	virtual void onComplete(uint32 status);
	virtual void onComplete(gcString& string);

	virtual void onMcfProgress(MCFCore::Misc::ProgressInfo& info);
	virtual void onProgressUpdate(uint32 progress);

	virtual void onError(gcException e);
	virtual void onNeedWildCard(WCSpecialInfo& info);

	virtual void onDownloadProvider(UserCore::Misc::GuiDownloadProvider &provider);
	virtual void onVerifyComplete(UserCore::Misc::VerifyComplete& info);

	virtual uint32 getId();
	virtual void setId(uint32 id);

	virtual void onPause(bool state);

	void removeUninstallInfo();
	void removeGameExplorerInfo();

	void removeDesuraCache();
	void removeDesuraSettings();

	void delLeftOverMcf(UTIL::FS::Path path, std::vector<std::string> &extList);
	gcString getCachePath();

	bool hasPaidBranch(UserCore::Item::ItemInfoI* item);
private:
	gcRefPtr<UserCore::UserI> m_pUser;
	uint32 m_uiId;
	uint32 m_iFlags;

	uint32 m_iTotalPos;
	uint32 m_iTotal;

	Thread::WaitCondition m_WaitCondition;
};

#endif //DESURA_UNINSTALLALLTHREAD_H
