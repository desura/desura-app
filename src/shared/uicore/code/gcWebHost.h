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

#ifndef DESURA_GCWEBHOST_H
#define DESURA_GCWEBHOST_H
#ifdef _WIN32
#pragma once
#endif

#ifdef NIX
#include "gcWebControl.h"
typedef gcWebControl gcWebHost;
#endif

#ifdef WIN32

#include "wx_controls/gcControls.h"
#include "gcWebControlI.h"

class EventHandler;
class IPCBrowser;
class CefIPCPipeClient;
class ChromiumMenuInfoFromMem;

class gcWebHost : public gcPanel, public gcWebControlI
{
public:
	gcWebHost(wxWindow* parent, const char* defaultUrl, const char* hostName);
	~gcWebHost();

	virtual void loadUrl(const gcString& url);
	virtual void loadString(const gcString& string);
	virtual void executeJScript(const gcString& code);

	virtual bool refresh();
	virtual bool stop();
	virtual bool back();
	virtual bool forward();

	virtual void forceResize();
	virtual void home();

	virtual void AddPendingEvent(const wxEvent &event)
	{
		gcPanel::AddPendingEvent(event);
	}

	virtual HWND getBrowserHWND()
	{
		return 0;
	}

	virtual void PopupMenu(wxMenu* menu)
	{
		gcPanel::PopupMenu(menu);
	}

	virtual void HandlePopupMenu(ChromiumMenuInfoFromMem* menu);

protected:
	void onResize(wxSizeEvent& event);
	void onPaint(wxPaintEvent& event);
	void onFocus(wxFocusEvent& event);
	void onPaintBg(wxEraseEvent& event);

	void onMouseDown(wxMouseEvent& event);
	void onMenuClicked(wxCommandEvent& event);

	void paintNoBrowser();
	void onBrowserDisconnect();

	void start(const char* url);

	virtual void handleKeyEvent(int eventCode);

	EventV onMenuEvent;
	void onMenuShow();

	void forceRefresh();
	void print();
	void viewSource();
	void zoomIn();
	void zoomOut();
	void zoomNormal();

	void undo();
	void redo();
	void cut();
	void copy();
	void paste();
	void del();
	void selectall();
	void showInspector();


	void onCookieUpdate();

private:
	IPCBrowser* m_pBrowser;
	CefIPCPipeClient* m_pPipeClient;
	EventHandler* m_pEventHandler;

	gcString m_szHostName;
	gcString m_szDefaultUrl;

	gcWString m_szDieString;

	ChromiumMenuInfoFromMem* m_MenuInfo;
};

#endif




#endif //DESURA_GCWEBHOST_H
