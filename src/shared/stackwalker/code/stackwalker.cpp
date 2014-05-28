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
#include <Windows.h>

#ifdef DEBUG
	#include "DbgHelp.h"
#endif


static std::mutex s_StackLock;
static bool s_InitStackTrace = false;
static std::thread::id s_IgnoreThread;

std::atomic<uint32> g_StackCount = { 0 };

extern "C"
{
	CEXPORT void ignoreThreadForStackTrace()
	{
		std::lock_guard<std::mutex> guard(s_StackLock);
		s_IgnoreThread = std::this_thread::get_id();
	}

	CEXPORT bool getStackTraceString(int nFrames, PVOID* addrs, char* szBuffer, uint32 nBuffSize)
	{
		std::lock_guard<std::mutex> guard(s_StackLock);

		if (s_IgnoreThread == std::this_thread::get_id())
			return 0;

		if (!s_InitStackTrace)
		{
			gcAssert(false);
			return false;
		}

		std::string outWalk;

		for (int i = 0; i < nFrames; i++)
		{
			// Allocate a buffer large enough to hold the symbol information on the stack and get 
			// a pointer to the buffer.  We also have to set the size of the symbol structure itself
			// and the number of bytes reserved for the name.
			ULONG64 buffer[(sizeof(SYMBOL_INFO) + 1024 + sizeof(ULONG64) - 1) / sizeof(ULONG64)] = { 0 };
			SYMBOL_INFO *info = (SYMBOL_INFO *)buffer;
			info->SizeOfStruct = sizeof(SYMBOL_INFO);
			info->MaxNameLen = 1024;

			// Attempt to get information about the symbol and add it to our output parameter.
			DWORD64 displacement = 0;
			if (::SymFromAddr(::GetCurrentProcess(), (DWORD64)addrs[i], &displacement, info))
			{
				outWalk.append(info->Name, info->NameLen);
				outWalk.append("\n");
			}
		}

		auto nSize = outWalk.size();

		if (nSize > nBuffSize)
			nSize = nBuffSize - 1;

		Safe::strncpy(szBuffer, nBuffSize, outWalk.c_str(), nSize);
		return true;

	}

	CEXPORT int getStackTrace(PVOID* addrs, uint32 nStart, uint32 nStop)
	{
		if (nStop > 25)
			nStop = 25;

		std::lock_guard<std::mutex> guard(s_StackLock);

		if (s_IgnoreThread == std::this_thread::get_id())
			return 0;

		if (!s_InitStackTrace)
		{
			// Set up the symbol options so that we can gather information from the current
			// executable's PDB files, as well as the Microsoft symbol servers.  We also want
			// to undecorated the symbol names we're returned.  If you want, you can add other
			// symbol servers or paths via a semi-colon separated list in SymInitialized.
			::SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_INCLUDE_32BIT_MODULES | SYMOPT_UNDNAME);

			if (!::SymInitialize(::GetCurrentProcess(), "E:\\SymbolCache;http://msdl.microsoft.com/download/symbols", TRUE))
				return -1;

			s_InitStackTrace = true;
		}

		++g_StackCount;
		return CaptureStackBackTrace(nStart, nStop, addrs, NULL);
	}
}