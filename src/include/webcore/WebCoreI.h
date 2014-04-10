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

#ifndef DESURA_WEBCOREI_H
#define DESURA_WEBCOREI_H
#ifdef _WIN32
#pragma once
#endif

#ifdef LINK_WITH_GMOCK
#include <gmock/gmock.h>
#endif

#include "ResumeUploadInfo.h"
#include "DLLVersion.h"
#include <atomic>

#define WEBCORE					"WEBCORE_INTERFACE_001"
#define WEBCORE_VER				"WEBCORE_VERSION"
#define WEBCORE_PASSREMINDER	"WEBCORE_PASSWORDREMINDER"
#define WEBCORE_USERAGENT		"WEBCORE_USERAGENT"

typedef void (*PassReminderFN)(const char*);
typedef gcString (*UserAgentFN)();
typedef const char* (*WebCoreVersionFN)();

namespace XML
{
	class gcXMLDocument;
}

namespace MCFCore 
{ 
	namespace Misc
	{ 
		class DownloadProvider;
	}
}

namespace WebCore
{

	namespace Misc
	{
		class DownloadImageInfo;
	}

	CEXPORT void* FactoryBuilder(const char* name);

	enum WebCoreUrl
	{
		Welcome,
		Cart,
		Purchase,
		Gifts,
		ActivateGame,
		Games,
		Mods,
		Community,
		Development,
		Support,
		McfUpload,
		Help,
		ThreadWatch,
		Inbox,
		Updates,
		LinuxToolHelp,
		ListKeys,
		Root,
		AppChangeLog,
		ComplexModTutorial,
		PlayJavaScript
	};

	class CookieCallbackI
	{
	public:
		virtual void operator()(const char* szRootUrl, const char* szName, const char* szValue)=0;
	};

	class WebCoreI
	{
	public:
		//! Init webcore
		//!
		//! @param appDataPath Path to save webcore cache db at
		//!
		virtual void init(const char* appDataPath)=0;

		//! Delete this instance
		//!
		virtual void destroy()=0;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Getters
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		//! Gets the id cookie
		//!
		//! @return id cookie
		//!
		virtual const char* getIdCookie()=0;

		//! Gets the session cookie
		//!
		//! @return Session cookie
		//!
		virtual const char* getSessCookie()=0;
	
		//! Gets the user agent for use in browsers and http connections
		//!
		//! @return Useragent string
		//!
		virtual const char* getUserAgent()=0;

		//! Gets item info from the web and saves it into the user
		//!
		//! @param internId Desura item internal id
		//! @param[out] doc Item xml
		//!
		virtual void getItemInfo(DesuraId id, XML::gcXMLDocument &xmlDocument, MCFBranch mcfBranch, MCFBuild mcfBuild)=0; 


		//! Gets all the items for the mod install wizard
		//!
		//! @param[out] doc Item list xml
		//!
		virtual void getInstalledItemList(XML::gcXMLDocument &xmlDocument)=0;

		//! Gets a cdkey for a branch
		//!
		//! @param id Item id
		//! @param branch Item branch id
		//! @return Cd key
		//!
		virtual gcString getCDKey(DesuraId id, MCFBranch branch)=0;

		//! When cookies get updated this event gets called
		//!
		//! @return CookieUpdate event
		//!
		virtual EventV* getCookieUpdateEvent()=0;


		//! Calls the update poll with the post data
		//!
		virtual void getUpdatePoll(XML::gcXMLDocument &xmlDocument, const std::map<std::string, std::string> &post)=0;

		//! Gets the items that where normally part of the login
		//!
		virtual void getLoginItems(XML::gcXMLDocument &xmlDocument)=0;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Setters
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		//! Sets all cookies at once. Only works if all three cookies are valid
		//!
		//! @param name Name cookie
		//! @param pass Password cookie
		//! @param sess Session cookie
		//!
		virtual void setCookie(const char* sess)=0;

		//! Sets the three cookies for a given httphandle
		//!
		//! @param hh HttpHandle to set cookies for
		//!
		virtual void setWCCookies(HttpHandle& hh)=0;


		//! Sets the root domain for all urls. Must be in form of desura.com not www.desura.com or http://desura.com
		//!
		//! @param domain Root domain
		//!
		virtual void setUrlDomain(const char* domain)=0;


		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Functions
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		//! Converts a short name to an Desura item id
		//!
		//! @param name Item name
		//! @param type Item type
		//! @return Desura item id
		//!
		virtual DesuraId nameToId(const char* name, const char* type)=0;

		//! Converts a hash to an Desura item id
		//!
		//! @param itemHashId Item hash
		//! @return Desura item id
		//!
		virtual DesuraId hashToId(const char* itemHashId)=0;

		//! Starts a new upload
		//! 
		//! @param id Item internal id
		//! @param hash Upload mcf hash
		//! @param fileSize Upload mcf size
		//! @param[out] key Upload key
		//!
		virtual void newUpload(DesuraId id, const char* hash, uint64 fileSize, char **key)=0;

		//! Gets info on a existing upload
		//!
		//! @param id Item internal id
		//! @param key Upload key
		//! @param[out] info ResumeUploadInfo object to store result in
		//!
		virtual void resumeUpload(DesuraId id, const char* key, WebCore::Misc::ResumeUploadInfo &info)=0;

		//! Download an image if it can
		//!
		//! @param id Item internal id
		//! @param[out] path Where the image got saved to
		//! @param name Item name
		//! @param postfix Append short name to image
		//! @param saveFolder Folder to save it to
		//!
		virtual void downloadImage(WebCore::Misc::DownloadImageInfo* dii, std::atomic<bool> &stop) = 0;

