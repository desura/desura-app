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

#ifndef DESURA_USERTASKS_H
#define DESURA_USERTASKS_H
#ifdef _WIN32
#pragma once
#endif

#include "util_thread/BaseThread.h"
#include "util_thread/ThreadPool.h"
#include "UserTask.h"

#include "mcfcore/DownloadProvider.h"

namespace sqlite3x
{
	class sqlite3_connection;
}

namespace MCFCore
{
	namespace MISC
	{
		class DownloadProvider;
	}
}

namespace UserCore
{
	namespace Item
	{
		class ItemThread;
	}

	namespace Task
	{

		class DeleteThread : public UserTask
		{
		public:
			DeleteThread(gcRefPtr<UserCore::UserI> user, gcRefPtr<UserCore::Item::ItemThread> &pThread);
			~DeleteThread();

			void doTask();
			const char* getName(){ return "DeleteThread"; }

		private:
			gcRefPtr<UserCore::Item::ItemThread> m_pThread;
		};



		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////


		class DownloadImgTask : public UserTask
		{
		public:
			enum IMAGE_IDENT
			{
				ICON = 0,
				LOGO,
			};

			//need iteminfo in this case as we might try this before the item is added to the list
			DownloadImgTask(gcRefPtr<UserCore::UserI> user, gcRefPtr<UserCore::Item::ItemInfo> itemInfo, uint8 image);
			void doTask();

			const char* getName(){ return "DownloadImgTask"; }

		protected:
			void onComplete();

		private:
			gcRefPtr<UserCore::Item::ItemInfo> m_pItem;
			uint8 m_Image;
		};


		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////




		class ChangeAccountTask : public UserTask
		{
		public:
			enum ACTION
			{
				ACCOUNT_ADD = 0,
				ACCOUNT_REMOVE,
			};

			ChangeAccountTask(gcRefPtr<UserCore::UserI> user, DesuraId id, uint8 action);
			void doTask();

			const char* getName(){ return "ChangeAccountTask"; }

		private:
			uint8 m_Action;
		};


		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////

		class DownloadBannerTask;

		class BannerCompleteInfo
		{
		public:
			BannerCompleteInfo(gcRefPtr<DownloadBannerTask> pTask, const MCFCore::Misc::DownloadProvider& provider)
				: task(pTask)
				, info(provider)
			{
			}

			bool complete;

			gcRefPtr<DownloadBannerTask> task;
			MCFCore::Misc::DownloadProvider info;
		};


		class DownloadBannerTask : public UserTask
		{
		public:
			DownloadBannerTask(gcRefPtr<UserCore::UserI> user, const MCFCore::Misc::DownloadProvider& dp);
			~DownloadBannerTask();

			void doTask();
			Event<BannerCompleteInfo> onDLCompleteEvent;

			const char* getName(){ return "DownloadBannerTask"; }

		private:
			MCFCore::Misc::DownloadProvider m_DownloadProvider;
		};


		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////


		class DownloadAvatarTask : public UserTask
		{
		public:
			DownloadAvatarTask(gcRefPtr<UserCore::UserI> user, const char* url, uint32 userId);
			void doTask();

			Event<gcException> onErrorEvent;

			const char* getName(){ return "DownloadAvatarTask"; }

		private:
			gcString m_szUrl;
			uint32 m_uiUserId;
		};


		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////


		class GatherInfoTask : public UserTask
		{
		public:
			GatherInfoTask(gcRefPtr<UserCore::UserI> user, DesuraId id, bool addToAccount = false);
			void doTask();

			const char* getName(){ return "GatherInfoTask"; }

		private:
			bool m_bAddToAccount;
		};

		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////

		class CDKeyTask;

		template <typename T>
		class CDKeyEventInfo
		{
		public:
			T t;
			DesuraId id;
			gcRefPtr<CDKeyTask> task;
		};


		class CDKeyTask : public UserTask
		{
		public:
			CDKeyTask(gcRefPtr<UserCore::UserI> user, DesuraId id);
			void doTask();

			Event<CDKeyEventInfo<gcString> > onCompleteEvent;
			Event<CDKeyEventInfo<gcException> > onErrorEvent;

			const char* getName(){ return "CDKeyTask"; }
		};

		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////

		class MigrateStandaloneTask : public UserTask
		{
		public:
			MigrateStandaloneTask(gcRefPtr<UserCore::UserI> user, const std::vector<UTIL::FS::Path> &fileList);
			void doTask();
			const char* getName(){ return "MigrateStandaloneTask"; }

		private:
			std::vector<UTIL::FS::Path> m_vFileList;
		};

		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////

		class RegenLaunchScriptsTask : public UserTask
		{
		public:
			RegenLaunchScriptsTask(gcRefPtr<UserCore::UserI> user);
			void doTask();
			const char* getName(){ return "RegenLaunchScriptsTask"; }
		};
	}
}

#endif //DESURA_USERTASKS_H
