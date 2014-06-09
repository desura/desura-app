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

#ifndef DESURA_ITEMTHREAD_H
#define DESURA_ITEMTHREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "util_thread/BaseThread.h"
#include "BaseItemTask.h"
#include "usercore/UserThreadManagerI.h"


namespace UserCore
{
	class UserThreadManagerI;

	namespace Item
	{

		class ItemThread : public ::Thread::BaseThread, public UserThreadProxyI
		{
		public:
			ItemThread(gcRefPtr<UserCore::Item::ItemHandle> handle);
			~ItemThread();

			void setThreadManager(const gcRefPtr<UserCore::UserThreadManagerI> &tm);
			void setWebCore(gcRefPtr<WebCore::WebCoreI> wc);
			void setUserCore(gcRefPtr<UserCore::UserI> uc);

			void queueTask(const gcRefPtr<UserCore::ItemTask::BaseItemTask> &task);

			void purge();

			void cancelCurrentTask();

			//! Is current task running
			//!
			//! @return True if running, false if not
			//!
			bool isRunningTask(){ return m_bRunningTask; }

			bool hasTaskToRun();

			Event<ITEM_STAGE> onTaskCompleteEvent;
			Event<ITEM_STAGE> onTaskStartEvent;

			::Thread::BaseThread* getThread() override
			{
				return this;
			}

			void cleanup() override
			{
				gcAssert(isStopped() || !isRunning());

				purge();

				m_pThreadManager.reset();
				m_pWebCore.reset();
				m_pUserCore.reset();
			}

		protected:
			void run();

			void onPause();
			void onUnpause();
			void onStop();

			bool performTask();
			gcRefPtr<UserCore::ItemTask::BaseItemTask> getNewTask();

		private:
			std::deque<gcRefPtr<UserCore::ItemTask::BaseItemTask>> m_vTaskList;
			gcRefPtr<UserCore::ItemTask::BaseItemTask> m_pCurrentTask;

			::Thread::WaitCondition m_WaitCond;

			std::mutex m_TaskMutex;
			std::mutex m_DeleteMutex;

			bool m_bRunningTask = false;
			bool m_bDeleteCurrentTask = false;

			gcRefPtr<UserCore::UserThreadManagerI> m_pThreadManager;
			gcRefPtr<WebCore::WebCoreI> m_pWebCore;
			gcRefPtr<UserCore::UserI> m_pUserCore;

			const gcString m_szBaseName;
		};
	}
}

#endif //DESURA_ITEMTHREAD_H
