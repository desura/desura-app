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

#ifndef COMMON_H
	#define COMMON_H
	#include <exception>

	#ifdef DEBUG
		#ifdef WIN32
			#define PAUSE_DEBUGGER() __asm int 3
		#else
			#define PAUSE_DEBUGGER() asm("int $3")
		#endif
	#else
		#define PAUSE_DEBUGGER()
	#endif

inline void DogcAssert(const char* szExp, const char* szFile, int nLine)
{
	PAUSE_DEBUGGER();
}

#ifdef gcAssert
#undef gcAssert
#endif

#ifdef DEBUG
	#define gcAssert( X )				\
		do							\
		{							\
			bool bFailed = !(X);	\
			if (bFailed)			\
			{						\
				DogcAssert( #X , __FILE__ , __LINE__ );	\
			}						\
		}							\
		while(false)
#else
	#define gcAssert( X ) do { } while(false)
#endif

	#define BOOST_ENABLE_ASSERT_HANDLER 1
	namespace 
	{
		class BoostException : public std::exception
		{
		public:
			virtual const char* what() const throw()
			{
				return "Boost assert failed";
			}
		};
	}

	namespace boost
	{
		inline void assertion_failed(char const * expr, char const * function, char const * file, long line)
		{
			gcAssert(false);
			throw BoostException();
		}

		inline void assertion_failed_msg(char const * expr, char const * msg, char const * function, char const * file, long line)
		{
			gcAssert(false);
			throw BoostException();
		}
	}

	#define DONT_INCLUDE_SHLOBJ

	#ifdef _DEBUG
		#ifndef DEBUG
			#define DEBUG 1
		#endif
	#endif

	#ifdef _WIN32
		#ifndef WIN32
			#define WIN32
		#endif
	#endif

	#ifdef _WIN64
		#ifndef WIN64
			#define WIN64
		#endif
	#endif

	#if defined linux || defined __linux || defined __linux__
		#ifndef NIX
			#define NIX 1
		#endif
	
		#if defined __x86_64 || defined __amd64 || defined __x86_64__
			#define NIX64 1
		#endif
	#endif

	#ifdef __APPLE__
		#ifndef MACOS
			#define MACOS 1
		#endif
	
		#define fopen64 fopen
	#endif


	#ifdef WIN32
		#define GCC_VERSION 0
	#else
		#define GCC_VERSION (__GNUC__ * 10000 \
			+ __GNUC_MINOR__ * 100 \
			+ __GNUC_PATCHLEVEL__)
	#endif

	#ifdef ERROR_OUTPUT
		#error "ERROR_OUTPUT is already defined!"
	#else
		#ifdef DEBUG
			#ifdef NIX
					#define ERROR_OUTPUT(error) fprintf(stdout, "(%d, son of %d) %s:%d - %s\n", getpid(), getppid(), __FILE__, __LINE__, error); fflush(stdout);
			#else
					#define ERROR_OUTPUT(error) fprintf(stdout, "%s:%d - %s\n",  __FILE__, __LINE__, error); fflush(stdout);
			#endif
		#else
			#define ERROR_OUTPUT(error)
		#endif
	#endif

	#ifdef ERROR_NOT_IMPLEMENTED
		#error "ERROR_NOT_IMPLEMENTED is already defined!"
	#else
		#ifdef DEBUG
			#define ERROR_NOT_IMPLEMENTED fprintf(stdout, "%s:%d (%s) - NOT IMPLEMENTED!\n", __FILE__, __LINE__, __FUNCTION__); fflush(stdout);
		#else
			#define ERROR_NOT_IMPLEMENTED
		#endif
	#endif

	#ifdef BUILDID_INTERNAL
		#undef BUILDID_INTERNAL
		#undef BUILDID_BETA
		#undef BUILDID_PUBLIC
	#endif

	#ifdef WIN32 // WINDOWS DEFINES
		#ifndef DONT_INCLUDE_WINH
			#include <windows.h>
		#else
			#include <afx.h>
		#endif
	
		#include <sys/stat.h>

		#define vsnwprintf _vsnwprintf
		#define vscprintf _vscprintf
		#define vscwprintf _vscwprintf

		typedef unsigned char uint8;
		typedef char int8;
		typedef unsigned short uint16;
		typedef short int16;
		typedef unsigned int uint32;
		typedef int int32;
		typedef unsigned long long uint64;
		typedef long long int64;

		#define DIRS_STR "\\"
		#define DIRS_CHAR '\\'
		#define DIRS_WSTR L"\\"
		#define DIRS_WCHAR L'\\'
	
		#define DIR_COMMON "Common"
		#define DIR_WCOMMON L"Common"

		#define CEXPORT __declspec(dllexport)
		#define CIMPORT __declspec(dllimport)
	
		#define BUILDID_INTERNAL 500
		#define BUILDID_BETA 300
		#define BUILDID_PUBLIC 100
	
		void inline gcSleep(uint32 miliSecs) { Sleep(miliSecs); }
	
		// mingw needs some imports
		#ifdef __MINGW32__
			#include <limits.h>
			#include <algorithm>
		
			// this is missing in the mingw headers
			// shellapi.h:
			#ifndef SEE_MASK_DEFAULT
				#define SEE_MASK_DEFAULT 0x00000000
			#endif
			// on mingw we have the XP version of NOTIFYICONDATA*
			#ifndef NOTIFYICONDATA_V3_SIZE
				#define NOTIFYICONDATAA_V3_SIZE sizeof(NOTIFYICONDATAA)
				#define NOTIFYICONDATAW_V3_SIZE sizeof(NOTIFYICONDATAW)
				#define NOTIFYICONDATA_V3_SIZE __MINGW_NAME_AW_EXT(NOTIFYICONDATA,_V3_SIZE)
			#endif
		
			// mscoree.h:
			#ifndef STARTUP_LOADER_OPTIMIZATION_MULTI_DOMAIN_HOST
				#define STARTUP_LOADER_OPTIMIZATION_MULTI_DOMAIN_HOST 0x3<<1
			#endif
			// netfw.h:
			// included in projects directly
		#endif

		#ifdef min
		#undef min
		#endif

		#ifdef max
		#undef max
		#endif

	#endif

	#ifdef NIX // LINUX 
		#define _LARGEFILE_SOURCE 1
		#define _LARGEFILE64_SOURCE 1
		#define _FILE_OFFSET_BITS   64
		#define __USE_LARGEFILE64 1
	
		#include <sys/types.h>
		#include <sys/stat.h>
		#include <sys/wait.h>
		#include <sys/un.h>
		#include <sys/socket.h>

		#include <unistd.h>
		#include <limits.h>
		#include <errno.h>

		#include <stdint.h>
		#include <stdarg.h>
		#include <strings.h>

		#define uint8	uint8_t
		#define int8	int8_t
		#define uint16	uint16_t
		#define int16	int16_t
		#define uint32	uint32_t
		#define int32	int32_t
		#define uint64	uint64_t
		#define int64	int64_t

		#define DIRS_STR "/"
		#define DIRS_CHAR '/'
		#define DIRS_WSTR L"/"
		#define DIRS_WCHAR L'/'
	
		#define DIR_COMMON "common"
		#define DIR_WCOMMON L"common"

		#define CEXPORT __attribute__((visibility("default")))
		#define CIMPORT
		#define WINAPI

		#define OutputDebugString(out)	fprintf(stderr, "%s", out);
		#define OutputDebugStringW(out)	fprintf(stderr, "%ls", out);

		#ifdef NIX64
			#define BUILDID_INTERNAL 520
			#define BUILDID_BETA 320
			#define BUILDID_PUBLIC 120
		#else
			#define BUILDID_INTERNAL 510
			#define BUILDID_BETA 310
			#define BUILDID_PUBLIC 110
		#endif

		inline int GetLastError() { return errno; }
		void inline gcSleep(uint32 miliSecs) { usleep(miliSecs*1000); }
	#endif

	#ifndef BUILDID_INTERNAL
		#error "DESURA: UNKNOWN INTERNAL BUILD"
	#endif

	#ifndef BUILDID_BETA
		#error "DESURA: UNKNOWN BETA BUILD"
	#endif

	#ifndef BUILDID_PUBLIC
		#error "DESURA: UNKNOWN PUBLIC BUILD"
	#endif

	///////////////////////////////////////////
	// UNSORTED STUFF
	///////////////////////////////////////////
	#define STR( t ) #t

	#include <stdio.h>
	#include <stdlib.h>
	#include <wchar.h>
	#include <string.h>

	//using namespace openutils;

	#include <vector>
	#include <map>
	#include <deque>
	#include <string>


	class MCFBranch
	{
	public:
		MCFBranch()
		{
			m_uiBranch = 0;
		}

		bool isGlobal()
		{
			return m_bGlobal;
		}

		operator uint32() const
		{
			return m_uiBranch;
		}

		static MCFBranch BranchFromInt(uint32 branch, bool global=false)
		{
			return MCFBranch(branch, global);
		}

	protected:
		MCFBranch(uint32 branch, bool global)
		{
			m_uiBranch = branch;
			m_bGlobal = global;
		}

	private:
		bool m_bGlobal;
		uint32 m_uiBranch;
	};


	class MCFBuild
	{
	public:
		MCFBuild()
		{
			m_uiBuild = 0;
		}

		operator uint32() const
		{
			return m_uiBuild;
		}

		static MCFBuild BuildFromInt(uint32 build)
		{
			return MCFBuild(build);
		}

	protected:
		MCFBuild(uint32 build)
		{
			m_uiBuild = build;
		}

	private:
		uint32 m_uiBuild;
	};



	#ifdef WIN32
		#define COMMONAPP_PATH "Desura\\DesuraApp"
		#define COMMONAPP_PATH_W L"Desura\\DesuraApp"
	#else
		#define COMMONAPP_PATH "appdata"
		#define COMMONAPP_PATH_W L"appdata"
	#endif

	#define THROWS( e )

	#define UPDATEFILE	"desura_update.mcf"
	#define UPDATEFILE_W L"desura_update.mcf"
	#define UPDATEXML	"desura_appfiles.xml"
	#define UPDATEXML_W L"desura_appfiles.xml"

	#define PRIMUPDATE "http://www.desura.com/api/appupdate"

	#define REGRUN "HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\Desura"
	#define APPID	"HKEY_LOCAL_MACHINE\\SOFTWARE\\Desura\\DesuraApp\\appid"
	#define APPBUILD "HKEY_LOCAL_MACHINE\\SOFTWARE\\Desura\\DesuraApp\\appver"

	//dont change the name as the uninstaller needs it!
	#define SERVICE_NAME "Desura Install Service"
	#define SERVICE_DESC "Desura Install Service"

	//this is disable the warning for std::vector not being an export class.
	#pragma warning( disable: 4251 )


	#define UNKNOWN_ITEM 0xFFFFFFFF

	//#define REDEFINE_PRINTF


	#define DEF_CHUNKSIZE (1024*1024*500) //500kb

	#define USERAGENT "Desura Default"


	#include "gcError.h"
	#include "Event.h"
	#include "util/UtilSafeDelete.h"
	#include "util/UtilSafeString.h"
	#include "util/UtilMisc.h"
	#include "util/UtilString.h"
	#include "util/UtilWeb.h"
	#include "util/UtilFs.h"
	#include "util/UtilFsPath.h"
	#include "util/UtilOs.h"
	#include "DesuraId.h"


	#ifdef DESURA_GCSTRING_H
		template <class charT, class traits> inline
		std::basic_ostream<charT,traits>& operator<<(std::basic_ostream<charT,traits>& strm, const gcException& t)
		{
			std::basic_string<charT> formatString = Template::converToStringType<charT>("{0} [{1}.{2}]");
			std::basic_string<charT> res = Template::Format(formatString.c_str(), t.getErrMsg(), t.getErrId(), t.getSecErrId());

			strm << res.c_str();	
			return strm;
		}
	#endif



	#ifdef MODCORE_LOG
		#include "Log.h"
	#else
		#include "LogBones.h"
	#endif

	#ifdef WIN32
		#include "util/UtilWindows.h"
	#endif

	#ifdef INCLUDE_CVAR_CONCOMMAND
		#include "managers\ConCommand.h"
		#include "managers\CVar.h"
	#endif

	#ifdef INCLUDE_WXWIDGETS
		#include <wx\wx.h>
	#endif

	#define Sleep gcSleep
	#define sleep gcSleep

	inline bool HasAnyFlags(uint32 value, uint32 flags)
	{
		return (value&flags?true:false);
	}

	inline bool HasAllFlags(uint32 value, uint32 flags)
	{
		return ((value&flags) == flags);
	}

	#include <memory>

	#define WeakPtr std::weak_ptr
	#define SmartPtr std::shared_ptr

	template <typename T>
	T Clamp(T val, T minVal, T maxVal)
	{
		return std::max(std::min(val, maxVal), minVal);
	}

	#include <branding/branding.h>

	#define PRODUCT_NAME_CAT(x) PRODUCT_NAME x
	#define PRODUCT_NAME_CATW(x) _T(PRODUCT_NAME) x

	// some glib overrides for ICC
	// in ICC __deprecated__ does something different, so glib 2.30 is incompatible with ICC
	#ifdef __ICC
	#  define __GLIB_H_INSIDE__ 1
	#  include <glib/gmacros.h>
	#  undef __GLIB_H_INSIDE__
	#  ifdef G_DEPRECATED_FOR
	#    undef G_DEPRECATED_FOR
	#    define G_DEPRECATED_FOR(a)
	#  endif
	#endif



	#ifdef __GNUC__
		#if ((GNUC_MAJOR == 4 && GNUC_MINOR < 7) || GNUC_MAJOR < 4)
			#define override
		#endif
	#endif

	#ifdef __clang__
		#if !__has_feature(cxx_override_control)
			#define override
		#endif

	#endif

	bool CheckVerify(bool bCon, const char* szFunction, const char* szCheck);
	#define VERIFY_OR_RETURN( con, ret ) {if (!CheckVerify(con, __FUNCTION__, #con)) return ret;}


	#ifdef WITH_GTEST

	#define ASSERT_PATHEQ( a, b ) { \
			UTIL::FS::Path pathA(a, "", false); \
			UTIL::FS::Path pathB(b, "", false); \
			ASSERT_STREQ(pathA.getFullPath().c_str(), pathB.getFullPath().c_str()); \
		}

	#define ASSERT_FILEEQ( a, b ) { \
			UTIL::FS::Path pathA(a, "", true); \
			UTIL::FS::Path pathB(b, "", true); \
			ASSERT_STREQ(pathA.getFullPath().c_str(), pathB.getFullPath().c_str()); \
		}

	#endif

	#ifdef NIX
	namespace std
	{
		template <typename T, typename... Args>
		inline unique_ptr<T> make_unique(Args&&... args)
		{
			return unique_ptr<T>(new T(forward<Args>(args)...));
		}
	}
	#endif

#endif


#ifdef LINK_WITH_GTEST
#include <gtest/gtest.h>
#endif

#ifdef LINK_WITH_GMOCK
#include <gmock/gmock.h>
#endif
