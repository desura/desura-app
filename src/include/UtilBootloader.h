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

#ifndef DESURA_BOOTLOADER_UTIL_H
#define DESURA_BOOTLOADER_UTIL_H
#ifdef _WIN32
#pragma once
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif	


#ifdef _WIN32
#include <windows.h> 
#endif

#include <vector>

namespace BootLoaderUtil
{
	unsigned int GetOSId();

	void SetCurrentDir();
	bool SetDllDir(const char* dir);

	void InitCommonControls();
	void WaitForDebugger();

	bool Restart(const char* args = nullptr, bool wait = true);
	bool RestartAsAdmin(const char* args = nullptr);
	bool RestartAsNormal(const char* args = nullptr);

	void GetLastFolder(char* dest, size_t destSize, const char* src);
	bool IsExeRunning(char* pName);

#ifdef WIN32
	bool CheckForOtherInstances(HINSTANCE hinstant);
	void WaitForOtherInstance(HINSTANCE hinstant);
#else
	bool CheckForOtherInstances(int hinstant);
	void WaitForOtherInstance(int hinstant);
#endif
	void WaitForOtherInstance(char* name);

	bool StartProcess(const char* name, const char* args);

	void PreReadImage(const char* imagePath);
}

#endif //DESURA_BOOTLOADER_UTIL_H
