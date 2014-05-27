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

#include "stdafx.h"
#include "UpdateFunctions.h"
#include <branding/branding.h>

#include <shlobj.h>
#include <sys/stat.h> 

#define UTILWEB_INCLUDE

#include "Common.h"
#include "AppUpdateInstall.h"
#include "IPCPipeClient.h"
#include "IPCManager.h"
#include "IPCUpdateApp.h"

#include "umcf/UMcf.h"

void SetRegValues();
void InstallService();

#ifdef DESURA_OFFICIAL_BUILD
int NeedUpdateNonGpl();
#endif


bool FolderExists(const char* strFolderName)
{   
	if (!strFolderName)
		return false;

	return GetFileAttributes(strFolderName) != INVALID_FILE_ATTRIBUTES;   
}

bool FolderExists(const wchar_t* strFolderName)
{   
	if (!strFolderName)
		return false;

	return GetFileAttributesW(strFolderName) != INVALID_FILE_ATTRIBUTES;   
}

bool FileExists(const wchar_t* fileName) 
{
	struct _stat64i32 stFileInfo;
	int intStat;

	// Attempt to get the file attributes
	intStat = _wstat(fileName, &stFileInfo);

	if (intStat == 0) 
		return true;
	else 
		return false;
}

#ifdef DESURA_OFFICIAL_BUILD
extern FILE* g_pUpdateLog;
#endif

static int GetServiceStatus()
{
	uint32 sres = UTIL::WIN::queryService(SERVICE_NAME);

	if (sres == SERVICE_STATUS_NOTINSTALLED || sres == SERVICE_STATUS_UNKNOWN)
		return UPDATE_SERVICE;

	char regname[MAX_PATH] = {0};
	char commonFiles[MAX_PATH] = {0};


	gcWString localPath = UTIL::OS::getCurrentDir(L"desura_service.exe");
	gcWString servicePath = UTIL::OS::getCommonProgramFilesPath(L"desura_service.exe");

	Safe::snprintf(regname,	255, "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\services\\%s\\ImagePath", SERVICE_NAME);
	gcWString regValue = UTIL::WIN::getRegValue(regname);
	
	bool regMatchesPath = (regValue == servicePath);
	bool regMatchesLocal = (regValue == localPath);

	if (FileExists(servicePath.c_str()) == false)
	{
		if (regMatchesLocal)
			return UPDATE_SERVICE_PATH;
		else
			return UPDATE_SERVICE_LOCATION;
	}
	else if (!regMatchesPath)
	{
		if (FileExists(servicePath.c_str()))
			return UPDATE_SERVICE_PATH;
		else
			return UPDATE_SERVICE_LOCATION;
	}

	std::string serviceHash = UTIL::MISC::hashFile(gcString(servicePath));
	std::string localHash = UTIL::MISC::hashFile(gcString(localPath));

	if (serviceHash != localHash)
		return UPDATE_SERVICE_HASH;

	Safe::snprintf(regname, 255, "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\%s\\Start", SERVICE_NAME);
	int res = UTIL::WIN::getRegValueInt(regname, true);

#ifdef DESURA_OFFICIAL_BUILD
	if (g_pUpdateLog)
		fprintf(g_pUpdateLog, "Status: %d\n", res);
#endif

	if (res == 4)
		return UPDATE_SERVICE_DISABLED;

	return UPDATE_NONE;
}

int IsServiceInstalled()
{
	auto res = GetServiceStatus();
	gcTraceS("Status: {0}", res);
	return res;
}

int NeedUpdate()
{
#ifdef DESURA_OFFICIAL_BUILD
	int res = NeedUpdateNonGpl();
	gcTraceS("Status: {0}", res);

	if (res != UPDATE_NONE)
		return res;
#endif

	return IsServiceInstalled();
}

void SetRegValues()
{
	UMcf u;
	u.setRegValues();
}

