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

#ifndef DESURA_INSTALLTHREAD_H
#define DESURA_INSTALLTHREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "BaseItemServiceTask.h"


class IPCInstallMcf;

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

		class InstallServiceTask : public BaseItemServiceTask
		{
		public:
			InstallServiceTask(gcRefPtr<UserCore::Item::ItemHandleI> handle, const char* path, MCFBranch branch, gcRefPtr<UserCore::Item::Helper::InstallerHandleHelperI> &ihh);
			~InstallServiceTask();


		protected:
			bool initService();
			void onComplete();

			virtual void onPause();
			virtual void onUnpause();
			virtual void onStop();

			void onFinish();
			void onProgUpdate(MCFCore::Misc::ProgressInfo& info);
			void onError(gcException &e);

			gcString getCurrentMcf();
			gcString downloadMcfHeader();

		private:
			std::shared_ptr<IPCInstallMcf> m_pIPCIM;

			bool m_bHasError;
			bool m_bInstalling;
			bool m_bHashMissMatch;
			gcString m_szPath;

			gcRefPtr<UserCore::Item::Helper::InstallerHandleHelperI> m_pIHH;
		};

	}
}


#endif
