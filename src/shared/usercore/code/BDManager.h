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
		class BannerNotifierI : public gcRefBase
		{
		public:
			virtual ~BannerNotifierI(){}
			virtual void onBannerComplete(MCFCore::Misc::DownloadProvider &info) = 0;
		};
	}

	class BDManager : public gcRefBase
	{
	public:
		BDManager(gcRefPtr<UserCore::User> user);
		~BDManager();

		void downloadBanner(gcRefPtr<UserCore::Misc::BannerNotifierI> obj, const MCFCore::Misc::DownloadProvider& provider);
		void cancelDownloadBannerHooks(UserCore::Misc::BannerNotifierI *pObj);

		void cleanup();

	protected:
		void onBannerComplete(UserCore::Task::BannerCompleteInfo& bci);

	private:
		std::recursive_mutex m_BannerLock;
		std::map<gcRefPtr<UserCore::Task::DownloadBannerTask>, gcRefPtr<UserCore::Misc::BannerNotifierI>> m_mDownloadBannerTask;

		gcRefPtr<UserCore::User> m_pUser;

		gc_IMPLEMENT_REFCOUNTING(BDManager)
	};

}

#endif //DESURA_BDMANAGER_H
