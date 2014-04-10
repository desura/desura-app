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

#ifndef DESURA_IPCTOOLMAIN_H
#define DESURA_IPCTOOLMAIN_H
#ifdef _WIN32
#pragma once
#endif

#include "IPCClass.h"

#ifndef DESURA_CLIENT
class ToolInstallThread;
#else
namespace UserCore
{
	class ToolInfo;
}
#endif


class IPCToolMain : public IPC::IPCClass
{
public:
	IPCToolMain(IPC::IPCManager* mang, uint32 id, DesuraId itemId);
	~IPCToolMain();

	void message(int type, const char* msg, uint64 col, std::map<std::string, std::string> *mpArgs);

#ifdef DESURA_CLIENT
	gcException installTool(UserCore::ToolInfo* info);
#else
	static void SetupLogging();
#endif

	//! Async. Wait for onCompleteEvent
	gcException installTool(const char* exe, const char* args);
	void setCrashSettings(const char* user, bool upload);

	Event<int32> onCompleteEvent;
	Event<gcException> onErrorEvent;

private:
#ifndef DESURA_CLIENT
	ToolInstallThread *m_pThread = nullptr;
#endif

	void registerFunctions();
};

#endif //DESURA_IPCTOOLMAIN_H
