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

#ifndef DESURA_DOWNLOADTOOLITEMTASK_H
#define DESURA_DOWNLOADTOOLITEMTASK_H
#ifdef _WIN32
#pragma once
#endif

#include "BaseItemTask.h"
#include "usercore/ItemHelpersI.h"
#include "ToolManager.h"

#include "util_thread/BaseThread.h"

namespace UserCore
{
	namespace ItemTask
	{
		class DownloadToolTask : public UserCore::ItemTask::BaseItemTask
		{
		public:
			DownloadToolTask(gcRefPtr<UserCore::Item::ItemHandleI> handle, ToolTransactionId ttid, const char* downloadPath, MCFBranch branch, MCFBuild build);
			DownloadToolTask(gcRefPtr<UserCore::Item::ItemHandleI> handle, bool launch, ToolTransactionId ttid = UINT_MAX);

			virtual ~DownloadToolTask();
			virtual void cancel();

		protected:
			void doRun();

			virtual void onPause();
			virtual void onUnpause();
			virtual void onStop();

			void onDLProgress(UserCore::Misc::ToolProgress &p);
			void onDLError(gcException &e);
			void onDLComplete();

			void onComplete();
			void validateTools();

		private:
			gcString m_szDownloadPath;
			ToolTransactionId m_ToolTTID = UINT_MAX;

			bool m_bLaunch = false;
			bool m_bCancelled = false;

			::Thread::WaitCondition m_WaitCond;
		};
	}
}

#endif //DESURA_DOWNLOADTOOLITEMTASK_H
