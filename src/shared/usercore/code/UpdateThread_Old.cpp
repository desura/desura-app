/*
Desura is the leading indie game distribution platform
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include "Common.h"
#include "UpdateThread_Old.h"

#include "XMLMacros.h"
#include "User.h"
#ifdef WIN32
#include "GameExplorerManager.h"
#endif
#ifdef NIX
#include "util/UtilLinux.h"
#endif
#include <branding/branding.h>

#include "util/gcTime.h"

#define SAVE_TIME	30 //seconds

#ifdef DEBUG
	#define UPDATE_TIME	1 //mins
#else
	#define UPDATE_TIME	9 //mins
#endif

namespace UserCore
{

UpdateThreadOld::UpdateThreadOld(EventV *onForcePollEvent, bool loadLoginItems)
{
	m_bForcePoll = false;
	m_pOnForcePollEvent = onForcePollEvent;

	if (m_pOnForcePollEvent)
		*m_pOnForcePollEvent += delegate(this, &UpdateThreadOld::onForcePoll);

	m_pUser = NULL;
	m_pWebCore = NULL;

	m_bLoadLoginItems = loadLoginItems;
}

void UpdateThreadOld::onStop()
{
	m_WaitCond.notify();
	
	if (m_pOnForcePollEvent)
		*m_pOnForcePollEvent -= delegate(this, &UpdateThreadOld::onForcePoll);

	m_pOnForcePollEvent = NULL;
}

void UpdateThreadOld::init()
{
	m_iAppId = 100; // Ignoring updates anyway.
	m_iAppVersion = 0;
	
	m_uiLastAppId = 0;
	m_uiLastVersion = 0;
}

void UpdateThreadOld::doRun()
{
	init();

	if (m_bLoadLoginItems)
		loadLoginItems();

	gcTime timer;
	gcTime savetimer;

	//see where we are within the 10 min window
	uint32 offset = timer.seconds() + (timer.minutes()%6)*60;
	bool lastFailed = false;

	while (!isStopped())
	{
		gcTime now;

		if (now >= savetimer)
		{
			m_pUser->getItemManager()->saveItems();
			m_pUser->getToolManager()->saveItems();

#ifdef WIN32
			m_pUser->getGameExplorerManager()->saveItems();
#endif

			savetimer = now;
			savetimer += std::chrono::seconds(SAVE_TIME);
		}

		if (now >= timer || m_bForcePoll)
		{
			timer = now;

			if (pollUpdates())
			{
				if (lastFailed || m_bForcePoll)
					timer -= std::chrono::seconds(offset);

				timer += std::chrono::minutes(UPDATE_TIME);
				lastFailed = false;
			}
			else
			{
				lastFailed = true;
				timer += std::chrono::minutes(1);
			}

			m_bForcePoll = false;
		}

		if (!isStopped())
		{
			auto saveSecs = (savetimer - now).seconds();
			auto pollSecs = (timer - now).seconds();

			m_WaitCond.wait(std::min(pollSecs, saveSecs)+1);
		}
	}
}

void UpdateThreadOld::onForcePoll()
{
	Msg("\t-- Forcing update poll\n");
	m_bForcePoll = true;
	m_WaitCond.notify();
}

bool UpdateThreadOld::pollUpdates()
{
	if (!m_pWebCore || !m_pUser)
		return false;

	updateBuildVer();

	std::map<std::string, std::string> post;

	post["appid"] = gcString("{0}", m_iAppId);
	post["build"] = gcString("{0}", m_iAppVersion);

	for (uint32 x=0; x< m_pUser->getItemManager()->getCount(); x++)
	{
		UserCore::Item::ItemInfoI* item = m_pUser->getItemManager()->getItemInfo(x);

		if (!item)
			continue;

		if (!(item->getStatus() & UserCore::Item::ItemInfoI::STATUS_ONCOMPUTER) || (item->getStatus()&UserCore::Item::ItemInfoI::STATUS_ONACCOUNT))
			continue;

		DesuraId id = item->getId();

		if (id.getType() == DesuraId::TYPE_LINK)
			continue;

		gcString key("updates[{0}][{1}]", id.getTypeString().c_str(), id.getItem());
		post[key] = "1";
	}

	XML::gcXMLDocument doc;

	try
	{
		m_pWebCore->getUpdatePoll(doc, post);
	}
	catch (gcException &e)
	{
		Warning(gcString("Update poll failed: {0}\n", e));
		return false;
	}

	parseXML(doc);

#ifdef WIN32
	checkFreeSpace();
#endif

	m_hHttpHandle->cleanPostInfo();
	m_hHttpHandle->cleanUp();
	m_hHttpHandle->clearCookies();

	return true;
}

#ifdef WIN32
void UpdateThreadOld::checkFreeSpace()
{
	gcString sysPath = UTIL::WIN::getWindowsPath();
	gcString dataPath = UTIL::OS::getMcfCachePath();

	if (dataPath.size() < 1 || dataPath[1] != ':')
		dataPath = UTIL::OS::getCurrentDir();

	uint64 space = UTIL::OS::getFreeSpace(dataPath.c_str());
	uint64 limit = 3221225472ull; //3gb

	if (space < limit)
	{
		std::pair<bool, char> arg = std::pair<bool, char>((sysPath[0] == dataPath[0]), dataPath[0]);
		m_pUser->getLowSpaceEvent()->operator()(arg);
	}
}
#endif


void UpdateThreadOld::parseXML(const XML::gcXMLDocument &doc)
{
	UserCore::User *pUser = dynamic_cast<UserCore::User*>(m_pUser);

	if (!pUser)
		return;

	int version = 1;

	try
	{
		version = doc.ProcessStatus("updatepoll");
	}
	catch (gcException &e)
	{
		Warning(gcString("Update poll had bad status in xml! Status: {0}\n", e));
		return;
	}

	auto uNode = doc.GetRoot("updatepoll");
	
	if (!uNode.IsValid())
		return;

	auto tempNode = uNode.FirstChildElement("cookies");


	if (tempNode.IsValid())
	{
		gcString szSessCookie;
		tempNode.GetChild("session", szSessCookie);

		if (szSessCookie != "")
			m_pWebCore->setCookie(szSessCookie.c_str());
	}

	tempNode = uNode.FirstChildElement("messages");

	if (tempNode.IsValid())
	{
		int32 up = -1;
		int32 pm = -1;
		int32 cc = -1;
		int32 th = -1;

		tempNode.GetChild("updates", up);
		tempNode.GetChild("privatemessages", pm);
		tempNode.GetChild("cart", cc);
		tempNode.GetChild("threadwatch", th);

		pUser->setCounts(pm, up, th, cc);
	}

	checkAppUpdate(uNode);

	if (version == 1)
	{
		tempNode = uNode.FirstChildElement("items");

		if (tempNode.IsValid())
			pUser->getItemManager()->itemsNeedUpdate(tempNode);
	}
	else
	{
		tempNode = uNode.FirstChildElement("platforms");

		if (tempNode.IsValid())
			pUser->getItemManager()->itemsNeedUpdate2(tempNode);
	}

	tempNode = uNode.FirstChildElement("news");

	if (tempNode.IsValid())
		pUser->parseNews(tempNode);


	tempNode = uNode.FirstChildElement("gifts");

	if (tempNode.IsValid())
		pUser->parseGifts(tempNode);
}

bool UpdateThreadOld::onMessageReceived(const char* resource, const XML::gcXMLElement &root)
{
	return false;
}

void UpdateThreadOld::setInfo(UserCore::UserI* user, WebCore::WebCoreI* webcore)
{
	m_pUser = user;
	m_pWebCore = webcore;
}

void UpdateThreadOld::loadLoginItems()
{
	UserCore::ItemManager* im = dynamic_cast<UserCore::ItemManager*>(m_pUser->getItemManager());

	XML::gcXMLDocument doc;

	try
	{
		m_pWebCore->getLoginItems(doc);

		auto first = doc.GetRoot("memberdata");
		im->parseLoginXml2(first.FirstChildElement("games"), first.FirstChildElement("platforms"));
	}
	catch (gcException &e)
	{
		Warning(gcString("Failed to get login items: {0}\n", e));
	}

	im->enableSave();
	m_pUser->getLoginItemsLoadedEvent()->operator()();
}


#ifdef DESURA_OFFICAL_BUILD

void UpdateThreadOld::checkAppUpdate(const XML::gcXMLElement &uNode)
{
	TiXmlElement* appEl = uNode->FirstChildElement("app");

	if (!appEl)
		return;

	UserCore::User *pUser = dynamic_cast<UserCore::User*>(m_pUser);

	if (!pUser)
		return;

	uint32 mcfversion = 0;
	uint32 appid = 0;

	const char* id = appEl->Attribute("id");
	const char* ver = appEl->GetText();

	if (id)
		appid = atoi(id);
		
	if (ver)
		mcfversion = atoi(ver);

	if (appid != 0 && mcfversion != 0 && !(appid == m_iAppId && mcfversion <= m_iAppVersion ) && !(appid == m_uiLastAppId && m_uiLastAppId >= mcfversion))
		pUser->appNeedUpdate(appid, mcfversion);
}

void UpdateThreadOld::updateBuildVer()
{
	std::string szAppid = UTIL::OS::getConfigValue(APPID);
	std::string szAppBuild = UTIL::OS::getConfigValue(APPBUILD);

	if (szAppid.size() > 0)
		m_iAppId = atoi(szAppid.c_str());

	if (szAppBuild.size() > 0)
		m_iAppVersion = atoi(szAppBuild.c_str());

	//if admin, give them internal build
	if (m_pUser->isAdmin() && m_iAppId != BUILDID_INTERNAL)
	{
		m_iAppId = BUILDID_INTERNAL;
		m_iAppVersion = 0;
	}
}

#else

void UpdateThreadOld::checkAppUpdate(const XML::gcXMLElement &uNode)
{
}

void UpdateThreadOld::updateBuildVer()
{
	UTIL::OS::setConfigValue(APPID, 999);
	UTIL::OS::setConfigValue(APPBUILD, 0);
}

#endif




}
