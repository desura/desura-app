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

#ifndef DESURA_UICOMPLEXMODSERVICETASK_H
#define DESURA_UICOMPLEXMODSERVICETASK_H
#ifdef _WIN32
#pragma once
#endif



#include "UIBaseServiceTask.h"


class IPCComplexLaunch;

namespace UserCore
{
	namespace ItemTask
	{
		class UIComplexModServiceTask : public UIBaseServiceTask
		{
		public:
			UIComplexModServiceTask(gcRefPtr<UserCore::Item::ItemHandleI> handle, MCFBranch installBranch, MCFBuild installBuild);
			~UIComplexModServiceTask();

			void setCALaunch();
			void setCAUIPatch();
			void setCAInstall();
			void setCAUninstallBranch();
			void setCAUninstall(bool removeAll, bool removeAccount);

			void setEndStage();

		protected:
			virtual bool initService();
			virtual void onComplete();

			void onProgress(MCFCore::Misc::ProgressInfo& p);

		private:
			bool m_bRemoveAll;
			bool m_bRemoveAcc;
			bool m_bEndStage;

			uint8 m_uiCompleteAction;
			std::shared_ptr<IPCComplexLaunch> m_pIPCCL;
		};
	}
}


#endif //DESURA_UICOMPLEXMODSERVICETASK_H
