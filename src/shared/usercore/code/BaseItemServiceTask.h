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
			BaseItemServiceTask(UserCore::Item::ITEM_STAGE type, const char* name, UserCore::Item::ItemHandle* handle, MCFBranch branch = MCFBranch(), MCFBuild = MCFBuild());
			~BaseItemServiceTask();

			bool hasStarted();

		protected:
			virtual bool initService() = 0;

			virtual void onStop();
			virtual void doRun();
			virtual void onFinish();



			IPC::ServiceMainI* getServiceMain();

			void waitForFinish();
			void resetFinish();
			void setFinished();

		private:
			::Thread::WaitCondition m_WaitCond;
			std::atomic<bool> m_bFinished;
			std::atomic<bool> m_bStarted;
		};
	}
}

#endif //DESURA_BASEITEMSERVICETASK_H
