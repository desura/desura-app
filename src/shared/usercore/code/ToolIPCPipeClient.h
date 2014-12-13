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

#ifndef DESURA_TOOLIPCPIPECLIENT_H
#define DESURA_TOOLIPCPIPECLIENT_H
#ifdef _WIN32
#pragma once
#endif

#include "IPCPipeClient.h"


class IPCToolMain;

namespace IPC
{
	class ServiceMainI;
}

namespace UserCore
{

	class ToolIPCPipeClient : public IPC::PipeClient
	{
	public:
		ToolIPCPipeClient(const char* user, bool uploadDumps, const char* key, HWND handle);
		~ToolIPCPipeClient();

		void start(const std::shared_ptr<IPC::ServiceMainI> &pServiceMain);

		std::shared_ptr<IPCToolMain> getToolMain();

	protected:
		void stopHelper();
		void startHelper();

		void onDisconnect();

		void tryStart();

	private:
		bool m_bUploadDumps;

		gcString m_szUser;
		gcString m_szKey;

		std::shared_ptr<IPCToolMain> m_pServiceMain;

		HWND m_WinHandle;
	};
}

#endif //DESURA_TOOLIPCPIPECLIENT_H
