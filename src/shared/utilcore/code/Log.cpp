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

extern "C"
{
	void CIMPORT RegDLLCB_MCF( LogCallback* cb );
	void CIMPORT RegDLLCB_WEBCORE( LogCallback* cb );
	void CIMPORT RegDLLCB_USERCORE( LogCallback* cb );
}

void InitLogging()
{
	LogCallback::MessageFn messageFn = [](MSG_TYPE type, const char* msg, Color* col, std::map<std::string, std::string> *mpArgs)
	{
#ifndef DEBUG
		if (type == MT_DEBUG)
			return;
#endif

		LogMsg(type, gcString(msg), col, mpArgs);
	};

	safe_delete(g_pLogCallback);
	g_pLogCallback = new LogCallback();
	g_pLogCallback->RegMsg(messageFn);

	RegDLLCB_MCF(g_pLogCallback);
	RegDLLCB_WEBCORE(g_pLogCallback);
	RegDLLCB_USERCORE(g_pLogCallback);
}

void DestroyLogging()
{
	RegDLLCB_MCF(nullptr);
	RegDLLCB_WEBCORE(nullptr);
	RegDLLCB_USERCORE(nullptr);

	safe_delete(g_pLogCallback);
}





