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
#include "UpdateThread_Old.h"

#include "XMLMacros.h"
#include "User.h"
#ifdef WIN32
#include "GameExplorerManager.h"
#include "UpdateUninstallTask_win.h"
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

//onForcePoll

using namespace UserCore;


UpdateThreadOld::UpdateThreadOld(Event<std::tuple<gcOptional<bool>, gcOptional<bool>, gcOptional<bool>>> *onForcePollEvent, bool loadLoginItems)
	: m_pOnForcePollEvent(onForcePollEvent)
	, m_bLoadLoginItems(loadLoginItems)
{
	if (m_pOnForcePollEvent)
		*m_pOnForcePollEvent += delegate(this, &UpdateThreadOld::onForcePoll);
}

void UpdateThreadOld::onStop()
{
	m_WaitCond.notify();
	
	if (m_pOnForcePollEvent)
		*m_pOnForcePollEvent -= delegate(this, &UpdateThreadOld::onForcePoll);

	m_pOnForcePollEvent = nullptr;
}

void UpdateThreadOld::doRun()
{
#ifdef DESURA_OFFICIAL_BUILD
	gcString test = m_pUser->getCVarValue("gc_qa_testing");
	std::transform(begin(test), end(test), begin(test), ::tolower);

	m_bInternalTesting = test == "true" || test == "1";
#endif

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
			auto saveSecs = (int)(savetimer - now).seconds();
			auto pollSecs = (int)(timer - now).seconds();

			m_WaitCond.wait(std::min(pollSecs, saveSecs)+1);
		}
	}
}

void UpdateThreadOld::onForcePoll(std::tuple<gcOptional<bool>, gcOptional<bool>, gcOptional<bool>> &info)
{
	auto first = std::get<0>(info);
	auto second = std::get<1>(info);
	auto third = std::get<2>(info);

	if (first && *first)
	{
		Msg("\t-- Forcing update poll\n");
		m_bForcePoll = true;
	}
		
#ifdef DESURA_OFFICIAL_BUILD
	if (second && m_bInternalTesting != *second)
	{
		if (*second)
		{
			Msg("\t-- Internal Testing activated\n");
			m_bForceTestingUpdate = true;
			m_bForcePoll = true;
		}
		m_bInternalTesting = *second;
	}

	if (third && *third)
	{
		Msg("\t-- Forced testing update\n");
		m_bForceTestingUpdate = true;
		m_bForcePoll = true;
	}
#else
	//shouldn't be using these in non official builds
	gcAssert(!second);
	gcAssert(!third);
#endif

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
		Warning("Update poll failed: {0}\n", e);
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
		Warning("Update poll had bad status in xml! Status: {0}\n", e);
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

	if (!pUser->isAltProvider())
		checkAppUpdate(uNode);
	else
		Warning("Skipping app update check due to alt provider\n");

	tempNode = uNode.FirstChildElement("member");

	if (tempNode.IsValid())
	{
		tempNode = tempNode.FirstChildElement("avatar");

		if (tempNode.IsValid() && !tempNode.GetText().empty())
			m_pUser->setAvatarUrl(tempNode.GetText().c_str());
	}

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
		Warning("Failed to get login items: {0}\n", e);
	}

	im->enableSave();

#ifdef WIN32
	m_pUser->getThreadPool()->queueTask(new UpdateUninstallTask(m_pUser));
#endif

	m_pUser->getLoginItemsLoadedEvent()->operator()();
}



