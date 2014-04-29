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
#include "Managers.h"

#include "MainApp.h"
#include "Console.h"

#include "LogBones.cpp"


static Console* g_pConsole;

extern bool admin_cb(CVar* var, const char* val);

CVar admin_developer("admin_developer", "0", CFLAG_ADMIN|CFLAG_NOSAVE, (CVarCallBackFn)&admin_cb);


#ifdef DEBUG
	CVar gc_debug("gc_debug", "1", CFLAG_USER); 
	CVar gc_showerror("gc_showerror", "1", CFLAG_USER);
#else
	CVar gc_debug("gc_debug", "0", CFLAG_USER); 
	CVar gc_showerror("gc_showerror", "0", CFLAG_USER);
#endif


void HideLogForm()
{
	if (!g_pConsole)
		 return;
	
	g_pConsole->Show(false);
}

void ShowLogForm(bool forced)
{
	if (!g_pConsole || !g_pMainApp)
		 return;
	
	if (!g_pMainApp->isLoggedIn() && !g_pMainApp->isOffline())
		return;

	if (!forced && g_pConsole->IsShownOnScreen())
		return;

	g_pConsole->postShowEvent();
}

extern "C"
{
	void CIMPORT RegDLLCB_MCF(LogCallback* cb);
	void CIMPORT RegDLLCB_WEBCORE(LogCallback* cb);
	void CIMPORT RegDLLCB_USERCORE(LogCallback* cb);
}



void LoggingapplyTheme()
{
	if (!g_pConsole)
		 return;

	g_pConsole->applyTheme();
}

void InitLogging()
{
	LogCallback::MessageFn messageFn = [&](MSG_TYPE type, const char* szMessage, Color *pColor, std::map<std::string, std::string> *pmArgs)
	{
		if (type == MT_TRACE)
		{
			Console::trace(szMessage, pmArgs);
			return;
		}

		if (!g_pConsole)
			return;

		if (type == MT_MSG)
		{
			g_pConsole->appendText(szMessage, pColor ? *pColor : Color(0));
		}
		else if (type == MT_WARN)
		{
			g_pConsole->appendText(szMessage, Color(0xFF000000));

			if (gc_showerror.getBool())
				ShowLogForm(false);
		}
		else if (type == MT_DEBUG)
		{
			if (gc_debug.getBool())
				g_pConsole->appendText(szMessage, Color(0x0000FF00));
		}
	};


	if (g_pConsole)
		safe_delete(g_pConsole);

	g_pConsole = new Console(nullptr);

	Msg("UICore Logging Started\n");

	safe_delete(g_pLogCallback);

	g_pLogCallback = new LogCallback();
	g_pLogCallback->RegMsg(messageFn);

	RegDLLCB_MCF(g_pLogCallback);
	RegDLLCB_WEBCORE(g_pLogCallback);
	RegDLLCB_USERCORE(g_pLogCallback);
}

void RegLogWithWindow()
{
	GetWindowManager().registerWindow(g_pConsole);
}

void DestroyLogging()
{
	g_pConsole->Destroy();
	g_pConsole = nullptr;

	RegDLLCB_MCF(nullptr);
	RegDLLCB_WEBCORE(nullptr);
	RegDLLCB_USERCORE(nullptr);

	safe_delete(g_pLogCallback);
}





