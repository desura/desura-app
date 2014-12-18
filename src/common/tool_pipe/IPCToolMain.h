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
	gcException installTool(gcRefPtr<UserCore::ToolInfo> info);
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