void UpdateThreadOld::checkAppUpdate(const XML::gcXMLElement &uNode, std::function<void(uint32, uint32, bool)> &updateCallback)
{
	auto processAppVersion = [&](const char* szNodeName, uint32 &appid, uint32 &mcfversion)
	{
		appid = 0;
		mcfversion = 0;

		auto appEl = uNode.FirstChildElement(szNodeName);

		if (!appEl.IsValid())
			return false;

		auto id = appEl.GetAtt("id");
		auto ver = appEl.GetText();

		if (!id.empty())
			appid = Safe::atoi(id.c_str());

		if (!ver.empty())
			mcfversion = Safe::atoi(ver.c_str());

		return appid != 0 && mcfversion != 0;
	};

	auto isInternalApp = [](uint32 appid)
	{
		return appid >= 500 && appid < 600;
	};

	uint32 mcfversion = 0;
	uint32 appid = 0;
	bool bIsNewerVersion = false;
	bool bIsForced = false;

	bool bIsQa = false;

#ifdef DESURA_OFFICIAL_BUILD
	if (m_bInternalTesting)
	{
		Msg("Checking internal testing for app update...\n");

		if (!processAppVersion("apptesting", appid, mcfversion) || !isInternalApp(appid))
		{
			Warning("Failed to find qa testing build on update poll");

			if (!processAppVersion("app", appid, mcfversion))
				return;
		}
		else
		{
			Msg(gcString("Found qa app build {0}.{1}\n", appid, mcfversion));

			if (m_bForceTestingUpdate)
			{
				Msg("Forcing testing update..\n");

				bIsForced = true;
				bIsNewerVersion = true;
			}

			m_bForceTestingUpdate = false;
			bIsQa = true;
		}
	}
	else if (!processAppVersion("app", appid, mcfversion))
	{
		return;
	}
#else
	if (!processAppVersion("app", appid, mcfversion))
	{
		return;
	}
#endif

	auto bNewerOriginalBranch = (m_uiLastAppId == 0) && (appid == m_iAppId) && (mcfversion > m_iAppVersion);
	auto bNewerLastUpdateBranch = (appid == m_uiLastAppId) && (mcfversion > m_uiLastVersion);
	auto bDiffBranch = (appid != m_iAppId) && (appid != m_uiLastAppId);

	if (bNewerLastUpdateBranch || bNewerOriginalBranch || bDiffBranch)
		bIsNewerVersion = true;

	if (bIsNewerVersion)
	{

#ifdef WIN32
		if (bIsQa && mcfversion > 0)
		{
			//need to set appver back a build otherwise updater will ignore this build due to it being older than current
			gcString strAppid("{0}", appid);
			gcString strAppVer("{0}", mcfversion - 1);

			m_pUser->getServiceMain()->updateRegKey(APPID, strAppid.c_str());
			m_pUser->getServiceMain()->updateRegKey(APPBUILD, strAppVer.c_str());
		}
#endif

		updateCallback(appid, mcfversion, bIsForced);
		m_uiLastAppId = appid;
		m_uiLastVersion = mcfversion;
	}
}


#ifdef DESURA_OFFICIAL_BUILD

