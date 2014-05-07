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

#ifndef DESURA_USERIPCPIPECLIENT_H
#define DESURA_USERIPCPIPECLIENT_H
#ifdef _WIN32
#pragma once
#endif

#ifdef NIX
#include "SharedObjectLoader.h"
#endif

#include "IPCPipeClient.h"

class IPCServiceMain;
class IPCServerI;

namespace UserCore
{
	class UserIPCPipeClient : public IPC::PipeClient
	{
	public:
		UserIPCPipeClient(const char* user, const char* appDataPath, bool uploadDumps);
		~UserIPCPipeClient();
#ifdef WIN32
		void restart();
#endif
		void start();

		std::shared_ptr<IPCServiceMain> getServiceMain()
		{
			return m_pServiceMain;
		}

	protected:
#ifdef WIN32
		void stopService();
		void startService();

		void onDisconnect();
#else
		static void recvMessage(void* obj, const char* buffer, size_t size);
		void recvMessage(const char* buffer, size_t size);
		static void sendMessage(void* obj, const char* buffer, size_t size);
		void sendMessage(const char* buffer, size_t size);
		virtual void setUpPipes();
#endif

	private:
		bool m_bUploadDumps;
		gcString m_szUser;
		gcString m_szAppDataPath;

		std::shared_ptr<IPCServiceMain> m_pServiceMain;

#ifdef NIX
		SharedObjectLoader m_hServiceDll;
		IPCServerI *m_pServer;
#endif
	};
}

#endif //DESURA_USERIPCPIPECLIENT_H
