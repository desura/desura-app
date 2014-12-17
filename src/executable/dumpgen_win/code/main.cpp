/*
Copyright (C) 2008 Mark Chandler
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

#include "windows.h"
#include "resource.h"

#include "UtilBootloader.h"
#include "util\UtilWindows.h"
#include "MiniDumpGenerator.h"

#include "SharedObjectLoader.h"
#include "UICoreI.h"

#include <vector>

#include "DesuraWinApp.h"


void ProcessDump(const char* commandLine);

class BootLoader : public Desurium::CDesuraWinApp
{
public:
	BootLoader();
	~BootLoader();

	void InitInstance() override;
	int ExitInstance() override;

protected:
	bool sendArgs();
	void loadUICore();

	void restartAsAdmin(int needupdate);

private:
	SharedObjectLoader m_hUICore;
	UICoreI* m_pUICore;
};

BootLoader theApp;



BootLoader::BootLoader()
{
	BootLoaderUtil::InitCommonControls();

#if 0
	WaitForDebugger();
#endif

	m_pUICore = nullptr;
}

BootLoader::~BootLoader()
{
}

void BootLoader::InitInstance()
{
	BootLoaderUtil::SetCurrentDir();

	if (!BootLoaderUtil::SetDllDir(".\\bin"))
	{
		::MessageBox(nullptr, "Failed to set the dll path to the bin folder.", "Desura: ERROR!", MB_OK);
		exit(-100);
	}

	if (strstr(m_lpCmdLine,"-crashreport")!=nullptr)
	{
		ProcessDump(m_lpCmdLine);
		return;
	}

#ifndef DEBUG
	bool hasAdminRights = false;
	unsigned int osid = BootLoaderUtil::GetOSId();

	if (osid == WINDOWS_PRE2000)
	{
		::MessageBox(nullptr, "Desura needs Windows xp or better to run.", "Desura Error: Old Windows", MB_OK);
		return;
	}
	else if (osid == WINDOWS_XP || osid == WINDOWS_XP64)
	{
		hasAdminRights = true;
	}

	if (strstr(m_lpCmdLine,"-admin")!=nullptr)
	{
		hasAdminRights = true;
	}

	if (!hasAdminRights)
	{
		BootLoaderUtil::RestartAsAdmin("-admin");
		return;
	}
#endif

	loadUICore();

	if (!m_pUICore)
		return;

	char** argv = new char*[1];
	argv[0] = "-crashdump";

	m_pUICore->initWxWidgets(m_hInstance, m_nCmdShow, 1, argv);

	delete [] argv;
}

int BootLoader::ExitInstance()
{
	int ret = 0;

	if (m_pUICore)
		m_pUICore->exitApp(&ret);

	return ret;
}

void BootLoader::loadUICore()
{
	const char* dllname = "utilcore.dll";

	if (!m_hUICore.load(dllname))
	{
		DWORD err = GetLastError();
		::MessageBox(nullptr, "Failed to load utilcore.dll", "Desura: ERROR!",  MB_OK);
		exit(-200);
	}

	UICoreFP UICoreGetInterface = m_hUICore.getFunction<UICoreFP>("GetInterface");

	if (!UICoreGetInterface)
	{
		::MessageBox(nullptr, "Failed to load wxWidgets mappings in utilcore.dll", "Desura: ERROR!", MB_OK);
		exit(-500);
	}

	m_pUICore = UICoreGetInterface();
}
