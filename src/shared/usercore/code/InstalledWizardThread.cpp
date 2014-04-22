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
#include "InstalledWizardThread.h"
#include "InstallInfo.h"
#include "ItemTaskGroup.h"
#include "ItemManager.h"

#include "User.h"

//This is also defined in CIPGrid in uicore
#define CREATE_CIPITEMINFO "create table cipiteminfo(internalid INTEGER PRIMARY KEY, name TEXT);"
#define COUNT_CIPITEMINFO "select count(*) from sqlite_master where name='cipiteminfo';"

#include "sqlite3x.hpp"
#include "sql/CustomInstallPathSql.h"

namespace UserCore
{
namespace Thread
{

InstalledWizardThread::InstalledWizardThread() : MCFThread( "Installed Wizard Thread" )
{
	m_bTriggerNewEvent = true;
	m_pTaskGroup = nullptr;

	m_uiDone = 0;
	m_uiTotal = 0;
}

InstalledWizardThread::~InstalledWizardThread()
{
	stop();
}


void InstalledWizardThread::doRun()
{
	m_szDbName = getCIBDb(getUserCore()->getAppDataPath());

	XML::gcXMLDocument doc;
	getWebCore()->getInstalledItemList(doc);


	int ver = doc.ProcessStatus("itemwizard");

	auto infoNode = doc.GetRoot("itemwizard");

	if (!infoNode.IsValid())
		throw gcException(ERR_BADXML);

	WildcardManager wMng = WildcardManager();
	wMng.onNeedSpecialEvent += delegate(&onNeedWCEvent);
	wMng.onNeedSpecialEvent += delegate(getUserCore()->getNeedWildCardEvent());
	

	if (isStopped())
		return;

	parseItemsQuick(infoNode);
	
	MCFCore::Misc::ProgressInfo pi = MCFCore::Misc::ProgressInfo();
	pi.percent = 0;
	onMcfProgressEvent(pi);

	if (ver == 1)
		parseItems1(infoNode, &wMng);
	else
		parseItems2(infoNode, &wMng);

	if (m_pTaskGroup)
	{
		m_pTaskGroup->finalize();
		m_pTaskGroup = nullptr;
	}

	try
	{
		createCIPDbTables(getUserCore()->getAppDataPath());

		sqlite3x::sqlite3_connection db(m_szDbName.c_str());
		sqlite3x::sqlite3_command cmd(db, "REPLACE INTO cipiteminfo (internalid, name) VALUES (?,?);");

		sqlite3x::sqlite3_transaction trans(db);

		for (size_t x=0; x<m_vGameList.size(); x++)
		{
			cmd.bind(1, (long long int)m_vGameList[x].getId().toInt64());
			cmd.bind(2, std::string(m_vGameList[x].getName()) ); 
			cmd.executenonquery();
		}

		trans.commit();
	}
	catch (std::exception &e)
	{
		Warning("Failed to update cip item list: {0}\n", e.what());
	}


	pi.percent = 100;
	onMcfProgressEvent(pi);

	uint32 prog = 0;
	onCompleteEvent(prog);
}


void InstalledWizardThread::onGameFound(UserCore::Misc::InstallInfo &game)
{
	if (isStopped())
		return;	

	m_vGameList.push_back(game);
	DesuraId id = game.getId();

	UserCore::Item::ItemInfoI *info = getUserCore()->getItemManager()->findItemInfo(id);
	if (info && (info->getStatus() & (UserCore::Item::ItemInfoI::STATUS_INSTALLED|UserCore::Item::ItemInfoI::STATUS_READY)))
		return;

	if (game.isInstalled())
	{
		try
		{
			WildcardManager tempwMng;
			tempwMng.onNeedSpecialEvent += delegate(&onNeedWCEvent);
			tempwMng.onNeedSpecialEvent += delegate(getUserCore()->getNeedWildCardEvent());

			getUserCore()->getItemManager()->retrieveItemInfo(id, 0, &tempwMng);

			UserCore::Item::ItemInfoI *item = getUserCore()->getItemManager()->findItemInfo(id);
			if ((!item)|| (item->getParentId().isOk() && !getUserCore()->getItemManager()->isInstalled(item->getParentId())))
				return;

			onItemFound(item);
		}
		catch (gcException &except)
		{
			Warning("Installed wizard had an error in checking games: {0}\n", except);
		}
	}
}

void InstalledWizardThread::onModFound(UserCore::Misc::InstallInfo &mod)
{
	if (isStopped())
		return;

	DesuraId id = mod.getId();

	UserCore::Item::ItemInfoI *info = getUserCore()->getItemManager()->findItemInfo(id);
	if (info && (info->getStatus() & (UserCore::Item::ItemInfoI::STATUS_INSTALLED|UserCore::Item::ItemInfoI::STATUS_READY)))
		return;

	if (mod.isInstalled())
	{
		try
		{
			WildcardManager tempwMng;
			tempwMng.onNeedSpecialEvent += delegate(&onNeedWCEvent);
			tempwMng.onNeedSpecialEvent += delegate(getUserCore()->getNeedWildCardEvent());

			getUserCore()->getItemManager()->retrieveItemInfo(id, 0, &tempwMng);


			UserCore::Item::ItemInfoI *item = getUserCore()->getItemManager()->findItemInfo(id);
			if ((!item ) || (item->getParentId().isOk() && !getUserCore()->getItemManager()->isInstalled(item->getParentId())))
				return;

			if (selectBranch(item))
			{
				//we have to do this here as the item info above doesnt save this flag for downloadable items.
				item->addSFlag(UserCore::Item::ItemInfoI::STATUS_INSTALLED);
				onItemFound(item);
			}
		}
		catch (gcException &except)
		{
			Warning("Installed wizard had an error in checking mods: {0}\n", except);
		}
	}
}

bool InstalledWizardThread::selectBranch(UserCore::Item::ItemInfoI *item)
{
	std::vector<uint32> vBranchIdList;

	for (uint32 x=0; x<item->getBranchCount(); x++)
	{
		UserCore::Item::BranchInfoI* bi = item->getBranch(x);

		if (!bi)
			continue;

		uint32 flags = bi->getFlags();

		if (HasAnyFlags(flags, UserCore::Item::BranchInfoI::BF_NORELEASES|UserCore::Item::BranchInfoI::BF_DEMO|UserCore::Item::BranchInfoI::BF_TEST))
			continue;

		if (!HasAllFlags(flags, UserCore::Item::BranchInfoI::BF_ONACCOUNT))
		{
			if (HasAnyFlags(flags, (UserCore::Item::BranchInfoI::BF_MEMBERLOCK|UserCore::Item::BranchInfoI::BF_REGIONLOCK)))
				continue;
		}

		if (!HasAllFlags(flags, UserCore::Item::BranchInfoI::BF_ONACCOUNT) && !(HasAllFlags(flags, UserCore::Item::BranchInfoI::BF_FREE)))
			continue;

		vBranchIdList.push_back(bi->getBranchId());
	}

	if (vBranchIdList.size() == 0)
		return false;
	
	//select the first free branch
	item->setInstalledMcf(MCFBranch::BranchFromInt(vBranchIdList[0]));
	return true;
}

void InstalledWizardThread::parseGameQuick(const XML::gcXMLElement &game)
{
	if (!game.IsValid())
		return;

	m_uiTotal++;

	game.FirstChildElement("mods").for_each_child("mod", [&](const XML::gcXMLElement &)
	{
		m_uiTotal++;
	});
}

void InstalledWizardThread::parseItemsQuick(const XML::gcXMLElement &fNode)
{
	if (!fNode.IsValid())
		return;

	auto platforms = fNode.FirstChildElement("platforms");

	if (platforms.IsValid())
	{
		platforms.for_each_child("platform", [&](const XML::gcXMLElement &platform)
		{
			if (isStopped())
				return;

			if (getUserCore()->platformFilter(platform, PlatformType::Item))
				return;

			parseItemsQuick(platform);
		});
	}
	else
	{
		fNode.FirstChildElement("games").for_each_child("game", [&](const XML::gcXMLElement &game)
		{
			if (isStopped())
				return;

			const std::string id = game.GetAtt("siteareaid");
			DesuraId gameId(id.c_str(), "games");

			if (gameId.isOk())
				parseGameQuick(game);
		});
	}
}

void InstalledWizardThread::triggerProgress()
{
	MCFCore::Misc::ProgressInfo pi = MCFCore::Misc::ProgressInfo();

	if (m_uiTotal != 0)
	{
		pi.percent = (uint8)(m_uiDone*100/m_uiTotal)+1;
		onMcfProgressEvent(pi);
	}
}

void InstalledWizardThread::parseGame(DesuraId id, const XML::gcXMLElement &game, WildcardManager *pWildCard, const XML::gcXMLElement &info)
{
	pWildCard->updateInstallWildcard("INSTALL_PATH", "INSTALL_PATH");
	pWildCard->updateInstallWildcard("PARENT_INSTALL_PATH", "%INSTALL_PATH%");


	gcString name = game.GetChild("name");

	if (name == "" && info.IsValid())
		name = info.GetChild("name");
	
	if (name == "")
		return;

	if (m_bTriggerNewEvent)
		onNewItemEvent(name);

	triggerProgress();
	m_uiDone++;

	UserCore::Misc::InstallInfo temp(id);

	try
	{
		if (info.IsValid())
			temp.loadXmlData(info, pWildCard);

		temp.loadXmlData(game, pWildCard);
	}
	catch (gcException &except)
	{
		Warning("ItemWizard: Error in xml parsing (installed wizard, games): {0}\n", except);
		return;
	}
		
	if (m_bTriggerNewEvent)
		onGameFound(temp);
	else
		m_vGameList.push_back(temp);

	pWildCard->updateInstallWildcard("INSTALL_PATH", "INSTALL_PATH");
	pWildCard->updateInstallWildcard("PARENT_INSTALL_PATH", temp.getPath());

	std::map<uint64, XML::gcXMLElement> mModMap;

	info.FirstChildElement("mods").for_each_child("mods", [&mModMap, this](const XML::gcXMLElement &mod)
	{
		if (isStopped())
			return;

		const std::string szId = mod.GetAtt("siteareaid");
		DesuraId internId(szId.c_str(), "mods");

		if (internId.isOk())
			mModMap[internId.toInt64()] = mod;
	});

	game.FirstChildElement("mods").for_each_child("mods", [&](const XML::gcXMLElement &mod)
	{
		if (isStopped())
			return;

		const std::string szId = mod.GetAtt("siteareaid");
		DesuraId internId(szId.c_str(), "mods");

		if (internId.isOk())
			parseMod(id, internId, mod, pWildCard, mModMap[internId.toInt64()]);
	});
}

void InstalledWizardThread::parseMod(DesuraId parId, DesuraId id, const XML::gcXMLElement &mod, WildcardManager *pWildCard, const XML::gcXMLElement &info)
{
	gcString name = mod.GetChild("name");

	if (name == "" && info.IsValid())
		name = info.GetChild("name");
	
	if (name == "")
		return;

	if (m_bTriggerNewEvent)
		onNewItemEvent(name);

	triggerProgress();

	m_uiDone++;

	UserCore::Misc::InstallInfo temp(id, parId);

	try
	{
		temp.loadXmlData(mod, pWildCard);

		if (m_bTriggerNewEvent)
			onModFound(temp);
	}
	catch (gcException &except)
	{
		Warning("ItemWizard: Error in xml parsing (installed wizard, mods): {0}\n", except);
	}
}

void InstalledWizardThread::parseItems1(const XML::gcXMLElement &fNode, WildcardManager *pWildCard, std::map<uint64, XML::gcXMLElement> *vMap)
{
	gcAssert(pWildCard);

	if (!fNode.IsValid())
		return;

	fNode.FirstChildElement("games").for_each_child("game", [&](const XML::gcXMLElement &game)
	{
		if (isStopped())
			return;

		const std::string szId = game.GetAtt("siteareaid");
		DesuraId gameId(szId.c_str(), "games");

		if (!gameId.isOk())
			return;

		XML::gcXMLElement info;

		if (vMap)
			info = (*vMap)[gameId.toInt64()];

		parseGame(gameId, game, pWildCard, info);
	});
}

void InstalledWizardThread::parseItems2(const XML::gcXMLElement &fNode, WildcardManager *pWildCard)
{
	gcAssert(pWildCard);

	if (!fNode.IsValid())
		return;

	std::map<uint64, XML::gcXMLElement> vMap;

	fNode.FirstChildElement("games").for_each_child("game", [&](const XML::gcXMLElement &game)
	{
		if (isStopped())
			return;

		const std::string szId = game.GetAtt("siteareaid");
		DesuraId gameId(szId.c_str(), "games");

		if (gameId.isOk())
			vMap[gameId.toInt64()] = game;
	});

	fNode.FirstChildElement("platforms").for_each_child("platform", [&](const XML::gcXMLElement &platform)
	{
		if (isStopped())
			return;

		if (getUserCore()->platformFilter(platform, PlatformType::Item))
			return;

		WildcardManager wm(pWildCard);
		auto wildCardNode = platform.FirstChildElement("wcards");

		if (wildCardNode.IsValid())
		{
			wm.parseXML(wildCardNode);
			wm.compactWildCards();
		}

		parseItems1(platform, &wm, &vMap);
	});
}


void InstalledWizardThread::onItemFound(UserCore::Item::ItemInfoI *item)
{
	bool verify = (item->isInstalled() && item->isDownloadable() && !(item->getStatus()&UserCore::Item::ItemInfoI::STATUS_LINK));

	if (verify)
	{
		if (!m_pTaskGroup)
		{
			UserCore::ItemManager* im = dynamic_cast<UserCore::ItemManager*>(getUserCore()->getItemManager());
			gcAssert(im);
			m_pTaskGroup = im->newTaskGroup(UserCore::Item::ItemTaskGroupI::A_VERIFY);
			m_pTaskGroup->start();
		}

		item->addSFlag(UserCore::Item::ItemInfoI::STATUS_ONCOMPUTER|UserCore::Item::ItemInfoI::STATUS_VERIFING);

		UserCore::Item::ItemHandleI* handle = getUserCore()->getItemManager()->findItemHandle(item->getId());
		handle->setPauseOnError(true);

		m_pTaskGroup->addItem(item);
	}

	DesuraId id = item->getId();
	onItemFoundEvent(id);
}


}
}
