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

#ifndef DESURA_GCWEBCONTROLI_H
#define DESURA_GCWEBCONTROLI_H
#ifdef _WIN32
#pragma once
#endif

#include <wx/event.h>

typedef struct
{
	const char* url;
	bool stop;
	bool main;
} newURL_s;

class Crumb
{
public:
	gcString name;
	gcString url;
};

class ChromiumMenuInfoFromMem;

class gcWebControlI
{
public:
	virtual void loadUrl(const gcString& url)=0;
	virtual void loadString(const gcString& string)=0;
	virtual void executeJScript(const gcString& code)=0;

	virtual bool refresh()=0;
	virtual bool stop()=0;
	virtual bool back()=0;
	virtual bool forward()=0;

	virtual void forceResize()=0;
	virtual void home()=0;

	virtual void AddPendingEvent(const wxEvent &event)=0;
	virtual void handleKeyEvent(int eventCode)=0;

	Event<newURL_s> onNewURLEvent;
	EventV onPageLoadEvent;
	EventV onPageStartEvent;
	EventV onAnyPageLoadEvent;

	EventV onClearCrumbsEvent;
	Event<Crumb> onAddCrumbEvent;

	EventV onFindEvent;

	//! Callee is responsable for deleting menu
	virtual void HandlePopupMenu(ChromiumMenuInfoFromMem* menu){};
	virtual void PopupMenu(wxMenu* menu)=0;


#ifdef WIN32
	virtual HWND getBrowserHWND()=0;
#endif
};

#endif //DESURA_GCWEBCONTROLI_H
