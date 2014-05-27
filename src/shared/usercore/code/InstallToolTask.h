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

#ifndef DESURA_INSTALLTOOLTASK_H
#define DESURA_INSTALLTOOLTASK_H
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
		class InstallToolTask : public UserCore::ItemTask::BaseItemTask
		{
		public:
			InstallToolTask(gcRefPtr<UserCore::Item::ItemHandleI> handle, bool launch);
			virtual ~InstallToolTask();

		protected:
			void doRun();

			virtual void onPause();
			virtual void onUnpause();
			virtual void onStop();

			void onIPCStart();
			void onINStart(DesuraId &id);
			void onINProgress(UserCore::Misc::ToolProgress &p);
			void onINError(gcException &e);
			void onINComplete();

			void onComplete();

		private:
			bool m_bInError;
			bool m_bLaunch;

			uint32 m_ToolTTID;
			::Thread::WaitCondition m_WaitCond;
		};
	}
}


#endif //DESURA_INSTALLTOOLTASK_H