void InstallService()
{
	unsigned int res = UTIL::WIN::queryService(SERVICE_NAME);
	gcTraceS("Status: {0}", res);

	char servicePath[255];

	char curDir[255];
	GetCurrentDirectory(255, curDir);
	Safe::snprintf(servicePath, 255, "%s\\desura_service.exe", curDir);

	if (res == SERVICE_STATUS_NOTINSTALLED)
	{
		UTIL::WIN::installService(SERVICE_NAME, servicePath, SERVICE_DESC);

		//this is a hack for a crash that happens in xp when you try and change the service access rights.
		//As xp has no uac its not needed.
		unsigned int osid = UTIL::WIN::getOSId();
		if (!(osid == WINDOWS_XP || osid == WINDOWS_XP64))
			UTIL::WIN::changeServiceAccess(SERVICE_NAME);
	}
	else
	{
		//make sure it is stopped
		UTIL::WIN::stopService(SERVICE_NAME);

		char regname[255];
		Safe::snprintf(regname, 255, "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\%s\\ImagePath", SERVICE_NAME);
		
		//update its path to the new location
		UTIL::WIN::setRegValue(regname, servicePath, false, true);
	}
}

class ServiceInstaller : public AppUpdateInstall
{
public:
	ServiceInstaller() : AppUpdateInstall(nullptr, false)
	{
	}

	virtual int run()
	{
		try
		{
			startService();
			IPC::PipeClient pc("DesuraIS");

			pc.setUpPipes();
			pc.start();

			std::shared_ptr<IPCUpdateApp> ipua = IPC::CreateIPCClass<IPCUpdateApp>(&pc, "IPCUpdateApp");

			if (!ipua)
				return -1;

			char curDir[255];
			GetCurrentDirectory(255, curDir);

			if (!ipua->updateService(curDir))
				return -3;
		}
		catch (gcException)
		{
			return -2;
		}

		return 0;
	}
};

bool ServiceUpdate(bool validService)
{
	gcTraceS("Valid: {0}", validService);

	if (validService)
	{
		ServiceInstaller si;

		if (si.run() != 0)
			return false;
	}
	else
	{
		std::wstring appPath = UTIL::OS::getCommonProgramFilesPath();

		if (!FolderExists(appPath.c_str()))
			CreateDirectoryW(appPath.c_str(), nullptr);

		std::wstring newService = UTIL::OS::getCommonProgramFilesPath(L"desura_service.exe");
		std::wstring curService = UTIL::OS::getCurrentDir(L"desura_service.exe");

		char regname[255];
		Safe::snprintf(regname, 255, "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\%s\\ImagePath", SERVICE_NAME);

		CopyFileW(curService.c_str(), newService.c_str(), FALSE);
		UTIL::WIN::setRegValue(regname, gcString(newService).c_str());
	}

	return true;
}

class DataMover : public AppUpdateInstall
{
public:
	DataMover() : AppUpdateInstall(nullptr, false)
	{
	}

	virtual int run()
	{
		try
		{
			startService();
			IPC::PipeClient pc("DesuraIS");

			pc.setUpPipes();
			pc.start();

			std::shared_ptr<IPCUpdateApp> ipua = IPC::CreateIPCClass<IPCUpdateApp>(&pc, "IPCUpdateApp");

			if (!ipua)
				return -1;

			if (!ipua->fixDataDir())
				return -3;
		}
		catch (gcException)
		{
			return -2;
		}

		return 0;
	}
};

bool MoveDataFolder()
{
	gcTraceS("");

	DataMover dm;

	if (dm.run() != 0)
		return false;

	return true;
}

bool FixServiceDisabled()
{
	gcTraceS("");

	try
	{
		UTIL::WIN::enableService(SERVICE_NAME);
	}
	catch (gcException)
	{
		MessageBox(nullptr, "The service desura needs to use to install content is disabled. \n\nPlease enable it via msconfig (\"" PRODUCT_NAME " Install Service\").", PRODUCT_NAME ": Failed to enable service", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	return true;
}

#ifndef DESURA_OFFICIAL_BUILD
void McfUpdate() {}
void FullUpdate() {}
#endif
