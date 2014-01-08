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

#ifndef DESURA_BDMANAGER_H
#define DESURA_BDMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "mcfcore/DownloadProvider.h"
#include "util_thread/BaseThread.h"
#include "UserTasks.h"



namespace UserCore
{
class User;

namespace Misc
{
	class BannerNotifierI
	{
	public:
		virtual void onBannerComplete(MCFCore::Misc::DownloadProvider &info)=0;
	};
}

class BDManager
{
public:
	BDManager(UserCore::User* user);
	~BDManager();

	void downloadBanner(UserCore::Misc::BannerNotifierI* obj, const MCFCore::Misc::DownloadProvider& provider);
	void cancelDownloadBannerHooks(UserCore::Misc::BannerNotifierI* obj);

protected:
	void onBannerComplete(UserCore::Task::BannerCompleteInfo& bci);

private:
	std::mutex m_BannerLock;
	std::map<UserCore::Task::DownloadBannerTask*, UserCore::Misc::BannerNotifierI*> m_mDownloadBannerTask;

	UserCore::User* m_pUser;
};

}

#endif //DESURA_BDMANAGER_H
