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

class gcWebControlI;
class gcCustomMenu;

enum Action
{
	IDC_BACK,
	IDC_FORWARD,
	IDC_DEV_TOOLS,
	IDC_DEV_TOOLS_CONSOLE,
	IDC_PRINT,
	IDC_RELOAD,
	IDC_RELOAD_IGNORING_CACHE,
	IDC_HOME,
	IDC_STOP,
	IDC_VIEW_SOURCE,
	IDC_ZOOM_MINUS,
	IDC_ZOOM_NORMAL,
	IDC_ZOOM_PLUS,
	IDC_FIND,
};

enum MenuEvents
{
	MENU_ID_UNDO = 10,
	MENU_ID_REDO,
	MENU_ID_CUT,
	MENU_ID_COPY,
	MENU_ID_PASTE,
	MENU_ID_DELETE,
	MENU_ID_SELECTALL,
	MENU_ID_BACK,
	MENU_ID_FORWARD,
	MENU_ID_RELOAD,
	MENU_ID_PRINT,
	MENU_ID_VIEWSOURCE,
	MENU_ID_VIEWPBROWSER,
	MENU_ID_VIEWLBROWSER,
	MENU_ID_VIEWIBROWSER,
	MENU_ID_ZOOM_MINUS,
	MENU_ID_ZOOM_NORMAL,
	MENU_ID_ZOOM_PLUS,
	MENU_ID_INSPECTELEMENT,
	MENU_ID_COPYURL,

	MENU_ID_CUSTOMACTION, //must be last
};

class ContextClientDataI
{
public:
	virtual bool processResult(uint32 res)=0;
	virtual void destroy()=0;
};

class gcMenu;
class ChromiumMenuInfoFromMem;


#ifdef USE_CHROMIUM_API_V2
#define CHROMIUMEVENTCLASS ChromiumDLL::ChromiumBrowserEventI_V2
#else
#define CHROMIUMEVENTCLASS ChromiumDLL::ChromiumBrowserEventI
#endif

class EventHandler : public CHROMIUMEVENTCLASS
{
public:
	EventHandler(gcWebControlI* parent);
	~EventHandler();

	bool onNavigateUrl(const char* url, bool isMain) override;
	void onPageLoadStart() override;
	void onPageLoadEnd() override;

	bool onJScriptAlert(const char* msg) override;
	bool onJScriptConfirm(const char* msg, bool* result) override;
	bool onJScriptPrompt(const char* msg, const char* defualtVal, bool* handled, char result[255]) override;

	bool onKeyEvent(ChromiumDLL::KeyEventType type, int code, int modifiers, bool isSystemKey) override;

	void onLogConsoleMsg(const char* message, const char* source, int line) override;

	void launchLink(const char* url) override;
	bool onLoadError(const char* errorMsg, const char* url, char* buff, size_t size) override;

	void HandleWndProc(int message, int wparam, int lparam) override;
	bool HandlePopupMenu(ChromiumDLL::ChromiumMenuInfoI* menuInfo) override;

	void HandleJSBinding(ChromiumDLL::JavaScriptObjectI* jsObject, ChromiumDLL::JavaScriptFactoryI* factory) override;

#ifdef USE_CHROMIUM_API_V2
	void onDownloadFile(const char* szUrl, const char* szMimeType, unsigned long long ullFileSize) override;

	//void onStatus(const char* szStatus, ChromiumDLL::StatusType eType) override {}

	//void onTitle(const char* szTitle) override {}

	//bool onToolTip(const char* szToolTop) override { return false; }
#endif

	uint32 getLastX()
	{
		return m_uiLastContextMenuX;
	}

	uint32 getLastY()
	{
		return m_uiLastContextMenuY;
	}

	void clearCrumbs();
	void addCrumb(const char* name, const char* url);

	void ForwardPopupMenu(ChromiumMenuInfoFromMem* menu);
	gcMenu* createMenu(ChromiumDLL::ChromiumMenuInfoI* menuInfo);

	gcWString getLastMenuUrl()
	{
		std::lock_guard<std::mutex> guard(m_UrlLock);
		return m_strLastMenuUrl;
	}

protected:
	void displayMenu(ChromiumDLL::ChromiumMenuInfoI* menuInfo, gcMenu *menu, int32 x, int32 y);
	void setupLastMenuUrl(ChromiumDLL::ChromiumMenuInfoI* menuInfo);

private:
	uint32 m_uiLastContextMenuX;
	uint32 m_uiLastContextMenuY;

	gcWebControlI* m_pParent;

	std::mutex m_UrlLock;
	gcWString m_strLastMenuUrl;
};



#endif //DESURA_GCWCEVENTS_H
