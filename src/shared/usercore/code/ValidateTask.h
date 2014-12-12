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

#ifndef DESURA_VALIDATETASK_H
#define DESURA_VALIDATETASK_H
#ifdef _WIN32
#pragma once
#endif

#include "BaseItemTask.h"

namespace UserCore
{
	namespace Task
	{
		class DownloadBannerTask;
	}

	namespace ItemTask
	{
		class ValidateTask : public UserCore::ItemTask::BaseItemTask
		{
		public:
			ValidateTask(gcRefPtr<UserCore::Item::ItemHandleI> handle, MCFBranch branch, MCFBuild build = MCFBuild());
			virtual ~ValidateTask();

		protected:
			void doRun() override;

			void onStop() override;
			void onPause() override;
			void onUnpause() override;
			void cancel() override;

			void onError(gcException &e);
			void onComplete(gcString &savePath);
			void onProgress(MCFCore::Misc::ProgressInfo& p);

			void updateStatusFlags();
			void validateHeader(MCFBuild &build, MCFBranch &branch);

			bool checkExistingMcf(gcString savePath);
			void copyLocalMcfs(MCFBranch branch, MCFBuild build);
			void copyLocalFiles();

			void onLocalFileProgress(MCFCore::Misc::ProgressInfo& p);

		private:
			enum class ACTION
			{
				CHECK_LOCALFILES,
				CHECK_EXISTINGMCF,
				CHECK_LOCALMCFS,
				PRE_ALLOCATING,
			};

			void setAction(ACTION action);

			bool m_bInError = false;
			bool m_bUpdating = false;
			bool m_bUnAuthed = false;
			bool m_bLocalMcf = false;
			bool m_bLeaveLocalFiles = false;

			MCFBuild m_LastInsBuild;
			gcString m_szInstallPath;

			ACTION m_Action = ACTION::CHECK_LOCALFILES;

			std::vector<uint32> m_vMcfFileCountList;

			uint32 m_CurMcfIndex = 0;
			uint32 m_TotalFileCount = 0;

			std::mutex m_McfLock;
			McfHandle *m_CurrentMcf = nullptr;
		};
	}
}


#endif //DESURA_VALIDATETASK_H
