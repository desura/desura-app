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

#ifndef DESURA_COMPLEXLAUNCHSERVICE_H
#define DESURA_COMPLEXLAUNCHSERVICE_H
#ifdef _WIN32
#pragma once
#endif


#include "BaseItemServiceTask.h"
#include "mcfcore/ProgressInfo.h"

class IPCComplexLaunch;

namespace UserCore
{

	namespace Item
	{
		namespace Helper
		{
			class InstallerHandleHelperI;
		}
	}

	namespace ItemTask
	{


		class ComplexLaunchServiceTask : public BaseItemServiceTask
		{
		public:
			ComplexLaunchServiceTask(UserCore::Item::ItemHandle *handle, bool clean, MCFBranch branch, MCFBuild build, UserCore::Item::Helper::InstallerHandleHelperI* ihh);
			~ComplexLaunchServiceTask();

			void completeStage()
			{
				m_bLaunch = false;
				m_bCompleteStage = true;
			}

			void launch()
			{
				m_bLaunch = true;
				m_bCompleteStage = false;
			}

		protected:
			bool initService();
			void onComplete();

			void onProgress(MCFCore::Misc::ProgressInfo& prog);
			void onError(gcException &e);

			bool backUp();
			bool install();
			bool remove();
			bool removeAndInstall();

			void completeRemove();
			void completeInstall();

			gcString getFullMcf();
			void onFinish();

			void onTrueComplete();
			bool isFilesToRestore();

		private:
			bool m_bClean;
			bool m_bHashMissMatch;

			uint8 m_iTier;
			uint8 m_iMode;

			DesuraId m_iRemoveId;

			std::shared_ptr<IPCComplexLaunch> m_pIPCIM;

			bool m_bCompleteStage;
			bool m_bLaunch;

			UserCore::Item::Helper::InstallerHandleHelperI* m_pIHH;
		};
	}
}

#endif //DESURA_COMPLEXLAUNCHSERVICE_H
