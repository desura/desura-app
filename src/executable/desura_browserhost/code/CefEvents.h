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


#ifndef DESURA_GCWCEVENTS_H
#define DESURA_GCWCEVENTS_H
#ifdef _WIN32
#pragma once
#endif

#include "cef_desura_includes/ChromiumBrowserI.h"


class EventHandler : public ChromiumDLL::ChromiumBrowserEventI
{
public:
	EventHandler();
	~EventHandler();

	virtual bool onNavigateUrl(const char* url, bool isMain);
	virtual void onPageLoadStart();
	virtual void onPageLoadEnd();

	virtual bool onJScriptAlert(const char* msg);
	virtual bool onJScriptConfirm(const char* msg, bool* result);
	virtual bool onJScriptPrompt(const char* msg, const char* defualtVal, bool* handled, char result[255]);

	virtual bool onKeyEvent(ChromiumDLL::KeyEventType type, int code, int modifiers, bool isSystemKey);

	virtual void onLogConsoleMsg(const char* message, const char* source, int line);

	virtual void launchLink(const char* url);
	virtual bool onLoadError(const char* errorMsg, const char* url, char* buff, size_t size);

	virtual void HandleWndProc(int message, int wparam, int lparam);
	virtual bool HandlePopupMenu(ChromiumDLL::ChromiumMenuInfoI* menuInfo);

	virtual void HandleJSBinding(ChromiumDLL::JavaScriptObjectI* jsObject, ChromiumDLL::JavaScriptFactoryI* factory);
};



#endif //DESURA_GCWCEVENTS_H
