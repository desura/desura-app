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

#include "sqlite3x.hpp"
#include "sql/WebCoreSql.h"



using namespace WebCore;

namespace
{

	static std::vector<int> g_vFailedAuthCodes = {
		103, //invalid password
		104, //invalid account
		105, //inactive account
		106, //banned account
		109 //suspended account
	};

	bool isFailedAuthReason(int res)
	{
		return std::find(begin(g_vFailedAuthCodes), end(g_vFailedAuthCodes), res) != end(g_vFailedAuthCodes);
	}

}


const XML::gcXMLElement WebCoreClass::postToServer(std::string url, std::string resource, PostMap &postData, XML::gcXMLDocument &xmlDocument, bool useHTTPS)
{
	if (!useHTTPS && !m_bUserAuth)
		throw gcException(ERR_BAD_PORU, "You have been logged out from the server. Please login again");

	gcString httpOut;

	if (m_bDebuggingOut)
	{
		if (url.find('?') == std::string::npos)
			url += "?XDEBUG_SESSION_START=xdebug";
		else
			url += "&XDEBUG_SESSION_START=xdebug";
	}

	gcTrace("Hitting api {0}", url);

	{
		HttpHandle hh(url.c_str(), useHTTPS);

		if (useHTTPS)
		{
			hh->setUserAgent(getUserAgent());

			if (m_bValidateCert)
				hh->setCertFile(UTIL::STRING::toStr(UTIL::OS::getDataPath(L"ca-bundle.crt")).c_str());
		}
		else
		{
			setWCCookies(hh);
		}

		PostMap::iterator it = postData.begin();

		while (it != postData.end())
		{
			hh->addPostText(it->first.c_str(), it->second.c_str());
			it++;
		}

		hh->postWeb();

		m_cookieMap = hh->getCookies();
		
		if (hh->getDataSize() == 0)
			throw gcException(ERR_BADRESPONSE, "Data size was zero");

		xmlDocument.LoadBuffer(const_cast<char*>(hh->getData()), hh->getDataSize());

		if (m_bDebuggingOut)
			httpOut.assign(const_cast<char*>(hh->getData()), hh->getDataSize());
	}

	try
	{
		xmlDocument.ProcessStatus(resource);
	}
	catch (gcException& e)
	{
		if (!useHTTPS && isFailedAuthReason(e.getSecErrId())) //no login
		{
			logOut();
			onLoggedOutEvent();
			throw gcException(ERR_BAD_PORU, "You have been logged out from the server. Please login again");
		}
		else
		{
			throw;
		}
	}


	return xmlDocument.GetRoot(resource);
}

const XML::gcXMLElement WebCoreClass::loginToServer(std::string url, std::string resource, PostMap &postData, XML::gcXMLDocument &xmlDocument)
{
	auto uNode = postToServer(url, resource, postData, xmlDocument, true);

	// pull m_AWSELBCookie from cookies after login
	m_AWSELBCookie = m_cookieMap["AWSELB"];

	return uNode;
}

DesuraId WebCoreClass::nameToId(const char* name, const char* type)
{
	if (!name)
		throw gcException(ERR_BADITEM, "The name is nullptr");

	gcString key("{0}-{1}", name, type);
	uint32 hash = UTIL::MISC::RSHash_CSTR(key.c_str());

	try
	{
		sqlite3x::sqlite3_connection db(getWebCoreDb(m_szAppDataPath.c_str()).c_str());
		gcString q("select internalid from namecache where nameid='{0}' and ttl > DATETIME('NOW');",  hash);

		DesuraId id(db.executeint64(q.c_str()));
		if (id.isOk())
			return id;
	}
	catch(std::exception &)
	{
	}


	XML::gcXMLDocument doc;
	PostMap post;

	post["nameid"] = name;
	post["sitearea"] = type;

	auto uNode = postToServer(getNameLookUpUrl(), "iteminfo", post, doc);
	auto cNode = uNode.FirstChildElement("item");

	if (cNode.IsValid())
	{
		const std::string idStr = cNode.GetAtt("siteareaid");
		const std::string typeS = cNode.GetAtt("sitearea");

		DesuraId id(idStr.c_str(), typeS.c_str());

		if (!id.isOk() || DesuraId::getTypeString(id.getType()) != type)
		{
			throw gcException(ERR_BADXML);
		}
		else
		{
			try
			{
				sqlite3x::sqlite3_connection db(getWebCoreDb(m_szAppDataPath.c_str()).c_str());
				gcString q("replace into namecache (internalid, nameid, ttl) values ('{0}','{1}', DATETIME('NOW', '+5 day'));", id.toInt64(), hash);
				db.executenonquery(q.c_str());
			}
			catch(std::exception &ex)
			{
				Warning("Failed to update namecache in webcore: {0}\n", ex.what());
			}

			return id;
		}
	}

	throw gcException(ERR_BADXML);
}

