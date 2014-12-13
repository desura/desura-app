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
			DownloadUpdateTask(gcRefPtr<UserCore::UserI> user, uint32 appver, uint32 build, bool bForced);

			void doTask();

			//i overload the ptr as a boolean value to test if we need to display the update window
			Event<uint32> onDownloadProgressEvent;
			Event<UserCore::Misc::update_s> onDownloadCompleteEvent;
			Event<UserCore::Misc::update_s> onDownloadStartEvent;

			const char* getName(){ return "DownloadUpdateTask"; }

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
