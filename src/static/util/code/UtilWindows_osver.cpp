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
#include "util/UtilWindows.h"


#include <windows.h>
#include <tchar.h>
#include <stdio.h>
//#include <strsafe.h>

#ifdef TEXT
#undef TEXT
#define TEXT(a) a
#endif

static const std::map<OS_VERSION, std::string> gs_mWindowsVerList =
{
	{ WINDOWS_UNKNOWN, "Unknown" },
	{ WINDOWS_SERVER2008, "Server 2008" },
	{ WINDOWS_SERVER2003, "Server 2003" },
	{ WINDOWS_SERVER2000, "Server 2000" },
	{ WINDOWS_HOMESERVER, "Home Server" },
	{ WINDOWS_VISTA, "Vista" },
	{ WINDOWS_XP64, "XP64" },
	{ WINDOWS_XP, "XP" },
	{ WINDOWS_2000, "2000" },
	{ WINDOWS_PRE2000, "Pre 2000" },
	{ WINDOWS_7, "7" },
	{ WINDOWS_8, "8" },
	{ WINDOWS_SERVER2012, "Server 2012" },
	{ WINDOWS_81, "8.1" },
	{ WINDOWS_SERVER2012R2, "Server 2012 R2" },
	{ WINDOWS_SERVER2008R2, "Server 2008 R2" }
};

namespace UTIL
{
	namespace WIN
	{
		typedef bool (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, DWORD*);
		typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);

		OS_VERSION getOSId()
		{
			OS_VERSION ver = WINDOWS_UNKNOWN;

			OSVERSIONINFOEXA osvi;
			SYSTEM_INFO si;
			PGNSI pGNSI;
			BOOL bOsVersionInfoEx;

			ZeroMemory(&si, sizeof(SYSTEM_INFO));
			ZeroMemory(&osvi, sizeof(OSVERSIONINFOEXA));

			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);

			if (!(bOsVersionInfoEx = GetVersionExA((OSVERSIONINFOA*)&osvi)))
				return WINDOWS_PRE2000;

			// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
			pGNSI = (PGNSI)GetProcAddress(GetModuleHandleA(TEXT("kernel32.dll")), "GetNativeSystemInfo");

			if (nullptr != pGNSI)
				pGNSI(&si);
			else
				GetSystemInfo(&si);

			if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion > 4)
			{
				if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3)
				{
					if (osvi.wProductType == VER_NT_WORKSTATION)
						ver = WINDOWS_81;
					else
						ver = WINDOWS_SERVER2012R2;
				}
				else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2)
				{
					if (osvi.wProductType == VER_NT_WORKSTATION)
						ver = WINDOWS_8;
					else
						ver = WINDOWS_SERVER2012;
				}
				else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1)
				{
					if (osvi.wProductType == VER_NT_WORKSTATION)
						ver = WINDOWS_7;
					else
						ver = WINDOWS_SERVER2008R2;
				}
				else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
				{
					if (osvi.wProductType == VER_NT_WORKSTATION)
						ver = WINDOWS_VISTA;
					else
						ver = WINDOWS_SERVER2008;
				}
				else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
				{
					if (GetSystemMetrics(SM_SERVERR2))
						ver = WINDOWS_SERVER2003;
					else if (osvi.wSuiteMask == VER_SUITE_STORAGE_SERVER)
						ver = WINDOWS_SERVER2003;
					else if (osvi.wSuiteMask == VER_SUITE_WH_SERVER)
						ver = WINDOWS_HOMESERVER;
					else if (osvi.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
						ver = WINDOWS_XP64;
					else
						ver = WINDOWS_SERVER2003;
				}
				else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
				{
					ver = WINDOWS_XP;
				}
				else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
				{
					if (osvi.wProductType == VER_NT_WORKSTATION)
					{
						ver = WINDOWS_2000;
					}
					else
					{
						ver = WINDOWS_SERVER2000;
					}
				}
			}
			else
			{
				ver = WINDOWS_PRE2000;
			}

			return ver;
		}


		void getOSString(char* dest, size_t destSize)
		{
			auto ver = getOSId();

			auto it = gs_mWindowsVerList.find(ver);

			if (it == end(gs_mWindowsVerList))
			{
				Safe::strcpy(dest, destSize, "Non Supported version of windows.");
				return;
			}

			gcString winVer("Windows {0}", it->second);


			SYSTEM_INFO si;
			PGNSI pGNSI;
			ZeroMemory(&si, sizeof(SYSTEM_INFO));

			// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
			pGNSI = (PGNSI)GetProcAddress(GetModuleHandleA(TEXT("kernel32.dll")), "GetNativeSystemInfo");

			if (nullptr != pGNSI)
				pGNSI(&si);
			else
				GetSystemInfo(&si);

			if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
				winVer += ", 64-bit";
			else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
				winVer += ", 32-bit";
			else
				winVer += ", other";

			OSVERSIONINFOEXA osvi;
			ZeroMemory(&osvi, sizeof(OSVERSIONINFOEXA));
			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);

			if (GetVersionExA((LPOSVERSIONINFOA)&osvi))
			{
				// Include service pack (if any) and build number.
				if (osvi.szCSDVersion[0] != '0')
					winVer += gcString(" {0}", osvi.szCSDVersion);

				winVer += gcString(" (build {0})", osvi.dwBuildNumber);
			}
		
			Safe::strcpy(dest, destSize, winVer.c_str());
		}
	}
}