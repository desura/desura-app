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


class gcWebControl;
typedef ChromiumDLL::ChromiumBrowserI* (*CreateBrowserFn)(gcWebControl *pControl, const char* loadUrl);

class gcWebControl : public gcPanel, public gcWebControlI
{
public:
	gcWebControl(wxWindow* parent, const char* defaultUrl, const char* unused="");
	gcWebControl(wxWindow* parent, const char* defaultUrl, CreateBrowserFn createBrowserFn);
	~gcWebControl();

	void loadUrl(const gcString& url) override;
	void loadString(const gcString& string) override;
	void executeJScript(const gcString& code) override;


	bool refresh() override;
	bool stop() override;
	bool back() override;
	bool forward() override;

	void forceResize() override;
	void home() override;

	Event<int32> onContextSelectEvent;

#ifdef WIN32
	HWND getBrowserHWND();
#endif

	ChromiumDLL::JavaScriptContextI* getJSContext();

	void AddPendingEvent(const wxEvent &event) override
	{
		gcPanel::AddPendingEvent(event);
	}

	void PopupMenu(wxMenu* menu, int x, int y) override
	{
//		gcPanel::PopupMenu(menu, x, y);
		if (menu != nullptr) {
			if (gcPanel::GetParent()) {
				gcPanel::SetFocus();
				gcPanel::PopupMenu(menu);
			}
		}	
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
	void handleKeyEvent(int eventCode) override;

	friend class EventHandler;

private:
	ChromiumDLL::ChromiumBrowserI* m_pChromeBrowser;
	bool m_bStartedLoading;
	bool m_bContentLoaded;

	gcString m_szHomeUrl;

	EventHandler* m_pEventHandler;
};

#endif //DESURA_GCWEBCONTROL_H
