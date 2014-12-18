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

#ifndef DESURA_TOOLINSTALLTHREAD_H
#define DESURA_TOOLINSTALLTHREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "util_thread/BaseThread.h"
#include "ToolManager.h"

#ifdef WIN32
class IPCToolMain;
#else
namespace UserCore
{
	class ToolInfo;
}

class IPCToolMain
{
public:
	gcException installTool(gcRefPtr<UserCore::ToolInfo> toolInfo);
	Event<int32> onCompleteEvent;
};
#endif

namespace UserCore
{
	class UserInternalI;

	namespace Misc
	{

		class ToolInstallThread : public ::Thread::BaseThread, public gcRefBase
		{
		public:
#ifdef WIN32
			ToolInstallThread(gcRefPtr<UserInternalI> pUserInternal, gcRefPtr<ToolManager> toolManager, std::mutex &mapLock, std::map<ToolTransactionId, gcRefPtr<ToolTransInfo>> &transactions, const char* userName, HWND handle);
#else
			ToolInstallThread(gcRefPtr<ToolManager> toolManager, std::mutex &mapLock, std::map<ToolTransactionId, gcRefPtr<ToolTransInfo>> &transactions);
#endif

			~ToolInstallThread();

			void startInstall(ToolTransactionId ttid);
			void cancelInstall(ToolTransactionId ttid);

			EventV onPipeDisconnectEvent;
			EventV onFailedToRunEvent;

		protected:
			void run();
			void onStop();
			void startIPC();

			void onINComplete(int32 &result);
			void onINError(gcException &error);

			void startInstallItem(uint32 index);

			void onPipeDisconnect();

			void doNextInstall();
			void doFirstInstall();
			void hideForm();

			bool hasToolMain();

			std::shared_ptr<IPCToolMain> getToolMain();
			bool preInstallStart();

		private:
			std::mutex &m_MapLock;
			std::map<ToolTransactionId, gcRefPtr<Misc::ToolTransInfo>> &m_mTransactions;

			::Thread::WaitCondition m_InstallWait;
			std::mutex m_InstallLock;

			std::deque<ToolTransactionId> m_dvInstallQue;

			DesuraId m_CurrentInstallId;
			ToolTransactionId m_CurrentInstall = -1;

			gcRefPtr<ToolManager> m_pToolManager;


#ifdef WIN32
			std::shared_ptr<ToolIPCPipeClient> m_pIPCClient;
			HWND m_WinHandle;
			gcString m_szUserName;
			gcRefPtr<UserInternalI> m_pUserInternal;
#else
            std::shared_ptr<IPCToolMain> m_pToolMain;
#endif

			bool m_bStillInstalling = false;

			gc_IMPLEMENT_REFCOUNTING(ToolInstallThread)
		};
	}
}

#endif //DESURA_TOOLINSTALLTHREAD_H