		//! Download a banner for a item download
		//! 
		//! @param dlp DownloadProvider information
		//!
		virtual void downloadBanner(MCFCore::Misc::DownloadProvider* dlp, const char* saveFolder)=0;

		//! Adds or removes items from user accounts
		//!
		//! @param internId Item interanl id
		//! @param add Add to account (if false it removes from account
		//!
		virtual void updateAccountItem(DesuraId id, bool add)=0;




		//! Checks the user login info
		//!
		//! @param user Username
		//! @param pass Password
		//! @param[out] doc Login xml
		//! @return Api Version Number
		//!
		virtual void logIn(const char* user, const char* pass, XML::gcXMLDocument &xmlDocument)=0;

		//! Logs the user out
		//!
		virtual void logOut()=0;

		//! Clears the name cache
		//!
		virtual void clearNameCache()=0;

		//! Enables debugging of api errors
		//!
		virtual void enableDebugging(bool state = true)=0;


		virtual gcString getUrl(WebCoreUrl id)=0;

		//! Init webcore
		//!
		//! @param appDataPath Path to save webcore cache db at
		//! @param szProviderUrl Url to use for server communication
		//!
		virtual void init(const char* appDataPath, const char* szProviderUrl)=0;

		//! Gets the download provider info for a item
		//!
		//! @param id Item id
		//! @param xmlDocument resulting api result
		//! @param mcfBranch branch of item
		//! @param mcfBuild build of item. Can be 0
		//!
		virtual void getDownloadProviders(DesuraId id, XML::gcXMLDocument &xmlDocument, MCFBranch mcfBranch, MCFBuild mcfBuild)=0;

		//! allows extern parts to use the cookies from webcore. Gets a callback for every cookie needed.
		//!
		//! @param pCallback Callback to use, caller responsible for deletion
		//!
		virtual void setCookies(CookieCallbackI *pCallback)=0;

		template <typename T>
		void setCookies(T &t)
		{
			class CCB : public CookieCallbackI
			{
			public:
				CCB(T &t)
					: m_tCallback(t)
				{
				}

				void operator()(const char* szRootUrl, const char* szName, const char* szValue) override
				{
					m_tCallback(szRootUrl, szName, szValue);
				}

				T &m_tCallback;
			};

			CCB ccb(t);
			setCookies(&ccb);
		}

		//! Gets a url for app update downloads
		//!
		//! Sets appId and appBuild to the values returned by the api.
		//!
		virtual gcString getAppUpdateDownloadUrl(uint32 &appId, uint32 &appBuild)=0;
	};
}


#ifdef LINK_WITH_GMOCK
#include "XMLMacros.h"

namespace WebCore
{
	class WebCoreMock : public WebCoreI
	{
	public:
		MOCK_METHOD1(init, void(const char* appDataPath));
		MOCK_METHOD0(destroy, void());
		MOCK_METHOD0(getIdCookie, const char*());
		MOCK_METHOD0(getSessCookie, const char*());
		MOCK_METHOD0(getUserAgent, const char*());
		MOCK_METHOD4(getItemInfo, void(DesuraId id, XML::gcXMLDocument &xmlDocument, MCFBranch mcfBranch, MCFBuild mcfBuild));
		MOCK_METHOD1(getInstalledItemList, void(XML::gcXMLDocument &xmlDocument));
		MOCK_METHOD2(getCDKey, gcString(DesuraId id, MCFBranch branch));
		MOCK_METHOD0(getCookieUpdateEvent, EventV*());
		MOCK_METHOD2(getUpdatePoll, void(XML::gcXMLDocument &xmlDocument, const std::map<std::string, std::string> &post));
		MOCK_METHOD1(getLoginItems, void(XML::gcXMLDocument &xmlDocument));
		MOCK_METHOD1(setCookie, void(const char* sess));
		MOCK_METHOD1(setWCCookies, void(HttpHandle& hh));
		MOCK_METHOD1(setUrlDomain, void(const char* domain));
		MOCK_METHOD2(nameToId, DesuraId(const char* name, const char* type));
		MOCK_METHOD1(hashToId, DesuraId(const char* itemHashId));
		MOCK_METHOD4(newUpload, void(DesuraId id, const char* hash, uint64 fileSize, char **key));
		MOCK_METHOD3(resumeUpload, void(DesuraId id, const char* key, WebCore::Misc::ResumeUploadInfo &info));
		MOCK_METHOD2(downloadImage, void(WebCore::Misc::DownloadImageInfo*, std::atomic<bool> &));
		MOCK_METHOD2(downloadBanner, void(MCFCore::Misc::DownloadProvider* dlp, const char* saveFolder));
		MOCK_METHOD2(updateAccountItem, void(DesuraId id, bool add));
		MOCK_METHOD3(logIn, void(const char* user, const char* pass, XML::gcXMLDocument &xmlDocument));
		MOCK_METHOD0(logOut, void());
		MOCK_METHOD0(clearNameCache, void());
		MOCK_METHOD1(enableDebugging, void(bool state));
		MOCK_METHOD1(getUrl, gcString(WebCoreUrl id));
		MOCK_METHOD2(init, void(const char* appDataPath, const char* szProviderUrl));
		MOCK_METHOD4(getDownloadProviders, void(DesuraId id, XML::gcXMLDocument &xmlDocument, MCFBranch mcfBranch, MCFBuild mcfBuild));
		MOCK_METHOD1(setCookies, void(CookieCallbackI *pCallback));
	};
}
#endif


#endif //DESURA_WEBCOREI_H
