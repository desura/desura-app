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

#ifndef DESURA_UNINSTALLTHREAD_H
#define DESURA_UNINSTALLTHREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "UIBaseServiceTask.h"

class IPCUninstallMcf;

namespace UserCore
{
	namespace ItemTask
	{
		class UIServiceTask : public UIBaseServiceTask
		{
		public:
			UIServiceTask(gcRefPtr<UserCore::Item::ItemHandleI> handle, bool removeAll, bool removeAcc);
			~UIServiceTask();

		protected:
			virtual bool initService();
			virtual void onComplete();

			virtual void onServiceError(gcException& e);

			void onError(gcException &e) override
			{
				onServiceError(e);
			}

		private:
			std::shared_ptr<IPCUninstallMcf> m_pIPCUI;

			bool m_bRemoveAll;
			bool m_bRemoveAcc;
			volatile bool m_bRunning;
		};
	}
}

#endif
