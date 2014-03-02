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