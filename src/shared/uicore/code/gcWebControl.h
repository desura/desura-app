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

#ifndef DESURA_GCWEBCONTROL_H
#define DESURA_GCWEBCONTROL_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcControls.h"
#include "gcWebControlI.h"




namespace ChromiumDLL
{
	class ChromiumBrowserI;
	class JavaScriptContextI;
}

class EventHandler;
class ChromiumMenuInfoFromMem;

//unused is because gcWebHost is typedef as this under linux

class gcWebControl : public gcPanel, public gcWebControlI
{
public:
	gcWebControl(wxWindow* parent, const char* defaultUrl, const char* unused="");
	~gcWebControl();

	virtual void loadUrl(const gcString& url);
	virtual void loadString(const gcString& string);
	virtual void executeJScript(const gcString& code);

	virtual bool refresh();
	virtual bool stop();
	virtual bool back();
	virtual bool forward();

	virtual void forceResize();
	virtual void home();

	Event<int32> onContextSelectEvent;

#ifdef WIN32
	HWND getBrowserHWND();
#endif

	ChromiumDLL::JavaScriptContextI* getJSContext();

	virtual void AddPendingEvent(const wxEvent &event)
	{
		gcPanel::AddPendingEvent(event);
	}
	
	virtual void PopupMenu(wxMenu* menu)
	{
		gcPanel::PopupMenu(menu);
	}

protected:
	void onResize(wxSizeEvent& event);
	void onPaintBg(wxEraseEvent& event);
	void onPaint(wxPaintEvent& event);

	void onMenuClicked(wxCommandEvent& event);
	void onMouseScroll(wxMouseEvent& event);
	void onFocus(wxFocusEvent& event);

	void onStartLoad();
	void onPageLoad();
	virtual void handleKeyEvent(int eventCode);
	
	friend class EventHandler;

private:
	ChromiumDLL::ChromiumBrowserI* m_pChromeBrowser;
	bool m_bStartedLoading;
	bool m_bContentLoaded;

	gcString m_szHomeUrl;

	EventHandler* m_pEventHandler;
};

#endif //DESURA_GCWEBCONTROL_H