void UpdateThreadOld::checkAppUpdate(const XML::gcXMLElement &uNode)
{
	UserCore::User *pUser = dynamic_cast<UserCore::User*>(m_pUser);

	if (!pUser)
		return;

	std::function<void(uint32,uint32, bool)> cb = std::bind(&UserCore::User::appNeedUpdate, pUser, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	checkAppUpdate(uNode, cb);
}

void UpdateThreadOld::updateBuildVer()
{
	std::string szAppid = UTIL::OS::getConfigValue(APPID);
	std::string szAppBuild = UTIL::OS::getConfigValue(APPBUILD);

	if (szAppid.size() > 0)
		m_iAppId = Safe::atoi(szAppid.c_str());

	if (szAppBuild.size() > 0)
		m_iAppVersion = Safe::atoi(szAppBuild.c_str());

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


#ifdef WITH_GTEST

#include <gtest/gtest.h>

namespace UnitTest
{
	class UpdateThreadOldFixture : public ::testing::Test
	{
	public:
		UpdateThreadOldFixture()
			: thread(nullptr, false)
		{
		}

		bool checkUpdate(uint32 appid, uint32 build)
		{
			XML::gcXMLDocument doc;
			doc.Create("apps");
			auto el = doc.GetRoot("apps").NewElement("app");
			el.SetAttribute("id", appid);
			el.SetText(gcString("{0}", build).c_str());

			bool bNeedsUpdate = false;

			std::function<void(uint32,uint32, bool)> callback = [&](uint32, uint32, bool)
			{
				bNeedsUpdate = true;
			};

			thread.checkAppUpdate(doc.GetRoot("apps"), callback);
			return bNeedsUpdate;
		}

		void setAppVersion(uint32 appid, uint32 build)
		{
			thread.m_iAppId = appid;
			thread.m_iAppVersion = build;
		}

		void setLastUpdateVersion(uint32 appid, uint32 build)
		{
			thread.m_uiLastAppId = appid;
			thread.m_uiLastVersion = build;
		}

		UpdateThreadOld thread;
	};


	TEST_F(UpdateThreadOldFixture, invalidApp)
	{
		ASSERT_TRUE(checkUpdate(1, 3));
	}


	TEST_F(UpdateThreadOldFixture, newVersionSameApp)
	{
		setAppVersion(1, 2);
		ASSERT_TRUE(checkUpdate(1, 3));
	}

	TEST_F(UpdateThreadOldFixture, oldVersionSameApp)
	{
		setAppVersion(1, 3);
		ASSERT_FALSE(checkUpdate(1, 2));
	}

	TEST_F(UpdateThreadOldFixture, sameVersionSameApp)
	{
		setAppVersion(1, 2);
		ASSERT_FALSE(checkUpdate(1, 2));
	}



	TEST_F(UpdateThreadOldFixture, newVersionDiffApp)
	{
		setAppVersion(1, 2);
		ASSERT_TRUE(checkUpdate(2, 3));
	}

	TEST_F(UpdateThreadOldFixture, oldVersionDiffApp)
	{
		setAppVersion(1, 3);
		ASSERT_TRUE(checkUpdate(2, 2));
	}

	TEST_F(UpdateThreadOldFixture, sameVersionDiffApp)
	{
		setAppVersion(1, 2);
		ASSERT_TRUE(checkUpdate(2, 2));
	}



	 
	TEST_F(UpdateThreadOldFixture, newVersionSameApp_sameVersionSameAppPoll)
	{
		setAppVersion(1, 2);
		setLastUpdateVersion(1, 3);

		ASSERT_FALSE(checkUpdate(1, 3));
	}

	TEST_F(UpdateThreadOldFixture, oldVersionSameApp_sameVersionSameAppPoll)
	{
		setAppVersion(1, 3);
		setLastUpdateVersion(1, 2);

		ASSERT_FALSE(checkUpdate(1, 2));
	}

	TEST_F(UpdateThreadOldFixture, sameVersionSameApp_sameVersionSameAppPoll)
	{
		setAppVersion(1, 2);
		setLastUpdateVersion(1, 2);

		ASSERT_FALSE(checkUpdate(1, 2));
	}



	TEST_F(UpdateThreadOldFixture, newVersionSameApp_olderVersionSameAppPoll)
	{
		setAppVersion(1, 2);
		setLastUpdateVersion(1, 2);

		ASSERT_TRUE(checkUpdate(1, 3));
	}


	TEST_F(UpdateThreadOldFixture, newVersionSameApp_newerVersionSameAppPoll)
	{
		setAppVersion(1, 2);
		setLastUpdateVersion(1, 4);

		ASSERT_FALSE(checkUpdate(1, 3));
	}

	TEST_F(UpdateThreadOldFixture, oldVersionSameApp_newerVersionSameAppPoll)
	{
		setAppVersion(1, 3);
		setLastUpdateVersion(1, 4);

		ASSERT_FALSE(checkUpdate(1, 2));
	}

	TEST_F(UpdateThreadOldFixture, sameVersionSameApp_newerVersionSameAppPoll)
	{
		setAppVersion(1, 2);
		setLastUpdateVersion(1, 3);

		ASSERT_FALSE(checkUpdate(1, 2));
	}






	TEST_F(UpdateThreadOldFixture, newVersionSameApp_sameVersionDiffAppPoll)
	{
		setAppVersion(1, 2);
		setLastUpdateVersion(2, 3);

		ASSERT_TRUE(checkUpdate(3, 3));
	}

	TEST_F(UpdateThreadOldFixture, oldVersionSameApp_sameVersionDiffAppPoll)
	{
		setAppVersion(1, 3);
		setLastUpdateVersion(2, 3);

		ASSERT_TRUE(checkUpdate(3, 2));
	}

	TEST_F(UpdateThreadOldFixture, sameVersionSameApp_sameVersionDiffAppPoll)
	{
		setAppVersion(1, 2);
		setLastUpdateVersion(2, 2);

		ASSERT_TRUE(checkUpdate(3, 2));
	}





}

#endif

