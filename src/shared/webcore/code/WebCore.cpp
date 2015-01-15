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
#include "WebCore.h"
#include "XMLMacros.h"

#ifdef WIN32
	#include <Wininet.h>
#endif

#include "mcfcore/MCFI.h"
#include "mcfcore/MCFMain.h"

#include "sqlite3x.hpp"
#include "sql/WebCoreSql.h"


static gcString g_szRootDomain = "desura.com";

namespace WebCore
{

extern gcString genUserAgent();

WebCoreClass::WebCoreClass()
	: m_ImageCache()
	, m_RefCount()
	, m_bDebuggingOut(false)
	, m_bUserAuth(false)
	, m_bValidateCert(true)
	, m_mSessLock()
	, m_szAppDataPath("")
	, m_szIdCookie("")
	, m_szUserAgent(genUserAgent())
	, m_uiUserId(0)
	, onCookieUpdateEvent()
	, onLoggedOutEvent()
{

#ifdef DEBUG
	m_bDebuggingOut = true;
#endif
}

WebCoreClass::~WebCoreClass()
{
	m_ImageCache.saveToDb();
}

void WebCoreClass::enableDebugging(bool state)
{
	m_bDebuggingOut = state;
}

void WebCoreClass::init(const char* appDataPath)
{
	init(appDataPath, nullptr);
}

void WebCoreClass::init(const char* appDataPath, const char* szProviderUrl)
{
	gcTrace("App data path {0}, Provider {1}", appDataPath, szProviderUrl);

	gcString strProvUrl(szProviderUrl);

	if (strProvUrl.size() == 0)
		setUrlDomain("desura.com");
	else
		setUrlDomain(strProvUrl.c_str());

	m_szAppDataPath = appDataPath;
	createWebCoreDbTables(appDataPath);

	m_ImageCache.init(appDataPath);
	m_ImageCache.loadFromDb();
}

void WebCoreClass::setUrlDomain(const char* domain)
{
	g_szRootDomain = domain;
	m_bValidateCert = g_szRootDomain == "desura.com";
}

gcString WebCoreClass::getMCFDownloadUrl()
{
	return gcString("http://api.") + g_szRootDomain + "/2/itemdownloadurl";
}

gcString WebCoreClass::getPassWordReminderUrl()
{
	return gcString("http://api.") + g_szRootDomain + "/1/memberpasswordreminder";
}

gcString WebCoreClass::getLoginUrl()
{
	return gcString("https://secure.") + g_szRootDomain + "/3/memberlogin";
}

gcString WebCoreClass::getRefreshUrl()
{
	return gcString("http://api.") + g_szRootDomain + "/1/refresh";
}

gcString WebCoreClass::getUpdatePollUrl()
{
	return gcString("http://api.") + g_szRootDomain + "/2/updatepoll";
}

gcString WebCoreClass::getNameLookUpUrl()
{
	return gcString("http://api.") + g_szRootDomain + "/1/itemnamelookup";
}

gcString WebCoreClass::getMcfUploadUrl()
{
	return gcString("http://api.") + g_szRootDomain + "/1/itemupload";
}

gcString WebCoreClass::getItemInfoUrl()
{
	return gcString("http://api.") + g_szRootDomain + "/2/item";
}

gcString WebCoreClass::getInstalledWizardUrl()
{
	return gcString("http://api.") + g_szRootDomain + "/2/itemwizard";
}

gcString WebCoreClass::getUpdateAccountUrl()
{
	return gcString("http://api.") + g_szRootDomain + "/1/iteminstall";
}

gcString WebCoreClass::getCDKeyUrl()
{
	return gcString("http://api.") + g_szRootDomain + "/1/cdkey";
}

gcString WebCoreClass::getMemberDataUrl()
{
	return gcString("http://api.") + g_szRootDomain + "/1/memberdata";
}

gcString WebCoreClass::getAppUpdateUrl()
{
	return gcString("http://api.") + g_szRootDomain + "/1/appupdate";
}

void WebCoreClass::setCookie(const char* sess)
{
	if (!sess)
		return;

	gcString strSess(sess);

	{
		std::lock_guard<std::mutex> l(m_mSessLock);

		if (strSess == gcString(m_szSessCookie))
			return;

		Safe::strncpy(const_cast<char*>(m_szSessCookie.data()), m_szSessCookie.size(), strSess.c_str(), strSess.size());
	}

	onCookieUpdateEvent();
}

void WebCoreClass::setCookies(gcRefPtr<CookieCallbackI> pCallback)
{
	gcAssert(pCallback);

	if (!pCallback)
		return;

	auto strRoot = getUrl(WebCore::Root);

	(*pCallback.get())(strRoot.c_str(), "freeman", m_szIdCookie.c_str());
	(*pCallback.get())(strRoot.c_str(), "masterchief", gcString(m_szSessCookie).c_str());
	(*pCallback.get())(strRoot.c_str(), "AWSELB", gcString(m_AWSELBCookie).c_str());
}

void WebCoreClass::setWCCookies(HttpHandle& hh)
{
	std::lock_guard<std::mutex> l(m_mSessLock);

	hh->addCookie("freeman", m_szIdCookie.c_str());
	hh->addCookie("masterchief", gcString(m_szSessCookie).c_str());
	hh->addCookie("AWSELB", gcString(m_AWSELBCookie).c_str());

	hh->setUserAgent(getUserAgent());
}

void WebCoreClass::clearNameCache()
{
	try
	{
		sqlite3x::sqlite3_connection db(getWebCoreDb(m_szAppDataPath.c_str()).c_str());
		db.executenonquery("DELETE FROM namecache");
	}
	catch (std::exception &ex)
	{
		Warning("Failed to clear namecache table: {0}\n", ex.what());
	}
}

gcString WebCoreClass::getUrl(WebCoreUrl id)
{
	gcString url("http://www.");
	url += g_szRootDomain;

	switch (id)
	{
	case Welcome:
		url += "?firsttime=t";
		break;

	case Cart:
		url = "https://secure." + g_szRootDomain + "/cart";
		break;

	case Purchase:
		url += "/cart/history";
		break;

	case Gifts:
		url += "/gifts";
		break;

	case ActivateGame:
		url += "/gifts";
		break;

	case Games:
		url += "/games";
		break;

	case Mods:
		url += "/mods";
		break;

	case Community:
		url += "/community";
		break;

	case Development:
		url += "/development";
		break;

	case Support:
		url += "/support";
		break;

	case Help:
		url += "/forum";
		break;

	case McfUpload:
		url = gcString("http://api.{0}/1/itemupload", g_szRootDomain);
		break;

	case ThreadWatch:
		url += "/forum/board/thread-watch";
		break;

	case Inbox:
		url += "/messages/inbox";
		break;

	case Updates:
		url += "/messages/updates";
		break;

	case LinuxToolHelp:
		url += "/tutorials/linux-tool-help";
		break;

	case ListKeys:
		url += "/collection";
		break;

	case AppChangeLog:
		url += "/app/changelog";
		break;

	case ComplexModTutorial:
		url += "/groups/desura/tutorials/complex-mod-installing";
		break;

	case PlayJavaScript:
		url += "/app/playjs";
		break;

	case Root:
		break;
	};

	return  url;
}


}


