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
#include "util/UtilOs.h"

#ifdef NIX
#include "util/UtilLinux.h"
#endif

#ifdef WIN32
	#include <shlobj.h>
#endif

#ifdef WIN32
#define getSystemPath(id, path) SHGetFolderPathW(nullptr, id , nullptr, SHGFP_TYPE_CURRENT, path);
#endif

namespace UTIL
{
namespace OS
{
bool is64OS()
{
#ifdef WIN32
	return UTIL::WIN::is64OS();
#else
	return UTIL::LIN::is64OS();
#endif
}

bool isPointOnScreen(int32 x, int32 y)
{
#ifdef WIN32
	return UTIL::WIN::isPointOnScreen(x, y);
#endif
#ifdef NIX
	return true;// TODO LINUX
#endif
}

uint64 getFreeSpace(const char* path)
{
#ifdef WIN32
	return UTIL::WIN::getFreeSpace(path);
#endif
#ifdef NIX
	return UTIL::LIN::getFreeSpace(path);
#endif
}

std::string getOSString()
{
#ifdef WIN32
	char buff[255] = {0};
	UTIL::WIN::getOSString(buff, 255);
 
	return std::string(buff, 255);
#endif

#ifdef NIX
	return UTIL::LIN::getOSString();
#endif
}

void setConfigValue(const std::string &regIndex, const std::string &value, bool expandStr, bool use64bit)
{
#ifdef WIN32
	return UTIL::WIN::setRegValue(regIndex, value, expandStr, use64bit);
#else
	return UTIL::LIN::setConfigValue(regIndex, value);
#endif
}

void setConfigValue(const std::string &regIndex, uint32 value, bool use64bit)
{
#ifdef WIN32
	return UTIL::WIN::setRegValue(regIndex, value, use64bit);
#else
	return UTIL::LIN::setConfigValue(regIndex, value);
#endif
}

void setConfigBinaryValue(const std::string &regIndex, const char* blob, size_t size, bool use64bit)
{
#ifdef WIN32
	return UTIL::WIN::setRegBinaryValue(regIndex, blob, size, use64bit);
#else
	ERROR_NOT_IMPLEMENTED;
#endif
}

std::string getConfigValue(const std::string &configKey, bool use64bit)
{
#ifdef WIN32
	return UTIL::WIN::getRegValue(configKey, use64bit);
#endif

#ifdef NIX
	return UTIL::LIN::getConfigValue(configKey);
#endif
}

std::wstring getCurrentDir(std::wstring extra)
{
#ifdef NIX
	return UTIL::LIN::getAppPath(extra);
#else
	wchar_t path[MAX_PATH];
	GetCurrentDirectoryW(MAX_PATH, path);

	gcWString out(path);

	if (extra.size() > 0)
	{
		out += DIRS_WSTR;
		out += extra;
	}

	return out;
#endif
}

#ifdef NIX
	#if \
		!defined(USE_XDG_DIRS) && \
		!defined(USE_SINGLE_HOME_DIR) && \
		!defined(USE_PORTABLE_DIR)
			#error Please select a directory structure for Desura to use!
	#endif
#endif

std::wstring getCachePath(std::wstring extra)
{
#ifdef NIX
	#if defined(USE_XDG_DIRS)
		std::string cachePath = getenv("XDG_CACHE_HOME");
		cachePath.append("/desura");
	#elif defined(USE_SINGLE_HOME_DIR)
		std::string cachePath = getenv("HOME");
		cachePath.append("/.desura/cache");
	#elif defined(USE_PORTABLE_DIR)
		std::string cachePath = UTIL::STRING::toStr(getCurrentDir(L"cache"));
	#endif
	
	if (extra.size() > 0)
		extra.insert(0, DIRS_WSTR);
	
	return UTIL::STRING::toWStr(cachePath) + extra;
#else
	return getAppDataPath(std::wstring(L"cache\\") + extra);
#endif
}

std::wstring getMcfCachePath()
{
#ifdef NIX
	return UTIL::OS::getAppDataPath();
#else
	return UTIL::OS::getCachePath(L"mcf");
#endif
}

std::wstring getAppInstallPath(std::wstring extra)
{
#ifdef NIX
	#if defined(USE_XDG_DIRS)
		std::string installPath = getenv("XDG_DATA_HOME");
		installPath.append("/desura");
	#elif defined(USE_SINGLE_HOME_DIR)
		std::string installPath = getenv("HOME");
		installPath.append("/.desura/games");
	#elif defined(USE_PORTABLE_DIR)
		std::string installPath = UTIL::STRING::toStr(getCurrentDir(L"games"));
	#endif
	
	if (extra.size() > 0)
		extra.insert(0, DIRS_WSTR);
	
	return UTIL::STRING::toWStr(installPath) + extra;
#else
	return UTIL::OS::getCurrentDir(DIR_WCOMMON);
#endif
}

std::wstring getAppDataPath(std::wstring extra)
{
#ifdef NIX
	#if defined(USE_XDG_DIRS)
		std::string configPath = getenv("XDG_CONFIG_HOME");
		configPath.append("/desura");
	#elif defined(USE_SINGLE_HOME_DIR)
		std::string configPath = getenv("HOME");
		configPath.append("/.desura");
	#elif defined(USE_PORTABLE_DIR)
		std::string configPath = UTIL::STRING::toStr(getCurrentDir(L"config"));
	#endif
	
	if (extra.size() > 0)
		extra.insert(0, DIRS_WSTR);
	
	return UTIL::STRING::toWStr(configPath) + extra;
#else
	wchar_t path[MAX_PATH];
	getSystemPath(CSIDL_COMMON_APPDATA, path);

	gcWString out(path);

	out += DIRS_WSTR;
	out += COMMONAPP_PATH_W;

	if (extra.size() > 0)
	{
		out += DIRS_WSTR;
		out += extra;
	}

	return out;
#endif
}

std::wstring getLocalAppDataPath(std::wstring extra)
{
#ifdef WIN32
	wchar_t path[MAX_PATH];
	getSystemPath(CSIDL_LOCAL_APPDATA, path);
	
	std::wstring out(path);

	out += L"\\";
	out += COMMONAPP_PATH_W;

	if (extra.size() > 0)
	{
		out += L"\\";
		out += extra;
	}

	return out;
#else
	return L"";
#endif
}

std::wstring getTempInternetPath(std::wstring extra)
{
#ifdef WIN32
	wchar_t path[MAX_PATH];
	getSystemPath(CSIDL_INTERNET_CACHE, path);
	
	std::wstring out(path);

	out += L"\\";
	out += COMMONAPP_PATH_W;

	if (extra.size() > 0)
	{
		out += L"\\";
		out += extra;
	}

	return out;
#else
	return L"";
#endif
}



std::wstring getCommonProgramFilesPath(std::wstring extra)
{
#ifdef WIN32
	wchar_t path[MAX_PATH];
	getSystemPath(CSIDL_PROGRAM_FILES_COMMON, path);
	
	std::wstring out(path);

	out += L"\\Desura";

	if (extra.size() > 0)
	{
		out += L"\\";
		out += extra;
	}

	return out;
#else
	return L"";
#endif
}

std::wstring getStartMenuProgramsPath(std::wstring extra)
{
#ifdef WIN32
	wchar_t path[MAX_PATH];
	getSystemPath(CSIDL_COMMON_PROGRAMS, path);
	
	std::wstring out(path);

	if (extra.size() > 0)
	{
		out += L"\\";
		out += extra;
	}

	return out;
#else //TODO LINUX
	return L"";
#endif
}

std::wstring getDesktopPath(std::wstring extra)
{
#ifdef WIN32
	wchar_t path[MAX_PATH];
	getSystemPath(CSIDL_DESKTOP, path);
	
	std::wstring out(path);

	if (extra.size() > 0)
	{
		out += L"\\";
		out += extra;
	}

	return out;
#else //TODO LINUX
	return L"";
#endif
}

#ifdef WIN32

gcString getAbsPath(const gcString& path)
{
	return path;
}

gcString getRelativePath(const gcString &path)
{
	return path;
}

#else

gcString getAbsPath(const gcString& path)
{
	if (path.size() == 0 || path[0] == '/')
		return path;
	
	gcString wd = UTIL::LIN::getAppPath(L"");
	
	if (path.find(wd) == std::string::npos)
		return wd + "/" + path;
	
	return path;	
}

gcString getRelativePath(const gcString &path)
{
	gcString wd = UTIL::LIN::getAppPath(L"");
	
	if (path.find(wd) == 0)
		return path.substr(wd.size()+1, std::string::npos);
	
	return path;
}

#endif



std::string UserEncodeString(const std::string& strKey, const std::string& strValue)
{
#ifdef WIN32
	DATA_BLOB secret;
	secret.pbData = (BYTE*)strKey.c_str();
	secret.cbData = strKey.size();

	DATA_BLOB db;
	db.pbData = (BYTE*)strValue.c_str();
	db.cbData = strValue.size();


	DATA_BLOB out;

	if (!CryptProtectData(&db, nullptr, &secret, nullptr, nullptr, CRYPTPROTECT_UI_FORBIDDEN, &out))
		return "";

	return UTIL::STRING::base64_encode((char*)out.pbData, out.cbData);
#else
	return UTIL::STRING::base64_encode((char*)strValue.c_str(), strValue.size());
#endif
}

std::string UserDecodeString(const std::string& strKey, const std::string& strValue)
{
	auto raw = UTIL::STRING::base64_decode(strValue);

#ifdef WIN32
	DATA_BLOB db;

	db.pbData = (BYTE*)raw.c_ptr();
	db.cbData = raw.size();

	DATA_BLOB secret;
	secret.pbData = (BYTE*)strKey.c_str();
	secret.cbData = strKey.size();

	DATA_BLOB out;

	if (CryptUnprotectData(&db, nullptr, &secret, nullptr, nullptr, CRYPTPROTECT_UI_FORBIDDEN, &out))
	{
		auto ret = std::string((char*)out.pbData, out.cbData);
		LocalFree(out.pbData);
		return ret;
	}

	return "";
#else
	return std::string(raw.c_ptr(), raw.size());
#endif
}


#ifdef DEBUG
#ifdef WIN32

#include "SharedObjectLoader.h"

typedef bool(*GetStackTraceStringFn)(int nFrames, PVOID* addrs, char* szBuffer, uint32 nBuffSize);
typedef int(*GetStackTraceFn)(PVOID* addrs, uint32 nStart, uint32 nStop);
typedef void(*IgnoreThreadForStackTraceFn)();

static GetStackTraceFn s_GetStackTrace = nullptr;
static GetStackTraceStringFn s_GetStackTraceString = nullptr;
static IgnoreThreadForStackTraceFn s_IgnoreThreadForStackTrace = nullptr;
static SharedObjectLoader s_StackWalker;

std::thread::id ignoreStackTraceOnThisThread()
{
	if (s_IgnoreThreadForStackTrace)
		s_IgnoreThreadForStackTrace();

	return std::this_thread::get_id();
}

std::shared_ptr<UTIL::OS::StackTrace> getStackTrace(uint32 nStart, uint32 nStop)
{
	static std::mutex s_Lock;
	std::lock_guard<std::mutex> guard(s_Lock);
	if (!s_GetStackTrace || !s_GetStackTraceString)
	{
		s_StackWalker.load("stackwalker.dll");

		if (!s_StackWalker.handle())
			return nullptr;

		s_GetStackTrace = s_StackWalker.getFunction<GetStackTraceFn>("getStackTrace");
		s_GetStackTraceString = s_StackWalker.getFunction<GetStackTraceStringFn>("getStackTraceString");
		s_IgnoreThreadForStackTrace = s_StackWalker.getFunction<IgnoreThreadForStackTraceFn>("ignoreThreadForStackTrace");

		if (!s_GetStackTrace || !s_GetStackTraceString)
			return nullptr;
	}

	auto st = std::make_shared<StackTrace>(25);
	st->m_nCount = s_GetStackTrace(st->m_StackPtrs, nStart, nStop);
	return st;
}

std::string getStackTraceString(const std::shared_ptr<UTIL::OS::StackTrace> &trace)
{
	if (!s_GetStackTraceString)
		return "";

	char szBuff[2048] = { 0 };
	if (!s_GetStackTraceString(trace->m_nCount, trace->m_StackPtrs, szBuff, 2048))
		return "";

	return std::string(szBuff);
}

#else

std::shared_ptr<UTIL::OS::StackTrace> getStackTrace(uint32 nStart, uint32 nStop)
{
	gcAssert(false);
	return nullptr;
}

std::string getStackTraceString(const std::shared_ptr<UTIL::OS::StackTrace> &trace)
{
	gcAssert(false);
	return "";
}

std::thread::id ignoreStackTraceOnThisThread()
{
	return std::this_thread::get_id();
}

#endif
#endif

}
}
