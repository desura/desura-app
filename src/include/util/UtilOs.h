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

#ifndef DESURA_UTIL_OS_H
#define DESURA_UTIL_OS_H
#ifdef _WIN32
#pragma once
#endif

#include <thread>

namespace UTIL
{
	namespace OS
	{
		enum class BinType
		{
			ELF32,
			ELF64,
			WIN32,
			WIN64,
			MACH32,
			MACH64,
			SH,
			BAT,
			UNKNOWN,
		};

		bool is64OS();

		//! Determines if a point is on the screen
		//!
		//! @param x coord
		//! @param y coord
		//! @return True if the point is on the screen
		//!
		bool isPointOnScreen(int32 x, int32 y);

		//! Gets the ammount of free space on a drive
		//!
		//! @param path Drive to analize
		//! @return Free space in bytes
		//!
		uint64 getFreeSpace(const char* path);

		//! Gets an OS information string
		//!
		//! @return OS information stirng
		//!
		std::string getOSString();

		//! Sets a config key
		//!
		//! @param regIndex Key to set
		//! @param value Value to set it to
		//! @param type Type of reg key, default is zero = string
		//!
		void setConfigValue(const std::string &regIndex, const std::string &value, bool expandStr = false, bool use64bit = false);
		void setConfigValue(const std::string &regIndex, uint32 value, bool use64bit = false);
		void setConfigBinaryValue(const std::string &regIndex, const char* blob, size_t size, bool use64bit);

		//! Gets a config value based on a key
		//!
		//! @param config key to lookup
		//! @param 64bit flag
		//! @return config key value
		//!
		std::string getConfigValue(const std::string &configKey, bool use64bit = false);

		//! Gets the Desura application folder
		//!
		//! @param extra Extra path to add to the end of app path
		//!
		std::wstring getCurrentDir(std::wstring extra = L"");

		//! Gets the folder for Desura's data
		//!
		//! @param extra Extra path to add to the end of app path
		//!
		std::wstring getDataPath(std::wstring extra = L"");

		//! Gets the folder for Desura's cache
		//!
		//! @param extra Extra path to add to the end of app path
		//!
		std::wstring getCachePath(std::wstring extra = L"");

		//! Gets the folder for Desura's Mcf cache
		//!
		std::wstring getMcfCachePath();

		//! Gets the folder for installing apps to
		//!
		//! @param extra Extra path to add to the end of app path
		//!
		std::wstring getAppInstallPath(std::wstring extra = L"");

		//! Gets the folder for saving Desura data to
		//!
		//! @param extra Extra path to add to the end of app path
		//!
		std::wstring getAppDataPath(std::wstring extra = L"");

		//! Gets the folder for saving user app data to
		//!
		//! @param extra Extra path to add to the end of app path
		//!
		std::wstring getLocalAppDataPath(std::wstring extra = L"");

		//! Gets the temp path for internet files
		//!
		//! @param extra Extra path to add to the end of app path
		//!
		std::wstring getTempInternetPath(std::wstring extra = L"");

		std::wstring getCommonProgramFilesPath(std::wstring extra = L"");

		std::wstring getStartMenuProgramsPath(std::wstring extra = L"");

		std::wstring getDesktopPath(std::wstring extra = L"");

		gcString getAbsPath(const gcString& path);

#ifndef WIN32
		//! Gets the folder for runtime lib loading
		//!
		gcString getRuntimeLibPath();
#endif

		gcString getRelativePath(const gcString &path);

		//! Encodes a string using a unique key per user, is reversible
		//!
		std::string UserEncodeString(const std::string& strKey, const std::string& strValue);

		//! Decodes a string using a unique key per user
		//!
		std::string UserDecodeString(const std::string& strKey, const std::string& strValue);

#ifdef DEBUG

#ifdef WIN32
		class StackTrace
		{
		public:
			StackTrace(int nSize)
				: m_nCount(0)
				, m_ThreadId(std::this_thread::get_id())
			{
				if (nSize > 0)
					m_StackPtrs = new PVOID[nSize];
				else
					m_StackPtrs = nullptr;
			}

			StackTrace(const StackTrace& st) = delete;

			~StackTrace()
			{
				delete[] m_StackPtrs;
			}

			PVOID* m_StackPtrs;
			uint32 m_nCount;
			std::thread::id m_ThreadId;
		};
#else
		class StackTrace
		{
		public:
		};
#endif

		std::thread::id ignoreStackTraceOnThisThread();
		std::shared_ptr<UTIL::OS::StackTrace> getStackTrace(uint32 nStart = 0, uint32 nStop = 10);
		std::string getStackTraceString(const std::shared_ptr<UTIL::OS::StackTrace> &trace);
#endif

		std::vector<uint32> getProcessesRunningAtPath(const char* szPath);
		void killProcess(uint32 pid);

		//! Returns the stdout of a system() call
		//!
		//! @param command command to execute
		//! @param stdErrDest 0 = do nothing, 1 = hide it, 2 = append to stdout
		//! @return stdout
		//! @note Throws exceptions if can't run command
		//!
		std::string getCmdStdout(const char* command, int stdErrDest = 0);

		bool launchFolder(const char* path);

		//! Returns the type of file by its magic mark at the start of the file
		//!
		//! @param buff Buffer that contains at least the first 4 bytes
		//! @param buffSize size of buffer
		//! @return Bin type if known or BT_UNKNOWN if not
		//!
		BinType getFileType(const char* buff, size_t buffSize);

		bool canLaunchBinary(BinType type);

		inline const char* SOCK_PATH(void)
		{
			return UTIL::STRING::toStr(UTIL::OS::getCachePath(L"socket")).c_str();
		}
	}
}

#endif //DESURA_UTIL_OS_H
