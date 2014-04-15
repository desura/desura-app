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
#include "gcWCUtil_Legacy.h"





typedef bool (*CEF_InitFn)(bool, const char*, const char*, const char*);
typedef void(*CEF_SetApiVersionFn)(int);

typedef bool(*CEF_InitFn)(bool, const char*, const char*, const char*);
typedef void(*CEF_StopFn)();

typedef bool(*CEF_RegisterJSExtenderFn)(ChromiumDLL::JavaScriptExtenderI*);
typedef bool(*CEF_RegisterSchemeExtenderFn)(ChromiumDLL::SchemeExtenderI*);

typedef void(*CEF_DeleteCookieFn)(const char*, const char*);
typedef ChromiumDLL::CookieI* (*CEF_CreateCookieFn)();
typedef void(*CEF_SetCookieFn)(const char* ulr, ChromiumDLL::CookieI*);
typedef ChromiumDLL::ChromiumBrowserI* (*CEF_NewChromiumBrowserFn)(int*, const char *, const char*);

typedef void(*CEF_DoWorkFn)();

typedef void(*CEF_PostCallbackFn)(ChromiumDLL::CallbackI*);


static void CEF_Stop_Legacy();


class LegacyChromiumController : public ChromiumDLL::ChromiumControllerI
{
public:
	int GetMaxApiVersion() override
	{
		return 1;
	}

	//Sets the api version the client supports
	void SetApiVersion(int nVersion) override
	{
		gcAssert(CEF_Init);

		if (CEF_SetApiVersion)
			CEF_SetApiVersion(nVersion);
	}

	void DoMsgLoop() override
	{
		gcAssert(CEF_Init);

		if (CEF_DoWork)
			CEF_DoWork();
	}

	void Stop() override
	{
		gcAssert(CEF_Init);

		if (CEF_Stop)
			CEF_Stop();

		CEF_Stop_Legacy();
	}

	bool RegisterJSExtender(ChromiumDLL::JavaScriptExtenderI* extender) override
	{
		gcAssert(CEF_Init);

		if (CEF_RegisterJSExtender)
			return CEF_RegisterJSExtender(extender);

		return false;
	}


	bool RegisterSchemeExtender(ChromiumDLL::SchemeExtenderI* extender) override
	{
		gcAssert(CEF_Init);

		if (CEF_RegisterSchemeExtender)
			return CEF_RegisterSchemeExtender(extender);

		return false;
	}


	void DeleteCookie(const char* url, const char* name) override
	{
		gcAssert(CEF_Init);

		if (CEF_DeleteCookie)
			CEF_DeleteCookie(url, name);
	}

	ChromiumDLL::CookieI* CreateCookie() override
	{
		gcAssert(CEF_Init);

		if (CEF_CreateCookie)
			return CEF_CreateCookie();

		return nullptr;
	}

	void SetCookie(const char* url, ChromiumDLL::CookieI* cookie) override
	{
		gcAssert(CEF_Init);

		if (CEF_SetCookie)
			CEF_SetCookie(url, cookie);
	}


	// Form handle as HWND
	ChromiumDLL::ChromiumBrowserI* NewChromiumBrowser(int* formHandle, const char *name, const char* defaultUrl) override
	{
		gcAssert(CEF_Init);

		if (CEF_NewChromiumBrowser)
			return CEF_NewChromiumBrowser(formHandle, name, defaultUrl);

		return nullptr;
	}

	//Creates a offscreen browser renderer
	ChromiumDLL::ChromiumRendererI* NewChromiumRenderer(int* formHandle, const char* defaultUrl, int width, int height) override
	{
		gcAssert(false);
		return nullptr;
	}

	// Return true to handle msg
	void SetLogHandler(ChromiumDLL::LogMessageHandlerFn logFn) override
	{
		gcAssert(false);
	}

	void PostCallback(ChromiumDLL::CallbackI* callback) override
	{
		gcAssert(CEF_Init);

		if (CEF_PostCallback)
			CEF_PostCallback(callback);
	}

	bool Init(bool threaded, const char* cachePath, const char* logPath, const char* userAgent)
	{
		gcAssert(CEF_Init);

		if (CEF_Init)
			return CEF_Init(threaded, cachePath, logPath, userAgent);

		return false;
	}

	bool Load(SharedObjectLoader &hCefDll)
	{
		CEF_SetApiVersion = hCefDll.getFunction<CEF_SetApiVersionFn>("CEF_SetApiVersion", false);

		hCefDll.resetFailed();

		CEF_Init = hCefDll.getFunction<CEF_InitFn>("CEF_Init");
		CEF_Stop = hCefDll.getFunction<CEF_StopFn>("CEF_Stop");
		CEF_RegisterJSExtender = hCefDll.getFunction<CEF_RegisterJSExtenderFn>("CEF_RegisterJSExtender");
		CEF_RegisterSchemeExtender = hCefDll.getFunction<CEF_RegisterSchemeExtenderFn>("CEF_RegisterSchemeExtender");
		CEF_DeleteCookie = hCefDll.getFunction<CEF_DeleteCookieFn>("CEF_DeleteCookie");
		CEF_CreateCookie = hCefDll.getFunction<CEF_CreateCookieFn>("CEF_CreateCookie");
		CEF_SetCookie = hCefDll.getFunction<CEF_SetCookieFn>("CEF_SetCookie");
		CEF_NewChromiumBrowser = hCefDll.getFunction<CEF_NewChromiumBrowserFn>("CEF_NewChromiumBrowser");
		CEF_DoWork = hCefDll.getFunction<CEF_DoWorkFn>("CEF_DoMsgLoop");
		CEF_PostCallback = hCefDll.getFunction<CEF_PostCallbackFn>("CEF_PostCallback");

		if (hCefDll.hasFailed())
		{
			Warning("Failed to find cef library exports\n");
			return false;
		}

		return true;
	}

private:
	CEF_InitFn CEF_Init = nullptr;
	CEF_SetApiVersionFn CEF_SetApiVersion = nullptr;
	CEF_StopFn CEF_Stop = nullptr;
	CEF_RegisterJSExtenderFn CEF_RegisterJSExtender = nullptr;
	CEF_RegisterSchemeExtenderFn CEF_RegisterSchemeExtender = nullptr;
	CEF_DeleteCookieFn CEF_DeleteCookie = nullptr;
	CEF_CreateCookieFn CEF_CreateCookie = nullptr;
	CEF_SetCookieFn CEF_SetCookie = nullptr;
	CEF_NewChromiumBrowserFn CEF_NewChromiumBrowser = nullptr;
	CEF_DoWorkFn CEF_DoWork = nullptr;
	CEF_PostCallbackFn CEF_PostCallback = nullptr;
};

LegacyChromiumController *g_pLegacyController = nullptr;

ChromiumDLL::ChromiumControllerI* CEF_Init_Legacy(SharedObjectLoader &hCefDll, bool threaded, const char* cachePath, const char* logPath, const char* userAgent)
{
	if (g_pLegacyController)
		return g_pLegacyController;

	g_pLegacyController = new LegacyChromiumController();

	if (g_pLegacyController->Load(hCefDll) && g_pLegacyController->Init(threaded, cachePath, logPath, userAgent))
		return g_pLegacyController;
	
	delete g_pLegacyController;
	g_pLegacyController = nullptr;

	return g_pLegacyController;
}

static void CEF_Stop_Legacy()
{
	delete g_pLegacyController;
	g_pLegacyController = nullptr;
}