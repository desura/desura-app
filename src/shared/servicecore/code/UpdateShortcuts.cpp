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

#include "Common.h"

#include <branding/branding.h>
#include "umcf/UMcf.h"

#ifdef WIN32
#include <ShlObj.h>
#endif

#ifdef WIN32
void UpdateShortCuts()
{
	char curDir[255];
	GetCurrentDirectory(255, curDir);

	gcString exe(curDir);
	gcString uninstall(curDir);
	gcString wd(curDir);

	exe += "\\desura.exe";
	uninstall += "\\Desura_Uninstaller.exe";

	gcWString folder = UTIL::OS::getConfigValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\Desura\\DesuraApp\\StartMenuFolder");

	if (folder == L"")
		folder = PRODUCT_NAME;

	gcString startFolder = UTIL::OS::getStartMenuProgramsPath(folder);

	gcString spOne = startFolder + "\\" PRODUCT_NAME ".lnk";
	gcString spTwo = startFolder + "\\" PRODUCT_NAME " (force update).lnk";
	gcString spThree = startFolder + "\\Uninstall " PRODUCT_NAME ".lnk";

	OS_VERSION ver = UTIL::WIN::getOSId();

	bool force = ((ver == WINDOWS_XP || ver == WINDOWS_XP64) && UTIL::FS::isValidFolder(startFolder));

	if (UTIL::FS::isValidFile(spOne) || force)
	{
		UTIL::FS::delFile(spOne);
		UTIL::WIN::createShortCut(gcWString(spOne).c_str(), exe.c_str(), wd.c_str(), "", false);
	}

#ifdef DESURA_OFFICIAL_BUILD
	if (UTIL::FS::isValidFile(spTwo) || force)
	{
		UTIL::FS::delFile(spTwo);
		UTIL::WIN::createShortCut(gcWString(spTwo).c_str(), exe.c_str(), wd.c_str(), "-forceupdate", true);
	}
#endif

	if (UTIL::FS::isValidFile(spThree) || force)
	{
		UTIL::FS::delFile(spThree);
		UTIL::WIN::createShortCut(gcWString(spThree).c_str(), uninstall.c_str(), wd.c_str(), "", true);
	}

	UMcf u;
	u.setRegValues(curDir);


	UTIL::WIN::delRegValue(gcString("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers\\{0}", UTIL::STRING::urlEncode(exe)));
	UTIL::WIN::delRegValue(gcString("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers\\{0}", UTIL::STRING::urlEncode(exe)));
}
#else

void UpdateShortCuts()
{
}

#endif
