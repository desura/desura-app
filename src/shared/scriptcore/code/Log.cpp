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

class Color;

LogCallback* g_pLogCallBack = nullptr;

void DESURA_Msg(const char* msg, Color* col = nullptr)
{
	Msg(msg);
}

void DESURA_Msg_W(const wchar_t* msg, Color* col = nullptr)
{
	gcString m(msg);
	Msg(m.c_str());
}

void DESURA_Warn(const char* msg)
{
	Warning(msg);
}

void DESURA_Warn_W(const wchar_t* msg)
{
	gcString m(msg);
	Warning(m.c_str());
}

void DESURA_Debug(const char* msg)
{
	Debug(msg);
}

void DESURA_Debug_W(const wchar_t* msg)
{
	gcString m(msg);
	Debug(m.c_str());
}


void InitLogging(RegDLLCB_MCF cb)
{
	g_pLogCallBack = new LogCallback();

	g_pLogCallBack->RegMsg(&DESURA_Msg);
	g_pLogCallBack->RegMsg(&DESURA_Msg_W);
	g_pLogCallBack->RegWarn(&DESURA_Warn);
	g_pLogCallBack->RegWarn(&DESURA_Warn_W);
	g_pLogCallBack->RegDebug(&DESURA_Debug);
	g_pLogCallBack->RegDebug(&DESURA_Debug_W);

	cb(g_pLogCallBack);
}

void DestroyLogging()
{
	safe_delete(g_pLogCallBack);
}
