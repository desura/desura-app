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



#include "Common.h"
#include "IPCPipeServer.h"
#include "util_thread\BaseThread.h"
#include "MiniDumpGenerator.h"
#include "SharedObjectLoader.h"


typedef BOOL (WINAPI* WaitForDebuggerFunc)();
typedef BOOL (WINAPI* SetDllDirectoryFunc)(LPCTSTR lpPathName);

namespace BootLoaderUtil
{
	bool SetDllDir(const char* dir)
	{
		SharedObjectLoader sol;

		if (sol.load("kernel32.dll"))
		{
			SetDllDirectoryFunc set_dll_directory = sol.getFunction<SetDllDirectoryFunc>("SetDllDirectoryA");
	
			if (set_dll_directory && set_dll_directory(dir)) 
				return true;
		}

		return false;
	}

	void SetCurrentDir()
	{
		char exePath[255];
		GetModuleFileName(nullptr, exePath, 255);

		size_t exePathLen = strlen(exePath);
		for (size_t x=exePathLen; x>0; x--)
		{
			if (exePath[x] == '\\')
				break;
			else
				exePath[x] = '\0';
		}

		SetCurrentDirectory(exePath);
	}

	void WaitForDebugger()
	{
		HMODULE kernel32_dll = GetModuleHandle("kernel32.dll");
		if (kernel32_dll != nullptr)
		{
			WaitForDebuggerFunc waitfor_debugger = (WaitForDebuggerFunc)GetProcAddress(kernel32_dll, "IsDebuggerPresent");
	
			if (waitfor_debugger != nullptr) 
			{
				while( !waitfor_debugger() )
					Sleep( 500 );
			}
		}
	}
}

class Color;

void LogMsg(MSG_TYPE type, std::string msg, Color* col, std::map<std::string, std::string> *mpArgs)
{
	fprintf(stdout, "%s", msg.c_str());
}

#include "DesuraPrintFRedirect.h"


::Thread::WaitCondition g_WaitCond;
volatile bool g_bClientConnected = false;

void OnPipeDisconnect(uint32 &e)
{
	g_WaitCond.notify();
}

void OnPipeConnect(uint32 &e)
{
	g_bClientConnected = true;
	g_WaitCond.notify();
}

bool InitWebControl(const char* hostName, const char* userAgent);
void ShutdownWebControl();

int CALLBACK WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef DEBUG
	//BootLoaderUtil::WaitForDebugger();
#endif

#ifndef DEBUG
	MiniDumpGenerator miniDump;
	miniDump.showMessageBox(false);
#endif

	BootLoaderUtil::SetCurrentDir();

	if (!BootLoaderUtil::SetDllDir(".\\bin"))
		exit(-100);			

	std::vector<std::string> out;
	UTIL::STRING::tokenize(gcString(lpCmdLine), out, " ");

	std::string userAgent;

	UTIL::STRING::base64_decode(out[2], [&userAgent](const unsigned char* buff, size_t size) -> bool
	{
		userAgent.append((const char*)buff, size);
		return true;
	});

	InitWebControl(out[1].c_str(), userAgent.c_str());

	{
		IPC::PipeServer pipeserver(out[0].c_str(), 1, true);

		pipeserver.onConnectEvent += delegate(&OnPipeConnect);
		pipeserver.onDisconnectEvent += delegate(&OnPipeDisconnect);

		pipeserver.start();

		g_WaitCond.wait(15);

		if (g_bClientConnected)
			g_WaitCond.wait();
	}

	ShutdownWebControl();
}