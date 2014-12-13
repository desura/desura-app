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

#ifndef DESURA_GCWCUTIL_LEGACY_H
#define DESURA_GCWCUTIL_LEGACY_H

#define CEF_IGNORE_FUNCTIONS 1
#include "cef_desura_includes/ChromiumBrowserI.h"
#include "SharedObjectLoader.h"

#ifndef USE_CHROMIUM_API_V2

namespace ChromiumDLL
{
	class CookieI;
	class CallbackI;
	class JavaScriptExtenderI;
	class SchemeExtenderI;
	class ChromiumBrowserI;
	class ChromiumRendererI;

	typedef bool (*LogMessageHandlerFn)(int severity, const char* str);

	class ChromiumControllerI
	{
	public:
		//Gets the max api version this dll supports
		virtual int GetMaxApiVersion() = 0;

		//Sets the api version the client supports
		virtual void SetApiVersion(int nVersion) = 0;

		virtual void DoMsgLoop() = 0;
		virtual void Stop() = 0;

		virtual bool RegisterJSExtender(JavaScriptExtenderI* extender) = 0;
		virtual bool RegisterSchemeExtender(SchemeExtenderI* extender) = 0;

		virtual void DeleteCookie(const char* url, const char* name) = 0;
		virtual CookieI* CreateCookie() = 0;
		virtual void SetCookie(const char* ulr, CookieI* cookie) = 0;

		// Form handle as HWND
		virtual ChromiumBrowserI* NewChromiumBrowser(int* formHandle, const char *name, const char* defaultUrl) = 0;

		//Creates a offscreen browser renderer
		virtual ChromiumRendererI* NewChromiumRenderer(int* formHandle, const char* defaultUrl, int width, int height) = 0;

		// Return true to handle msg
		virtual void SetLogHandler(LogMessageHandlerFn logFn) = 0;
		virtual void PostCallback(CallbackI* callback) = 0;
	};
}

#else

namespace ChromiumDLL
{
	class ChromiumControllerI;
}

#endif

ChromiumDLL::ChromiumControllerI* CEF_Init_Legacy(SharedObjectLoader &hCefDll, bool threaded, const char* cachePath, const char* logPath, const char* userAgent);


#endif
