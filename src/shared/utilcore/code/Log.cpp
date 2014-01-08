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
#include "Log.h"
#include "LogCallback.h"

#include "LogBones.cpp"
//LogCallback* g_pLogCallback = nullptr;
//void LogMsg(int type, std::string msg, Color* col);
//void LogMsg(int type, std::wstring msg, Color* col);

void DESURA_Msg(const char* msg, Color *col = nullptr)
{
	if (!msg)
		return;

	LogMsg(MT_MSG, msg, col);
}

void DESURA_Msg_W(const wchar_t* msg, Color *col = nullptr)
{
	if (!msg)
		return;

	LogMsg(MT_MSG, msg, col);
}

void DESURA_Warn(const char* msg)
{
	if (!msg)
		return;

	LogMsg(MT_WARN, msg, nullptr);
}

void DESURA_Warn_W(const wchar_t* msg)
{
	if (!msg)
		return;

	LogMsg(MT_WARN, msg, nullptr);
}

void DESURA_Debug(const char* msg)
{
#ifdef DEBUG
	if (!msg)
		return;

	LogMsg(MT_MSG, msg, nullptr);
#endif
}

void DESURA_Debug_W(const wchar_t* msg)
{
#ifdef DEBUG
	if (!msg)
		return;

	LogMsg(MT_MSG, msg, nullptr);
#endif
}


extern "C"
{
	void CIMPORT RegDLLCB_MCF( LogCallback* cb );
	void CIMPORT RegDLLCB_WEBCORE( LogCallback* cb );
	void CIMPORT RegDLLCB_USERCORE( LogCallback* cb );
}

void InitLogging()
{
	safe_delete(g_pLogCallback);
	g_pLogCallback = new LogCallback();

	g_pLogCallback->RegMsg(&DESURA_Msg);
	g_pLogCallback->RegMsg(&DESURA_Msg_W);
	g_pLogCallback->RegWarn(&DESURA_Warn);
	g_pLogCallback->RegWarn(&DESURA_Warn_W);
	g_pLogCallback->RegDebug(&DESURA_Debug);
	g_pLogCallback->RegDebug(&DESURA_Debug_W);
}

void DestroyLogging()
{
	RegDLLCB_MCF(nullptr);
	RegDLLCB_WEBCORE(nullptr);
	RegDLLCB_USERCORE(nullptr);

	safe_delete(g_pLogCallback);
}





