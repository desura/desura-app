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

#ifndef DESURA_GATHERINFOTHREAD_H
#define DESURA_GATHERINFOTHREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "BaseItemTask.h"
#include "usercore/ItemHelpersI.h"
#include "ItemHandle.h"


namespace UserCore
{
	namespace ItemTask
	{
		class GatherInfoTask : public BaseItemTask
		{
		public:
			GatherInfoTask(gcRefPtr<UserCore::Item::ItemHandleI> handle, MCFBranch branch, MCFBuild build, gcRefPtr<UserCore::Item::Helper::GatherInfoHandlerHelperI> &helper, uint32 flags);
			~GatherInfoTask();

		protected:
			virtual void onError(gcException& e);
			virtual void onComplete();

			virtual void cancel();

			void doRun();

			void checkRequirements();
			uint32 validate();

			bool isValidBranch();
			bool handleInvalidBranch();

			bool checkNullBranch(gcRefPtr<UserCore::Item::BranchInfoI> &branchInfo);

			void completeStage();
			void resetStage();

		private:
			gcRefPtr<UserCore::Item::Helper::GatherInfoHandlerHelperI> m_pGIHH;
			uint32 m_uiFlags;

			bool m_bFirstTime;
			volatile bool m_bCanceled;
		};
	}
}

#endif //DESURA_GATHERINFOTHREAD_H
