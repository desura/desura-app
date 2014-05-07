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
	gcException installTool(UserCore::ToolInfo* toolInfo);
	Event<int32> onCompleteEvent;
};
#endif

namespace UserCore
{
	namespace Misc
	{

		class ToolInstallThread : public ::Thread::BaseThread
		{
		public:
#ifdef WIN32
			ToolInstallThread(ToolManager* toolManager, std::mutex &mapLock, std::map<ToolTransactionId, ToolTransInfo*> &transactions, const char* userName, HWND handle);
#else
			ToolInstallThread(ToolManager* toolManager, std::mutex &mapLock, std::map<ToolTransactionId, ToolTransInfo*> &transactions);
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
			std::map<ToolTransactionId, Misc::ToolTransInfo*> &m_mTransactions;

			::Thread::WaitCondition m_InstallWait;
			std::mutex m_InstallLock;

			std::deque<ToolTransactionId> m_dvInstallQue;

			DesuraId m_CurrentInstallId;
			ToolTransactionId m_CurrentInstall;

			ToolManager* m_pToolManager;


#ifdef WIN32
			ToolIPCPipeClient* m_pIPCClient;
			HWND m_WinHandle;
			gcString m_szUserName;
#else
			IPCToolMain* m_pToolMain;
#endif

			bool m_bStillInstalling;
		};
	}
}

#endif //DESURA_TOOLINSTALLTHREAD_H
