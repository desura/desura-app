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
#include "Browser.h"
#include "cef_desura_includes/ChromiumBrowserI.h"
#include <branding/branding.h>

bool InitWebControl()
{
	gcWString logPath;

	if (!CEF_Init(true, "", "", ""))
		return false;

	return true;
}

void ShutdownWebControl()
{
	CEF_Stop();
}

class EventHandler : public ChromiumDLL::ChromiumBrowserEventI
{
public:
	EventHandler(Browser* parent)
	{
		m_pParent = parent;
	}

	virtual bool onNavigateUrl(const char* url)
	{
		return true;
	}

	virtual void onPageLoadStart()
	{
	}

	virtual void onPageLoadEnd()
	{
	}

	virtual bool onJScriptAlert(const char* msg)
	{

		return false;
	}

	virtual bool onJScriptConfirm(const char* msg, bool* result)
	{
		return false;
	}

	virtual bool onJScriptPrompt(const char* msg, const char* defualtVal, bool* handled, char result[255])
	{
		return false;
	}

	virtual bool onKeyEvent(ChromiumDLL::KeyEventType type, int code, int modifiers, bool isSystemKey)
	{
		return false;
	}

	virtual void onLogConsoleMsg(const char* message, const char* source, int line)
	{
	}

	virtual void launchLink(const char* url)
	{
	}

	virtual bool onLoadError(const char* errorMsg, const char* url, char* buff, size_t size)
	{
		return false;
	}

	virtual void HandleWndProc(int message, int wparam, int lparam)
	{
	}

	virtual bool HandlePopupMenu(ChromiumDLL::ChromiumMenuInfoI* menuInfo)
	{
		return false;
	}

	virtual void HandleJSBinding(ChromiumDLL::JavaScriptObjectI* jsObject, ChromiumDLL::JavaScriptFactoryI* factory)
	{

	}

private:
	Browser* m_pParent;
};



BEGIN_EVENT_TABLE( Browser, wxPanel )
	EVT_SIZE( Browser::onResize )
	EVT_ERASE_BACKGROUND( Browser::onPaintBg )
	EVT_PAINT( Browser::onPaint )
END_EVENT_TABLE()


Browser::Browser(wxWindow* parent) : gcPanel(parent, wxID_ANY)
{
	m_pEventHandler = new EventHandler(this);

	m_pChromeBrowser = CEF_NewChromiumBrowser((int*)GetHWND(), PRODUCT_NAME, "http://www.bobbyvandersluis.com/swfobject/testsuite_2_2/test_dynamic.html"); //L"http://192.168.211.10/cookie.php");
	m_pChromeBrowser->setEventCallback(m_pEventHandler);

}

Browser::~Browser()
{
	m_pChromeBrowser->setEventCallback(NULL);
	m_pChromeBrowser->destroy();

	safe_delete(m_pEventHandler);
}


void Browser::setCookie(const char* name, const char* value)
{
	ChromiumDLL::CookieI* cookie = CEF_CreateCookie();

	cookie->SetDomain("192.168.211.10");
	cookie->SetPath("/");
	cookie->SetName(name);
	cookie->SetData(value);

	CEF_SetCookie("http://192.168.211.10", cookie);
	cookie->destroy();

	m_pChromeBrowser->refresh();
}



void Browser::onResize( wxSizeEvent& event )
{
	m_pChromeBrowser->onResize();
}

void Browser::onPaintBg( wxEraseEvent& event )
{
	m_pChromeBrowser->onPaintBg();
}

void Browser::onPaint( wxPaintEvent& event )
{
	m_pChromeBrowser->onPaint();
}

void Browser::loadUrl(const char* url)
{
	m_pChromeBrowser->loadUrl(url);
}