DesuraId WebCoreClass::hashToId(const char* itemHashId)
{
	if (!itemHashId)
		throw gcException(ERR_BADITEM, "The hash is nullptr");


	try
	{
		sqlite3x::sqlite3_connection db(getWebCoreDb(m_szAppDataPath.c_str()).c_str());
		gcString q("select internalid from namecache where hashid='{0}' and ttl > DATETIME('NOW');", UTIL::MISC::RSHash_CSTR(itemHashId) );
		DesuraId id(db.executeint(q.c_str()));

		if (id.isOk())
			return id;
	}
	catch(std::exception &)
	{
	}


	XML::gcXMLDocument doc;
	PostMap post;

	post["hashid"] = itemHashId;

	auto uNode = postToServer(getNameLookUpUrl(), "iteminfo", post, doc);
	auto cNode = uNode.FirstChildElement("item");

	if (cNode.IsValid())
	{
		const std::string idStr = cNode.GetAtt("siteareaid");
		const std::string typeS = cNode.GetAtt("sitearea");

		if (typeS.empty() || idStr.empty())
		{
			throw gcException(ERR_BADXML);
		}
		else
		{
			DesuraId id(idStr.c_str(), typeS.c_str());

			try
			{
				sqlite3x::sqlite3_connection db(getWebCoreDb(m_szAppDataPath.c_str()).c_str());
				gcString q("replace into namecache (internalid, hashid, ttl) values ('{0}','{1}', DATETIME('NOW', '+5 day'));", id.toInt64(), UTIL::MISC::RSHash_CSTR(itemHashId));
				db.executenonquery(q.c_str());
			}
			catch(std::exception &ex)
			{
				Warning("Failed to update namecache in webcore: {0}\n", ex.what());
			}

			return id;
		}
	}

	throw gcException(ERR_BADXML);
}


void WebCoreClass::updateAccountItem(DesuraId id, bool add)
{
	XML::gcXMLDocument doc;
	PostMap post;

	post["siteareaid"] = id.getItem();
	post["sitearea"] = id.getTypeString();
	post["action"] = add?"add":"delete";

	postToServer(getUpdateAccountUrl(), "iteminstall", post, doc);
}

void WebCoreClass::newUpload(DesuraId id, const char* hash, uint64 fileSize, char **key)
{
	gcString size("{0}", fileSize);

	XML::gcXMLDocument doc;
	PostMap post;

	post["siteareaid"] = id.getItem();
	post["sitearea"] = id.getTypeString();
	post["action"] = "newupload";
	post["filehash"] = hash;
	post["filesize"] = size;

	auto uNode = postToServer(getMcfUploadUrl(), "itemupload", post, doc);
	auto iNode = uNode.FirstChildElement("mcf");

	if (!iNode.IsValid())
		throw gcException(ERR_BADXML);

	if (key)
	{
		const std::string text = iNode.GetAtt("key");

		if (text.empty())
			throw gcException(ERR_BADXML);

		Safe::strcpy(key, text.c_str(), text.size());
	}
}



void WebCoreClass::resumeUpload(DesuraId id, const char* key, WebCore::Misc::ResumeUploadInfo &info)
{
	XML::gcXMLDocument doc;
	PostMap post;

	post["siteareaid"] = id.getItem();
	post["sitearea"] = id.getTypeString();
	post["action"] = "resumeupload";
	post["key"] = key;

	auto uNode = postToServer(getMcfUploadUrl(), "itemupload", post, doc);
	auto iNode = uNode.FirstChildElement("mcf");

	if (!iNode.IsValid())
		throw gcException(ERR_BADXML);

	gcString complete;
	iNode.GetChild("complete", complete);

	if (complete == "1")
		throw gcException(ERR_COMPLETED);

	iNode.GetChild("date", info.szDate);
	iNode.GetChild("filehash", info.szHash);
	iNode.GetChild("filesize", info.size);
	iNode.GetChild("filesizeup", info.upsize);
}


void WebCoreClass::getItemInfo(DesuraId id, XML::gcXMLDocument &xmlDocument, MCFBranch mcfBranch, MCFBuild mcfBuild)
{
	PostMap post;

	post["siteareaid"] = id.getItem();
	post["sitearea"] = id.getTypeString();

	if (mcfBuild != 0)
		post["build"] = mcfBuild;

	if (mcfBranch != 0)
	{
		if (mcfBranch.isGlobal())
			post["branchglobal"] = mcfBranch;
		else
			post["branch"] = mcfBranch;
	}

	postToServer(getItemInfoUrl(), "iteminfo", post, xmlDocument);
}

