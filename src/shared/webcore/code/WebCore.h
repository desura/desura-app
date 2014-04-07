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

#ifndef DESURA_WEBCORE_H
#define DESURA_WEBCORE_H

#include "webcore/WebCoreI.h"
#include "util_thread/BaseThread.h"
#include "ImageCache.h"

#include <array>
#include <mutex>

namespace sqlite3x
{
	class sqlite3_connection;
}

namespace XML
{
	class gcXMLElement;
}


namespace WebCore
{

class PostString : public std::string
{
public:
	std::string& operator=(const uint32 &val)
	{
		std::string::operator=(gcString("{0}", val));
		return *this;
	}

	std::string& operator=(const std::string &val)
	{
		std::string::operator=(val);
		return *this;
	}

	std::string& operator=(const char* val)
	{
		std::string::operator=(val);
		return *this;
	}
};

typedef std::map<std::string, PostString> PostMap;


class WebCoreClass : public WebCoreI
{
public:
	WebCoreClass();
	~WebCoreClass();

	virtual void init(const char* appDataPath, const char* szProviderUrl) override;
	virtual void init(const char* appDataPath);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Getters
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual const char* getIdCookie();
	virtual const char* getSessCookie();

	virtual const char* getUserAgent();
	virtual void getItemInfo(DesuraId id, XML::gcXMLDocument &xmlDocument, MCFBranch mcfBranch, MCFBuild mcfBuild); 
	virtual void getInstalledItemList(XML::gcXMLDocument &xmlDocument);
	virtual EventV* getCookieUpdateEvent();
	virtual gcString getCDKey(DesuraId id, MCFBranch branch);
	virtual void getUpdatePoll(XML::gcXMLDocument &xmlDocument, const std::map<std::string, std::string> &post);
	virtual void getLoginItems(XML::gcXMLDocument &xmlDocument);
	void getDownloadProviders(DesuraId id, XML::gcXMLDocument &xmlDocument, MCFBranch mcfBranch, MCFBuild mcfBuild) override;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Setters
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	void setCookie(const char* sess)  override;
	void setCookies(CookieCallbackI *pCallback) override;
	void setWCCookies(HttpHandle& hh)  override;
	void setUrlDomain(const char* domain)  override;
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Functions
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual DesuraId nameToId(const char* name, const char* type);
	virtual DesuraId hashToId(const char* itemHashId);
	virtual void newUpload(DesuraId id, const char* hash, uint64 fileSize, char **key);
	virtual void resumeUpload(DesuraId id, const char* key, WebCore::Misc::ResumeUploadInfo &info);
	virtual void downloadImage(WebCore::Misc::DownloadImageInfo *dii, std::atomic<bool> &stop);
	virtual void downloadBanner(MCFCore::Misc::DownloadProvider* dlp, const char* saveFolder);
	virtual void updateAccountItem(DesuraId id, bool add);
	virtual void clearNameCache();


	virtual void enableDebugging(bool state = true);

	virtual void destroy()
	{
		delete this;
	}

	virtual gcString getUrl(WebCoreUrl id);

	gcString getAppUpdateDownloadUrl(uint32 &appId, uint32 &appBuild) override;

	//! Sends a password reminder to the dest email
	//!
	//! @param email Email to send reminder to
	//!
	static void sendPassReminder(const char* email);

	gcString getMCFDownloadUrl();

	static gcString getPassWordReminderUrl();
	static gcString getLoginUrl();
	static gcString getRefreshUrl();
	static gcString getUpdatePollUrl();
	static gcString getNameLookUpUrl();
	static gcString getMcfUploadUrl();
	static gcString getItemInfoUrl();
	static gcString getInstalledWizardUrl();
	static gcString getUpdateAccountUrl();
	static gcString getCDKeyUrl();
	static gcString getMemberDataUrl();
	static gcString getAppUpdateUrl();

protected:
	virtual void logIn(const char* user, const char* pass, XML::gcXMLDocument &xmlDocument);
	virtual void logOut();

	void startRefreshCycle();
	void stopRefreshCycle();

	const XML::gcXMLElement postToServer(std::string url, std::string resource, PostMap &postData, XML::gcXMLDocument &xmlDocument, bool useHTTPS = false);
	const XML::gcXMLElement loginToServer(std::string url, std::string resource, PostMap &postData, XML::gcXMLDocument &xmlDocument);

	EventV onCookieUpdateEvent;

	void onHttpProg(std::atomic<bool>* stop, Prog_s& prog);

private:
	bool m_bUserAuth;
	uint32 m_uiUserId;

	gcString m_szUserAgent;
	gcString m_szIdCookie;
	std::array<char, 4096> m_szSessCookie;
	gcString m_szAppDataPath;

	bool m_bDebuggingOut;
	bool m_bValidateCert;

	ImageCache m_ImageCache;

	std::mutex m_mSessLock;
};

inline const char* WebCoreClass::getUserAgent()
{
	return m_szUserAgent.c_str();
}

inline const char* WebCoreClass::getIdCookie()
{
	return m_szIdCookie.c_str();
}

inline const char* WebCoreClass::getSessCookie()
{
	std::lock_guard<std::mutex> l(m_mSessLock);
	return m_szSessCookie.data();
}

inline EventV* WebCoreClass::getCookieUpdateEvent()
{
	return &onCookieUpdateEvent;
}

}

#endif
