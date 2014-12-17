/*
Copyright (C) 2010 DesuraNet
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

#include "Common.h"

#include <string.h>

#include "CrashAlert.h"
#include "SharedObjectLoader.h"

#include <deque>

#include "UtilBootloader.h"

#ifdef WIN32
#include <process.h>
#include <Psapi.h>
#endif

#include "DesuraWnd.h"

using namespace Desurium;

UINT UploadDump(const std::string &strFile, const std::string &strUser, const std::string &strTracer);

bool RestartDesura(const char* args);
void GetBuildBranch(int &build, int &branch);
void GetString(const char* str, const char* input, char* out, size_t outSize);

typedef bool(*UploadCrashExFn)(const char* path, const char* user, int build, int branch, const char* szTracer);


void TerminateDesura()
{
	unsigned long aProcesses[1024], cbNeeded, cProcesses;
	if(!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		return;

	unsigned long curPID = GetCurrentProcessId();

	cProcesses = cbNeeded / sizeof(unsigned long);
	for (unsigned int i = 0; i < cProcesses; i++)
	{
		if(aProcesses[i] == 0)
			continue;

		if (aProcesses[i] == curPID)
			continue;

		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE | PROCESS_VM_READ, 0, aProcesses[i]);

		if (!hProcess)
			continue;

		char buffer[50] = {0};

		if (GetModuleBaseName(hProcess, 0, buffer, 50) > 0 && strcmp("desura.exe", buffer) == 0)
			TerminateProcess(hProcess, 0);

		CloseHandle(hProcess);
	}
}

#include <thread>

void ProcessDump(const char* m_lpCmdLine)
{
	UTIL::MISC::CMDArgs args(m_lpCmdLine);

	std::thread thread;
	std::string file;
	std::string user;
	std::string tracer;

	file.resize(255);
	user.resize(255);
	tracer.resize(255);

	bool msgbox = args.hasArg("msgbox");
	bool upload = (args.hasArg("noupload") == false);

	if (args.hasArg("file"))
		args.getString("file", &file[0], 255);

	if (args.hasArg("user"))
		args.getString("user", &user[0], 255);

	if (args.hasArg("tracer"))
		args.getString("tracer", &tracer[0], 255);

	if (file[0] && upload)
	{
		thread = std::thread([file, user, tracer](){
			try
			{
				UploadDump(file, user, tracer);
				TerminateDesura();
			}
			catch (...)
			{
			}
		});
	}
	else
	{
		TerminateDesura();
	}

	if (msgbox)
	{
		CrashAlert ca;
		int res = ca.DoModal();

		if (res == IDOK) //restart
		{
			RestartDesura("");
		}
		else if (res == IDYES) //force update
		{
			RestartDesura("-forceupdate");
		}
	}

	try
	{
		if (thread.get_id() != std::thread::id() && thread.joinable())
			thread.join();
	}
	catch (...)
	{
	}
}

SharedObjectLoader g_CrashUploader;

UINT UploadDump(const std::string &strFile, const std::string &strUser, const std::string &strTracer)
{
	int build = 0;
	int branch = 0;

	GetBuildBranch(build, branch);

	const char* modualName = "crashuploader.dll";

	if (!g_CrashUploader.load(modualName))
		return -1;

	UploadCrashExFn uploadCrash = g_CrashUploader.getFunction<UploadCrashExFn>("UploadCrashEx");

	if (!uploadCrash)
		return -2;

	if (!uploadCrash(strFile.c_str(), strUser.c_str(), build, branch, strTracer.c_str()))
		return -3;

	return 0;
}

bool RestartDesura(const char* args)
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

	PROCESS_INFORMATION ProcInfo = {0};
	STARTUPINFO StartupInfo = {0};

	char launchArg[512];
	const char* exeName = "desura.exe";

	_snprintf_s(launchArg, 512, _TRUNCATE, "%s %s", exeName, args?args:"");
	BOOL res = CreateProcess(nullptr, launchArg, nullptr, nullptr, false, NORMAL_PRIORITY_CLASS, nullptr, exePath, &StartupInfo, &ProcInfo);

	CloseHandle(ProcInfo.hProcess);
	CloseHandle(ProcInfo.hThread);

	return res?true:false;
}

void GetBuildBranch(int &build, int &branch)
{
	char lszValue[255];

	DWORD dwType=REG_SZ;
	DWORD dwSize=255;
	HKEY hk;

	DWORD err1 = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Desura\\DesuraApp", 0, KEY_QUERY_VALUE, &hk);

	lszValue[0] = 0;
	DWORD err2 = RegQueryValueEx(hk, "appid", nullptr, &dwType,(LPBYTE)&lszValue, &dwSize);

	if (dwType == REG_SZ)
		branch = Safe::atoi(lszValue);
	else
		RegQueryValueEx(hk, "appid", nullptr, &dwType, (LPBYTE)&branch, &dwSize);

	lszValue[0] = 0;
	dwSize = 255;
	DWORD err3 = RegQueryValueEx(hk, "appver", nullptr, &dwType,(LPBYTE)&lszValue, &dwSize);

	if (dwType == REG_SZ)
		build = Safe::atoi(lszValue);
	else
		RegQueryValueEx(hk, "appver", nullptr, &dwType, (LPBYTE)&build, &dwSize);

	RegCloseKey(hk);
}