gcString WebCoreClass::getCDKey(DesuraId id, MCFBranch branch)
{
	XML::gcXMLDocument doc;
	PostMap post;

	post["siteareaid"] = id.getItem();
	post["sitearea"] = id.getTypeString();
	post["branch"] = (size_t)branch;

#ifdef WIN32
	post["token"] =  UTIL::WIN::getRegValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Cryptography\\MachineGuid", true);
#else
	post["token"] = "todo";
#endif

	auto root = postToServer(getCDKeyUrl(), "cdkey", post, doc);
	auto key = root.FirstChildElement("key");

	if (!key.IsValid())
		throw gcException(ERR_BADXML);

	return key.GetText();
}

void WebCoreClass::logIn(const char* user, const char* pass, XML::gcXMLDocument &xmlDocument)
{
	if (m_bUserAuth)
		throw gcException(ERR_ALREADYLOGGEDIN);

	PostMap post;

	post["username"] = user;
	post["password"] = pass;

	auto uNode = loginToServer(getLoginUrl(), "memberlogin", post, xmlDocument);
	auto memNode = uNode.FirstChildElement("member");

	if (!memNode.IsValid())
		throw gcException(ERR_BADXML);

	const std::string idStr =  memNode.GetAtt("siteareaid");

	if (idStr.empty() || Safe::atoi(idStr.c_str()) <= 0)
		throw gcException(ERR_BAD_PORU);

	m_uiUserId = Safe::atoi(idStr.c_str());

	auto cookieNode = memNode.FirstChildElement("cookies");

	if (cookieNode.IsValid())
	{
		std::lock_guard<std::mutex> l(m_mSessLock);

		cookieNode.GetChild("id", m_szIdCookie);

		gcString strSession = cookieNode.GetChild("session");
		Safe::strncpy(const_cast<char*>(m_szSessCookie.data()), m_szSessCookie.size(), strSession.c_str(), strSession.size());
	}

	m_bUserAuth = true;
}

void WebCoreClass::logOut()
{
	std::lock_guard<std::mutex> l(m_mSessLock);

	m_bUserAuth = false;
	m_szIdCookie = gcString("");
	m_AWSELBCookie = "";
	m_szSessCookie[0] = '\0';
}

void WebCoreClass::getUpdatePoll(XML::gcXMLDocument &xmlDocument, const std::map<std::string, std::string> &post)
{
	PostMap postData;

	std::for_each(post.begin(), post.end(), [&postData](std::pair<std::string, std::string> p)
	{
		postData[p.first] = p.second;
	});

	postToServer(getUpdatePollUrl(), "updatepoll", postData, xmlDocument);
}

void WebCoreClass::getLoginItems(XML::gcXMLDocument &xmlDocument)
{
	PostMap postData;
	postToServer(getMemberDataUrl(), "memberdata", postData, xmlDocument);
}

void WebCoreClass::getDownloadProviders(DesuraId id, XML::gcXMLDocument &xmlDocument, MCFBranch mcfBranch, MCFBuild mcfBuild)
{
	PostMap postData;

	postData["siteareaid"] = id.getItem();
	postData["sitearea"] = id.getTypeString();
	postData["branch"] = (size_t) mcfBranch;

	if (mcfBuild != 0)
		postData["build"] = (size_t) mcfBuild;

	postToServer(getMCFDownloadUrl(), "itemdownloadurl", postData, xmlDocument);
}

gcString WebCoreClass::getAppUpdateDownloadUrl(uint32 &appId, uint32 &appBuild)
{
	gcAssert(appId);

	PostMap postData;

	postData["appid"] = appId;

	if (appBuild > 0)
		postData["build"] = appBuild;

	XML::gcXMLDocument doc;
	postToServer(getAppUpdateUrl(), "appupdate", postData, doc);

	auto mNode = doc.GetRoot("appupdate").FirstChildElement("mcf");

	if (!mNode.IsValid())
		throw gcException(ERR_BADXML);

	mNode.GetAtt("appid", appId);
	mNode.GetAtt("build", appBuild);

	if (appId == 0 || appBuild == 0)
		throw gcException(ERR_BADXML);

	gcString url = mNode.GetChild("url");

	if (url.size() == 0)
		throw gcException(ERR_BADXML);

	return url;
}
