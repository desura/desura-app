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

#include "Common.h"

#define CEF_IGNORE_FUNCTIONS 1
#include "cef_desura_includes/ChromiumBrowserI.h"

#include "gcWCUtil_Legacy.h"
#include "MainApp.h"

#include "SharedObjectLoader.h"
#include "gcSchemeBase.h"
#include "gcJSBase.h"

#include "webcore/WebCoreI.h"


#if defined(NIX) && !defined(MACOS)
#include "managers/CVar.h"
#include <gtk/gtk.h>
#endif

#include <wx/app.h>

void RegisterJSBindings();
void RegisterSchemes();

typedef gcString (*UserAgentFN)();

#if defined(NIX) && !defined(MACOS)
guint m_timeoutSource = 0;
#endif

bool g_bLoaded = false;
SharedObjectLoader g_CEFDll;
ChromiumDLL::ChromiumControllerI* g_pChromiumController = nullptr;


void SetCookies();

#if defined(NIX) && !defined(MACOS)
gboolean onTimeout(gpointer data)
{
	if (!g_bLoaded || !g_pChromiumController)
	{
		m_timeoutSource = 0;
		return false;
	}

	g_pChromiumController->DoMsgLoop();

	//if we dont do this here we could end up starving the pending que due to g_pChromiumController->DoMsgLoop() taking to long.
	if (wxTheApp)
		wxTheApp->ProcessPendingEvents();

	return true;
}

void RestartTimer(uint timeout)
{
	if (! g_bLoaded)
		return;

	if (m_timeoutSource != 0)
		g_source_remove(m_timeoutSource);
	m_timeoutSource = g_timeout_add(timeout, onTimeout, nullptr);
}

bool RestartTimerCB(const CVar* hook, const char* newval)
{
	if (! newval)
		return false;

	RestartTimer(Safe::atoi(newval));

	return true;
}

CVar gc_cef_timeout("gc_cef_timeout", "75", 0, &RestartTimerCB);
#endif

#ifdef WIN32
const char* szCefDLL = "cef_desura.dll";
#else
const char* szCefDLL = "libcef_desura.so";
#endif

bool LoadCEFDll()
{
	gcTraceS("");

	if (!g_CEFDll.load(szCefDLL))
	{
		WarningS("Failed to load cef library: {0}\n", GetLastError());
		return false;
	}

	UserAgentFN userAgent = (UserAgentFN) WebCore::FactoryBuilder(WEBCORE_USERAGENT);
	gcString ua;

	//stupid hack cause crappy paypal does useragent sniffing. FFFFFFFFFFFFUUUUUUUUUUUUUUUUUUUUUUU
#ifdef WIN32
	ua += "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/535.1 (KHTML, like Gecko) Chrome/13.0.782.220 Safari/535.1";
#else
	ua += "Mozilla/5.0 (X11; Linux i686) AppleWebKit/535.1 (KHTML, like Gecko) Chrome/13.0.782.220 Safari/535.1";
#endif

	ua += " " + userAgent();

	UTIL::FS::Path path(UTIL::OS::getTempInternetPath(L"desura"), L"", false);
	UTIL::FS::recMakeFolder(path);

	gcString logPath;

#ifdef DEBUG
	logPath = "Chromium_log.txt";
#endif

#ifdef WIN32
	bool multiThreaded = true;
#else
	bool multiThreaded = false;
#endif

	g_pChromiumController = CEF_Init_Legacy(g_CEFDll, multiThreaded, gcString(path.getFolderPath()).c_str(), logPath.c_str(), ua.c_str());

	if (!g_pChromiumController)
	{
		WarningS("Failed to init cef.\n");
		return false;
	}

	g_pChromiumController->SetApiVersion(2);
	return true;
}

bool InitWebControl()
{
	gcTraceS("");

	if (g_bLoaded)
		return true;

	if (!LoadCEFDll())
		return false;

	if (!g_pChromiumController)
		return false;

	RegisterJSBindings();
	RegisterSchemes();

#if defined(NIX) && !defined(MACOS)
	m_timeoutSource = g_timeout_add(50, onTimeout, nullptr);
#endif

	g_bLoaded = true;
	SetCookies();
	return true;
}

void ShutdownWebControl()
{
	gcTraceS("");

	g_bLoaded = false;

#if defined(NIX) && !defined(MACOS)
	if (m_timeoutSource != 0)
	{
		g_source_remove(m_timeoutSource);
		m_timeoutSource = 0;
	}
#endif

	if (g_pChromiumController)
		g_pChromiumController->Stop();

	g_pChromiumController = nullptr;

	//Dont unload the dll yet, we might still have valid pointers
	//g_CEFDll.unload();
}

