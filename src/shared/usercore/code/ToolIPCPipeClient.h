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

#ifndef DESURA_TOOLIPCPIPECLIENT_H
#define DESURA_TOOLIPCPIPECLIENT_H
#ifdef _WIN32
#pragma once
#endif

#include "IPCPipeClient.h"


class IPCToolMain;

namespace UserCore
{

class ToolIPCPipeClient : public IPC::PipeClient
{
public:
	ToolIPCPipeClient(const char* user, bool uploadDumps, const char* key, HWND handle);
	~ToolIPCPipeClient();

	void start();

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
