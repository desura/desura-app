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
						ver = WINDOWS_SERVER2008;
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

		// The following code was taken from http://msdn.microsoft.com/en-us/library/ms724429(VS.85).aspx and remains the property of its respected owner.
		void getOSString(char* dest, size_t destSize)
		{
#define BUFSIZE 1000
			char pszOS[BUFSIZE];

			OSVERSIONINFOEXA osvi;
			SYSTEM_INFO si;
			PGNSI pGNSI;
			PGPI pGPI;
			BOOL bOsVersionInfoEx;
			DWORD dwType;

			ZeroMemory(&si, sizeof(SYSTEM_INFO));
			ZeroMemory(&osvi, sizeof(OSVERSIONINFOEXA));

			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);

			if (!(bOsVersionInfoEx = GetVersionExA((LPOSVERSIONINFOA)&osvi)))
				return;

			// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
			pGNSI = (PGNSI)GetProcAddress(GetModuleHandleA(TEXT("kernel32.dll")), "GetNativeSystemInfo");

			if (nullptr != pGNSI)
				pGNSI(&si);
			else
				GetSystemInfo(&si);

			if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion > 4)
			{
				Safe::strcpy(pszOS, BUFSIZE, TEXT("Microsoft "));

				if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3)
				{
					if (osvi.wProductType == VER_NT_WORKSTATION)
						Safe::strcat(pszOS, BUFSIZE, TEXT("Windows 8.1"));
					else
						Safe::strcat(pszOS, BUFSIZE, TEXT("Windows Server 2012 R2"));
				}
				else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2)
				{
					if (osvi.wProductType == VER_NT_WORKSTATION)
						Safe::strcat(pszOS, BUFSIZE, TEXT("Windows 8"));
					else
						Safe::strcat(pszOS, BUFSIZE, TEXT("Windows Server 2012"));
				}
				else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1)
				{
					if (osvi.wProductType == VER_NT_WORKSTATION)
						Safe::strcat(pszOS, BUFSIZE, TEXT("Windows 7"));
					else
						Safe::strcat(pszOS, BUFSIZE, TEXT("Windows Server 2008 R2"));
				}
				else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
				{
					if (osvi.wProductType == VER_NT_WORKSTATION)
						Safe::strcat(pszOS, BUFSIZE, TEXT("Windows Vista"));
					else
						Safe::strcat(pszOS, BUFSIZE, TEXT("Windows Server 2008"));
				}
				else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
				{
					if (GetSystemMetrics(SM_SERVERR2))
						Safe::strcat(pszOS, BUFSIZE, TEXT("Windows Server 2003 R2"));
					else if (osvi.wProductType == VER_NT_WORKSTATION)
						Safe::strcat(pszOS, BUFSIZE, TEXT("Windows XP Professional"));
					else
						Safe::strcat(pszOS, BUFSIZE, TEXT("Windows Server 2003"));
				}
				else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
				{
					Safe::strcat(pszOS, BUFSIZE, TEXT("Windows XP"));
				}
				else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
				{
					Safe::strcat(pszOS, BUFSIZE, TEXT("Windows 2000 "));

					if (osvi.wProductType == VER_NT_WORKSTATION)
						Safe::strcat(pszOS, BUFSIZE, TEXT("Professional"));
					else
						Safe::strcat(pszOS, BUFSIZE, TEXT("Server"));
				}
				else
				{
					Safe::strcat(pszOS, BUFSIZE, TEXT("Unknown (New?)"));
				}

				if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					Safe::strcat(pszOS, BUFSIZE, TEXT(", 64-bit"));
				else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
					Safe::strcat(pszOS, BUFSIZE, TEXT(", 32-bit"));

				// Include service pack (if any) and build number.
				if (osvi.szCSDVersion[0] != '0')
				{
					Safe::strcat(pszOS, BUFSIZE, TEXT(" "));
					Safe::strcat(pszOS, BUFSIZE, osvi.szCSDVersion);
				}

				char buf[80];
				Safe::snprintf(buf, 80, TEXT(" (build %d)"), osvi.dwBuildNumber);
				Safe::strcat(pszOS, BUFSIZE, buf);

				Safe::strcpy(dest, destSize, pszOS);
			}
			else
			{
				Safe::strcpy(dest, destSize, "Non Supported version of windows.\n");
			}
		}
	}
}