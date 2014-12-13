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

#ifndef DESURA_BASEITEMSERVICETASK_H
#define DESURA_BASEITEMSERVICETASK_H
#ifdef _WIN32
#pragma once
#endif

#include "BaseItemTask.h"
#include "util_thread/BaseThread.h"

#include <atomic>

namespace IPC
{
	class ServiceMainI;
}

namespace UserCore
{
	namespace ItemTask
	{

		class BaseItemServiceTask : public BaseItemTask
		{
		public:
			BaseItemServiceTask(UserCore::Item::ITEM_STAGE type, const char* name, gcRefPtr<UserCore::Item::ItemHandleI> &handle, MCFBranch branch = MCFBranch(), MCFBuild = MCFBuild());
			~BaseItemServiceTask();

			bool hasStarted();

		protected:
			virtual bool initService() = 0;

			void onStop() override;
			void doRun() override;
			virtual void onFinish();
			virtual void onError(gcException &e)
			{
				//should handle this
				gcAssert(false);
			}

			std::shared_ptr<IPC::ServiceMainI> getServiceMain();

			void waitForFinish();
			void resetFinish();
			void setFinished();

		private:
			::Thread::WaitCondition m_WaitCond;
            std::atomic<bool> m_bFinished = {false};
            std::atomic<bool> m_bStarted = {false};
		};
	}
}

#endif //DESURA_BASEITEMSERVICETASK_H