static std::mutex g_RootUrlMutex;
static gcString g_strRootUrl = "desura.com";

void DeleteCookies()
{
	if (!g_pChromiumController)
		return;

	gcString urlRoot;

	{
		std::lock_guard<std::mutex> a(g_RootUrlMutex);
		urlRoot = g_strRootUrl;
	}

	g_pChromiumController->DeleteCookie(urlRoot.c_str(), "freeman");
	g_pChromiumController->DeleteCookie(urlRoot.c_str(), "masterchief");
}

void SetCookies()
{
	if (!g_bLoaded || !GetWebCore())
		return;

	ChromiumDLL::CookieI* cookie = g_pChromiumController->CreateCookie();

	if (!cookie)
	{
		WarningS("Cef failed to create cookie. Failed to set cookies. :(\n");
		return;
	}

	gcString urlRoot;

	{
		std::lock_guard<std::mutex> a(g_RootUrlMutex);

		if (GetWebCore())
			g_strRootUrl = GetWebCore()->getUrl(WebCore::Root);

		urlRoot = g_strRootUrl;
	}

	if (urlRoot.find("http://") == 0)
		urlRoot = urlRoot.substr(7);

	if (urlRoot.find("www") == 0)
		urlRoot = urlRoot.substr(3);
	else
		urlRoot = gcString(".") + urlRoot;

	cookie->SetDomain(urlRoot.c_str());
	cookie->SetPath("/");

	std::function<void(const char*, const char*, const char*)> cookieCallback
		= [&](const char* szRootUrl, const char* szName, const char* szValue)
	{
		cookie->SetName(szName);
		cookie->SetData(szValue);

		g_pChromiumController->SetCookie(szRootUrl, cookie);
	};

	GetWebCore()->setCookies(cookieCallback);
	cookie->destroy();
}

#if defined(WIN32)
//ChromiumDLL::ChromiumBrowserI* NewChromiumBrowser(HWND hwnd, const char* name, const char* loadUrl)
ChromiumDLL::ChromiumBrowserI* NewChromiumBrowser(HWND hwnd, const char* name, const char* loadUrl)
#elif defined(MACOS)
ChromiumDLL::ChromiumBrowserI* NewChromiumBrowser(void* hwnd, const char* name, const char* loadUrl)
#else
ChromiumDLL::ChromiumBrowserI* NewChromiumBrowser(int* hwnd, const char* name, const char* loadUrl)
#endif
{
	gcTraceS("Name: {0}, Url: {1}", name, loadUrl);

	if (!g_bLoaded && !InitWebControl())
		return nullptr;

	return g_pChromiumController->NewChromiumBrowser((int*)hwnd, name, loadUrl);
}



std::vector<ChromiumDLL::JavaScriptExtenderI*> *g_vJSExtenderList=nullptr;

void RegisterJSExtender( ChromiumDLL::JavaScriptExtenderI* scheme )
{
	if (!g_vJSExtenderList)
		g_vJSExtenderList = new std::vector<ChromiumDLL::JavaScriptExtenderI*>();

	g_vJSExtenderList->push_back(scheme);
}

void RegisterJSBindings()
{
	if (!g_vJSExtenderList || !g_pChromiumController)
		return;

	for (size_t x=0; x<g_vJSExtenderList->size(); x++)
		g_pChromiumController->RegisterJSExtender((*g_vJSExtenderList)[x]);

	g_vJSExtenderList->clear();
	safe_delete(g_vJSExtenderList);
}



std::vector<ChromiumDLL::SchemeExtenderI*> *g_vSchemeList = nullptr;

void RegisterScheme( ChromiumDLL::SchemeExtenderI* scheme )
{
	if (!g_vSchemeList)
		g_vSchemeList = new std::vector<ChromiumDLL::SchemeExtenderI*>();

	g_vSchemeList->push_back(scheme);
}

void RegisterSchemes()
{
	if (!g_vSchemeList || !g_pChromiumController)
		return;

	for (size_t x=0; x<g_vSchemeList->size(); x++)
		g_pChromiumController->RegisterSchemeExtender((*g_vSchemeList)[x]);

	g_vSchemeList->clear();
	safe_delete(g_vSchemeList);
}

void BrowserUICallback(ChromiumDLL::CallbackI* callback)
{
	if (g_pChromiumController)
		g_pChromiumController->PostCallback(callback);
}
