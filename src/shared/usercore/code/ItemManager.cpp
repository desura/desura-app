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
#include "ItemManager.h"
#include "usercore/ItemInfoI.h"
#include "ItemInfo.h"
#include "User.h"

#ifdef WIN32
#include "GameExplorerManager.h"
#endif
#include "ItemTaskGroup.h"

#include "sqlite3x.hpp"
#include "sql/ItemInfoSql.h"

#include "CIPManager.h"
#include "UserTasks.h"

namespace UM = UserCore::Item;




class CompairDesuraIds
{
public:
	bool operator()(const DesuraId a, const DesuraId b)
	{
		return (a.toInt64() < b.toInt64());
	}
};


typedef std::map<DesuraId, std::pair<XML::gcXMLElement, DesuraId>, CompairDesuraIds> InfoMap;

class InfoMaps
{
public:
	InfoMap gameMap;
	InfoMap modMap;
};

using namespace UserCore;


class ItemManager::ParseInfo
{
public:
	ParseInfo(uint32 statusOverride, WildcardManager* pWildCard = nullptr, bool reset=false, InfoMaps* maps=nullptr)
	{
		this->statusOverride = statusOverride;
		this->pWildCard = pWildCard;
		this->reset = reset;
		this->maps = maps;

		platform = -1;
	}

	XML::gcXMLElement rootNode;
	XML::gcXMLElement infoNode;

	WildcardManager* pWildCard;
	InfoMaps* maps;

	uint32 statusOverride = 0;
	uint32 platform;

	bool reset;
};

ItemManager::ItemManager(User* user) : BaseManager(true)
{
	m_pUser = user;
	m_bEnableSave = false;
	m_bFirstLogin = false;

	m_szAppPath = user->getAppDataPath();

	createItemInfoDbTables(m_szAppPath.c_str());
	gcString oldPath("{0}{1}iteminfo_c.sqlite", m_szAppPath, DIRS_STR);

	if (UTIL::FS::isValidFile(oldPath))
	{
		try
		{
			migrateOldItemInfo(oldPath.c_str(), getItemInfoDb(m_szAppPath.c_str()).c_str());
		}
		catch (std::exception &e)
		{
			Warning("Failed to migrate old Item Info DB: {0}", e.what());
		}

		UTIL::FS::delFile(oldPath);
	}

	updateItemIds();
	m_uiDelayCount = 0;
}

ItemManager::~ItemManager()
{
	saveDbItems(true);

	onUpdateEvent.reset();
	onFavoriteUpdateEvent.reset();
	onRecentUpdateEvent.reset();
}

void ItemManager::migrateOldItemInfo(const char* olddb, const char* newdb)
{
	sqlite3x::sqlite3_connection db(olddb);
	sqlite3x::sqlite3_connection ndb(newdb);

	trycatch(db, "ALTER TABLE branchinfo ADD installscript TEXT;");
	trycatch(db, "ALTER TABLE branchinfo ADD installscriptCRC INTEGER;");
	trycatch(db, "ALTER TABLE exe ADD rank INTEGER;");
	trycatch(db, "ALTER TABLE branchinfo ADD globalid INTEGER;");

	sqlite3x::sqlite3_transaction trans(ndb);




	{
		sqlite3x::sqlite3_command cmd(db, "SELECT * FROM recent;");
		sqlite3x::sqlite3_reader reader = cmd.executereader();

		while (reader.read())
		{
			sqlite3x::sqlite3_command cmd(ndb, "INSERT OR IGNORE INTO recent VALUES (?,?,?);");

			cmd.bind(1, reader.getstring(0));
			cmd.bind(2, reader.getstring(1));
			cmd.bind(3, reader.getstring(2));

			cmd.executenonquery();
		}
	}

	{
		sqlite3x::sqlite3_command cmd(db, "SELECT * FROM favorite;");
		sqlite3x::sqlite3_reader reader = cmd.executereader();

		while (reader.read())
		{
			sqlite3x::sqlite3_command cmd(ndb, "INSERT OR IGNORE INTO favorite VALUES (?,?);");

			cmd.bind(1, reader.getstring(0));
			cmd.bind(2, reader.getstring(1));

			cmd.executenonquery();
		}
	}

	{
		sqlite3x::sqlite3_command cmd(db, "SELECT * FROM newItems;");
		sqlite3x::sqlite3_reader reader = cmd.executereader();

		while (reader.read())
		{
			sqlite3x::sqlite3_command cmd(ndb, "INSERT OR IGNORE INTO newItems VALUES (?,?,?);");

			cmd.bind(1, reader.getstring(0));
			cmd.bind(2, reader.getstring(1));
			cmd.bind(3, reader.getstring(2));
			cmd.executenonquery();
		}
	}

	{
		sqlite3x::sqlite3_command cmd(db, "SELECT * FROM tools;");
		sqlite3x::sqlite3_reader reader = cmd.executereader();

		while (reader.read())
		{
			sqlite3x::sqlite3_command cmd(ndb, "INSERT OR IGNORE INTO tools VALUES (?,?);");

			cmd.bind(1, reader.getstring(0));
			cmd.bind(2, reader.getstring(1));

			cmd.executenonquery();
		}
	}


	{
		sqlite3x::sqlite3_command cmd(db, "SELECT internalid, parentid, percent, statusflags, rating, developer, name, shortname, profile, devprofile, icon, iconurl, logo, logourl,"
			"installpath, installcheck, iprimpath, imod, ibranch, ibuild, eula, lastbuild, lastbranch, publisher, pubprofile FROM iteminfo;");

		sqlite3x::sqlite3_reader reader = cmd.executereader();

		while (reader.read())
		{
			{
				sqlite3x::sqlite3_command cmd(ndb, "INSERT OR IGNORE INTO iteminfo VALUES (?,?,?,?,?, ?,?,?,?,?, ?,?,?,?,?, ?,?,?);");

				cmd.bind(1, reader.getstring(0));
				cmd.bind(2, reader.getstring(1));
				cmd.bind(3, reader.getstring(2));
				cmd.bind(4, reader.getstring(3));
				cmd.bind(5, reader.getstring(4));

				cmd.bind(6, reader.getstring(5));
				cmd.bind(7, reader.getstring(6));
				cmd.bind(8, reader.getstring(7));
				cmd.bind(9, reader.getstring(8));
				cmd.bind(10, reader.getstring(9));

				cmd.bind(11, reader.getstring(10));
				cmd.bind(12, reader.getstring(11));
				cmd.bind(13, reader.getstring(12));
				cmd.bind(14, reader.getstring(13));

				cmd.bind(15, reader.getstring(23));
				cmd.bind(16, reader.getstring(24));

				cmd.bind(17, reader.getstring(18));
				cmd.bind(18, reader.getstring(22));

				cmd.executenonquery();
			}

			{
				sqlite3x::sqlite3_command cmd(ndb, "INSERT OR IGNORE INTO installinfo VALUES (?,?,?,?,?, ?,?,?);");

				cmd.bind(1, reader.getstring(0));
				cmd.bind(2, BUILDID_PUBLIC);

				cmd.bind(3, reader.getstring(14));
				cmd.bind(4, reader.getstring(15));
				cmd.bind(5, reader.getstring(16));

				cmd.bind(6, reader.getstring(17));
				cmd.bind(7, reader.getstring(19));
				cmd.bind(8, reader.getstring(21));

				cmd.executenonquery();
			}
		}
	}

	{
		sqlite3x::sqlite3_command cmd(db, "SELECT * FROM branchinfo;");
		sqlite3x::sqlite3_reader reader = cmd.executereader();trycatch(db, "ALTER TABLE branchinfo ADD installscriptCRC INTEGER;");

		while (reader.read())
		{
			sqlite3x::sqlite3_command cmd(ndb, "INSERT OR IGNORE INTO branchinfo VALUES (?,?,?,?,?, ?,?,?,?,?, ?,?);");

			cmd.bind(1, reader.getstring(0));
			cmd.bind(2, reader.getstring(1));
			cmd.bind(3, reader.getstring(2));
			cmd.bind(4, reader.getstring(3));
			cmd.bind(5, reader.getstring(4));
			cmd.bind(6, reader.getstring(5));
			cmd.bind(7, reader.getstring(6));
			cmd.bind(8, reader.getstring(7));
			cmd.bind(9, reader.getstring(8));
			cmd.bind(10, reader.getstring(9));
			cmd.bind(11, reader.getstring(10));
			cmd.bind(12, BUILDID_PUBLIC);

			cmd.executenonquery();
		}
	}

	{
		sqlite3x::sqlite3_command cmd(db, "SELECT * FROM exe;");
		sqlite3x::sqlite3_reader reader = cmd.executereader();

		while (reader.read())
		{
			sqlite3x::sqlite3_command cmd(ndb, "INSERT OR IGNORE INTO exe VALUES (?,?,?,?,?, ?,?);");

			cmd.bind(1, reader.getstring(0));
			cmd.bind(2, BUILDID_PUBLIC);
			cmd.bind(3, reader.getstring(1));
			cmd.bind(4, reader.getstring(2));
			cmd.bind(5, reader.getstring(3));
			cmd.bind(6, reader.getstring(4));
			cmd.bind(7, reader.getstring(5));

			cmd.executenonquery();
		}
	}

	trans.commit();
}

void ItemManager::migrateStandaloneFiles()
{
	gcString path = UTIL::OS::getAppDataPath(L"standalone");

	if (!UTIL::FS::isValidFolder(path))
		return;

	std::vector<UTIL::FS::Path> outList;

	std::vector<std::string> filter;
	filter.push_back("xml");

	UTIL::FS::getAllFiles(UTIL::FS::Path(path, "", false), outList, &filter);

	if (outList.size() > 0)
		m_pUser->getThreadPool()->forceTask(new UserCore::Task::MigrateStandaloneTask(m_pUser, outList));
}

void ItemManager::updateItemIds()
{
}

bool ItemManager::isInstalled(DesuraId id)
{
	UserCore::Item::ItemInfo* temp = findItemInfoNorm(id);
	
	if (temp)
		return temp->isInstalled();

	return false;
}

void ItemManager::getCIP(DesuraId id, char** buff)
{
	UserCore::Misc::CIPItem cip;
	cip.id = id;

	if (m_pUser->getCIPManager()->getCIP(cip))
		Safe::strcpy(buff, cip.path.c_str(), cip.path.length());
}

uint32 ItemManager::getDevItemCount()
{
	std::vector<UserCore::Item::ItemInfoI*> dList;
	getDevList(dList);
	return dList.size();
}

void ItemManager::getAllItems(std::vector<UserCore::Item::ItemInfoI*> &aList)
{
	itemIterator it;
	itemIterator endit = m_mItemMap.end();

	for (it = m_mItemMap.begin(); it != endit; ++it)
	{
		UserCore::Item::ItemInfo* info = it->second->getItemInfoNorm();
		
		if (!info)
			continue;
		
		aList.push_back(info);
	}
}

void ItemManager::getGameList(std::vector<UserCore::Item::ItemInfoI*> &gList, bool includeDeleted)
{
	itemIterator it;
	itemIterator endit = m_mItemMap.end();

	for (it = m_mItemMap.begin(); it != endit; ++it)
	{
		UserCore::Item::ItemInfo* info = it->second->getItemInfoNorm();

		if (!info)
			continue;

		if (info->getId().getType() != DesuraId::TYPE_GAME)
			continue;

		if (!includeDeleted && (info->getStatus() & UM::ItemInfoI::STATUS_DELETED))
			continue;

		if (HasAnyFlags(info->getStatus(), UM::ItemInfoI::STATUS_STUB))
			continue;

		//If this is Diablo 2 (50) and we have LOD (13824) install, dont return d2
		if (info->getId().getItem() == 50 && m_mItemMap.find(DesuraId("13824", "games").toInt64()) != m_mItemMap.end())
			continue;

		gList.push_back(info);
	}

}

void ItemManager::getModList(DesuraId gameId, std::vector<UserCore::Item::ItemInfoI*> &mList, bool includeDeleted)
{
	for_each([&mList, includeDeleted, gameId](UserCore::Item::ItemHandleI* handle)
	{
		UserCore::Item::ItemInfoI* info = handle->getItemInfo();

		if (info->getId().getType() != DesuraId::TYPE_MOD || info->getParentId() != gameId)
			return;		

		if (!includeDeleted && (info->getStatus() & UM::ItemInfoI::STATUS_DELETED))
			return;

		if (HasAnyFlags(info->getStatus(), UM::ItemInfoI::STATUS_STUB))
			return;

		mList.push_back(info);
	});

	//if this is lod (13824) show d2 mods as well
	if (gameId.getItem() == 13824 && m_mItemMap.find(DesuraId("50", "games").toInt64()) != m_mItemMap.end())
		getModList(DesuraId("50", "games"), mList, includeDeleted);
}

void ItemManager::getDevList(std::vector<UserCore::Item::ItemInfoI*> &dList)
{
	itemIterator it;
	itemIterator endit = m_mItemMap.end();

	for (it = m_mItemMap.begin(); it != endit; ++it)
	{
		UserCore::Item::ItemInfo* info = it->second->getItemInfoNorm();

		if (!info)
			continue;

		if (info->getId().getType() == DesuraId::TYPE_TOOL)
			continue;

		if (info->getStatus() & (UM::ItemInfoI::STATUS_DEVELOPER) && !(info->getStatus() & (UM::ItemInfoI::STATUS_DELETED)))
			dList.push_back(info);
	}
}


void ItemManager::getLinkList(std::vector<UserCore::Item::ItemInfoI*> &lList)
{
	itemIterator it;
	itemIterator endit = m_mItemMap.end();

	for (it = m_mItemMap.begin(); it != endit; ++it)
	{
		UserCore::Item::ItemInfo* info = it->second->getItemInfoNorm();

		if (!info)
			continue;

		if (info->getId().getType() != DesuraId::TYPE_LINK)
			continue;

		if (info->getStatus() & UM::ItemInfoI::STATUS_DELETED)
			continue;

		lList.push_back(info);
	}
}


bool ItemManager::isItemFavorite(DesuraId id)
{
	bool res = false;

	m_FavLock.lock();

	for (size_t x=0; x<m_vFavList.size(); x++)
	{
		if (m_vFavList[x] == id)
		{
			res = true;
			break;
		}
	}

	m_FavLock.unlock();

	return res;
}

void ItemManager::getFavList(std::vector<UserCore::Item::ItemInfoI*> &fList)
{
	m_FavLock.lock();
	for (size_t x=0; x<m_vFavList.size(); x++)
	{
		UserCore::Item::ItemInfoI* item = findItemInfo(m_vFavList[x]);

		if (item)
			fList.push_back(item);
	}
	m_FavLock.unlock();
}

void ItemManager::loadFavList()
{
	m_FavLock.lock();

	m_vFavList.clear();
	sqlite3x::sqlite3_connection db(getItemInfoDb(m_szAppPath.c_str()).c_str());

	try
	{
		sqlite3x::sqlite3_command cmd(db, "SELECT internalid FROM favorite WHERE userid=?;");
		cmd.bind(1, (int)m_pUser->getUserId());

		sqlite3x::sqlite3_reader reader = cmd.executereader();

		while (reader.read())
			m_vFavList.push_back(DesuraId(reader.getint64(0)));
	}
	catch (std::exception &)
	{
	}

	m_FavLock.unlock();
}

void ItemManager::getRecentList(std::vector<UserCore::Item::ItemInfoI*> &rList)
{
	sqlite3x::sqlite3_connection db(getItemInfoDb(m_szAppPath.c_str()).c_str());

	try
	{
		sqlite3x::sqlite3_command cmd(db, "SELECT internalid FROM recent WHERE userid=? ORDER BY time DESC;");
		cmd.bind(1, (int)m_pUser->getUserId());

		sqlite3x::sqlite3_reader reader = cmd.executereader();

		while (reader.read())
		{
			DesuraId id(reader.getint64(0));

			UserCore::Item::ItemInfoI* item = findItemInfo(id);
			if (item)
				rList.push_back(item);
		}
	}
	catch (std::exception &)
	{
	}
}

void ItemManager::getNewItems(std::vector<UserCore::Item::ItemInfoI*> &rList)
{
	sqlite3x::sqlite3_connection db(getItemInfoDb(m_szAppPath.c_str()).c_str());

	try
	{
		sqlite3x::sqlite3_command cmd(db, "SELECT internalid FROM newItems WHERE userid=? AND time > datetime('now', '-5 day');");
		cmd.bind(1, (int)m_pUser->getUserId());

		sqlite3x::sqlite3_reader reader = cmd.executereader();

		while (reader.read())
		{
			DesuraId id(reader.getint64(0));

			UserCore::Item::ItemInfoI* item = findItemInfo(id);
			if (item)
				rList.push_back(item);
		}
	}
	catch (std::exception)
	{
	}
}


void ItemManager::removeItem(DesuraId id)
{
	gcTrace("ItemId {0}", id);
	UserCore::Item::ItemInfo* item = findItemInfoNorm(id);

	if (!item)
		return;

	if (item->getStatus() & (UM::ItemInfoI::STATUS_INSTALLED|UM::ItemInfoI::STATUS_ONACCOUNT))
		return;

	std::vector<UserCore::Item::ItemInfoI*> mList;
	getModList(id, mList);

	//make sure we have no children
	if (mList.size() > 0)
		return;

	//we add a flag instead of deleting the item to save headaches arising from other areas 
	//caching the pointer to this item. Thus they can check the flag before doing work
	item->softDelete();

#ifdef WIN32
	m_pUser->getGameExplorerManager()->removeItem(id);
#endif

	uint32 num = 0;
	m_pUser->onItemsAddedEvent(num);
	DesuraId currentID(item->getId());
	onNewItem(currentID);
}


void ItemManager::retrieveItemInfoAsync(DesuraId id, bool addToAccount)
{
	gcTrace("ItemId {0}", id);
	m_pUser->m_pThreadPool->queueTask(new UserCore::Task::GatherInfoTask(m_pUser, id, addToAccount));
}

void ItemManager::retrieveItemInfo(DesuraId id, uint32 statusOveride, WildcardManager* pWildCard, MCFBranch mcfBranch, MCFBuild mcfBuild, bool reset)
{
	gcTrace("ItemId {0}", id);

	gcAssert(m_pUser->m_pWebCore);

	XML::gcXMLDocument doc;
	m_pUser->m_pWebCore->getItemInfo(id, doc, mcfBranch, mcfBuild);

	auto uNode = doc.GetRoot("iteminfo");
	uint32 ver = doc.ProcessStatus("iteminfo");

	if (!uNode.IsValid())
		throw gcException(ERR_BADXML);

	auto wcNode = uNode.FirstChildElement("wcards");
	auto gamesNode = uNode.FirstChildElement("games");

	//make sure atoi worked
	if (ver == 0)
		ver = 1;

	if (ver== 1 && !gamesNode.IsValid())
		throw gcException(ERR_BADXML);

	if (pWildCard)
	{
		pWildCard->onNeedSpecialEvent += delegate(&m_pUser->onNeedWildCardEvent);

		if (wcNode.IsValid())
			pWildCard->parseXML(wcNode);
	}

	m_pUser->getToolManager()->initJSEngine();

	if (ver == 1)
	{
		ParseInfo pi(statusOveride, pWildCard, reset);
		pi.rootNode = gamesNode;

		parseGamesXml(pi);

		m_pUser->getToolManager()->parseXml(uNode.FirstChildElement("toolinfo"));
	}
	else
	{
		InfoMaps maps;
		generateInfoMaps(gamesNode, &maps);

		ParseInfo pi(statusOveride, pWildCard, reset, &maps);

		uNode.FirstChildElement("platforms").for_each_child("platform", [&](const XML::gcXMLElement &platform)
		{
			if (!m_pUser->platformFilter(platform, PlatformType::Tool))
				m_pUser->getToolManager()->parseXml(platform.FirstChildElement("toolinfo"));

			platform.GetAtt("id", pi.platform);
			parseKnownBranches(platform.FirstChildElement("games"));

			if (m_pUser->platformFilter(platform, PlatformType::Item))
				return;

			pi.rootNode = platform.FirstChildElement("games");

			if (!pWildCard)
			{
				parseGamesXml(pi);
			}
			else
			{
				WildcardManager wc(pWildCard);
				const XML::gcXMLElement &wcNode = platform.FirstChildElement("wcards");

				if (wcNode.IsValid())
					wc.parseXML(wcNode);

				pi.pWildCard = &wc;
				parseGamesXml(pi);
				pi.pWildCard = pWildCard;
			}
		});
		
		processLeftOvers(maps, true);
	}

	m_pUser->getToolManager()->destroyJSEngine();

	if (pWildCard)
		pWildCard->onNeedSpecialEvent -= delegate(&m_pUser->onNeedWildCardEvent);

	saveItems();
}

void ItemManager::processLeftOvers(InfoMaps &maps, bool addMissing)
{
	//if we have left over games we didnt parse add them as deleted items if they dont exist
	for (auto p : maps.gameMap)
	{
		DesuraId pid = p.second.second;
		DesuraId id = p.first;

		auto infoNode = p.second.first;

		UserCore::Item::ItemInfo* info = this->findItemInfoNorm(id);
		ParseInfo pi(UM::ItemInfo::STATUS_STUB);

		if (info)
			return;

		auto isDev = [](const XML::gcXMLElement &infoNode) -> bool
		{
			bool isDev = false;
			return (infoNode.GetChild("devadmin", isDev) && isDev);
		};
		
		bool isDevOfGame = isDev(infoNode);

		if (!addMissing && !isDevOfGame)
			return;
		
		pi.rootNode = infoNode;
		info = createNewItem(pid, id, pi);

		bool isDevOfMod = false;
		bool addMissingChild = addMissing;
		ItemManager* im = this;

		for(auto p : maps.modMap)
		{
			DesuraId gid = p.second.second;
			const XML::gcXMLElement &infoNode = p.second.first;
			
			if (id != gid)
				return;

			DesuraId mid = p.first;
			UserCore::Item::ItemInfo* info = im->findItemInfoNorm(mid);

			if (info)
				return;

			bool dev = isDev(infoNode);

			if (!addMissingChild || !dev)
				return;

			pi.rootNode = infoNode;
			info = im->createNewItem(gid, mid, pi);

			if (dev)
			{
				info->delSFlag(UM::ItemInfo::STATUS_STUB);
				isDevOfMod = true;
			}
		}

		if (isDevOfMod || isDevOfGame)
			info->delSFlag(UM::ItemInfo::STATUS_STUB);
	}
}

void ItemManager::loadItems()
{
	loadDbItems();
}

void ItemManager::loadDbItems()
{
	migrateStandaloneFiles();
	gcString szItemDb = getItemInfoDb(m_szAppPath.c_str());

	try
	{
		sqlite3x::sqlite3_connection db(szItemDb.c_str());

		sqlite3x::sqlite3_command cmd(db, "SELECT count(*) FROM newItems WHERE userid=?;");
		cmd.bind(1, (int)m_pUser->getUserId());

		m_bFirstLogin = (cmd.executeint() == 0);
	}
	catch (std::exception)
	{
	}


	try
	{
		sqlite3x::sqlite3_connection db(szItemDb.c_str());

		if (db.executeint("select count(*) from sqlite_master where name='iteminfo';") != 0)
		{
			uint32 count = 0;

			sqlite3x::sqlite3_command cmd(db, "SELECT internalid, parentid FROM iteminfo;");
			sqlite3x::sqlite3_reader reader = cmd.executereader();

			while(reader.read()) 
			{
				DesuraId id(reader.getint64(0));
				DesuraId pid(reader.getint64(1));

				UserCore::Item::ItemInfo* temp = new UserCore::Item::ItemInfo(m_pUser, id, pid);
				temp->loadDb(&db); //UM::ItemInfoI::STATUS_ONCOMPUTER

				UserCore::Item::ItemHandle* handle = new UserCore::Item::ItemHandle(temp, m_pUser);

				addItem(id.toInt64(), handle);
				count++;
			}

			if (count)
				m_pUser->onItemsAddedEvent(count);
		}

	}
	catch (std::exception &e)
	{
		Warning("Failed to load items from db: {0}\n", e.what());
	}

	loadFavList();
}

void ItemManager::saveItems()
{
	if (!m_bEnableSave)
		return;

	saveDbItems(false);
}

void ItemManager::saveDbItems(bool fullSave)
{
	gcString szItemDb = getItemInfoDb(m_szAppPath.c_str());

	try
	{
		sqlite3x::sqlite3_connection db(szItemDb.c_str());
		sqlite3x::sqlite3_transaction trans(db);
	
		for_each([&db](UserCore::Item::ItemHandle* handle){

			if (handle && handle->getItemInfoNorm())
				handle->getItemInfoNorm()->saveDbFull(&db);

		});

		trans.commit();
	}
	catch (std::exception &e)
	{
		Warning("Failed to save items to db: {0}\n", e.what());
	}
}

void ItemManager::itemsNeedUpdate(const XML::gcXMLElement &itemsNode)
{
	if (!itemsNode.IsValid())
		return;

	m_pUser->getToolManager()->initJSEngine();

	parseItemUpdateXml("mod", itemsNode);
	parseItemUpdateXml("game", itemsNode);

	m_pUser->getToolManager()->destroyJSEngine();

	onUpdateEvent();
}

void ItemManager::itemsNeedUpdate2(const XML::gcXMLElement &platformsNode)
{
	if (!platformsNode.IsValid())
		return;

	m_pUser->getToolManager()->initJSEngine();

	platformsNode.for_each_child("platform", [this](const XML::gcXMLElement &platform)
	{
		if (!m_pUser->platformFilter(platform, PlatformType::Item))
		{
			parseItemUpdateXml("mod", platform);
			parseItemUpdateXml("game", platform);
		}

		parseKnownBranches(platform.FirstChildElement("games"));
	});

	m_pUser->getToolManager()->destroyJSEngine();
	onUpdateEvent();
}

void ItemManager::parseItemUpdateXml(const char* area, const XML::gcXMLElement &itemsNode)
{
	gcString rootArea = gcString(area) + "s";

	auto modNode = itemsNode.FirstChildElement(rootArea.c_str());
	if (!modNode.IsValid())
		return;

	modNode.for_each_child(area, [&](const XML::gcXMLElement &itemNode)
	{
		const std::string szId = itemNode.GetAtt("siteareaid");
		DesuraId id(szId.c_str(), rootArea.c_str());

		if (!id.isOk())
			return;

		UserCore::Item::ItemInfo* item = findItemInfoNorm(id);
				
		if (!item)
		{
			retrieveItemInfoAsync(id, true);
			return;
		}

		if (item->isDeleted())
		{
			DesuraId currentID = item->getId();
						
			item->delSFlag(UserCore::Item::ItemInfoI::STATUS_DELETED);
			item->addSFlag(UserCore::Item::ItemInfoI::STATUS_ONACCOUNT);
			onNewItem(currentID);
		}

		item->processUpdateXml(itemNode);
		m_pUser->getToolManager()->findJSTools(item);
	});
}

void ItemManager::postParseLoginXml()
{
	for_each([this](UserCore::Item::ItemHandle* handle){

		UserCore::Item::ItemInfo* info = dynamic_cast<UserCore::Item::ItemInfo*>(handle->getItemInfo());

		if (info->wasOnAccount())
			removeItem(info->getId());
		else
			m_pUser->getToolManager()->findJSTools(handle->getItemInfoNorm());
	});

	m_bFirstLogin = false;
}

void ItemManager::generateInfoMaps(const XML::gcXMLElement &gamesNode, InfoMaps* maps)
{
	gamesNode.for_each_child("game", [&maps, this](const XML::gcXMLElement &game)
	{
		InfoMaps* pMaps = maps;

		const std::string szId = game.GetAtt("siteareaid");
	
		if (szId.empty())
			return;

		DesuraId pid = getParentId(game, game);
		DesuraId gid(szId.c_str(), "games");
		
		maps->gameMap[gid] = std::pair<XML::gcXMLElement, DesuraId>(game, pid);

		game.FirstChildElement("mods").for_each_child("mod", [&pMaps, gid](const XML::gcXMLElement &mod)
		{
			const std::string id = mod.GetAtt("siteareaid");
	
			if (id.empty())
				return;

			DesuraId mid(id.c_str(), "mods");
			pMaps->modMap[mid] = std::pair<XML::gcXMLElement, DesuraId>(mod, gid);
		});
	});
}

UserCore::Item::ItemInfo* ItemManager::createNewItem(DesuraId pid, DesuraId id, ParseInfo& pi)
{
	gcTrace("ItemId {0}", id);
	UserCore::Item::ItemInfo* temp = new UserCore::Item::ItemInfo(m_pUser, id, pid);
	UserCore::Item::ItemHandle* handle = new UserCore::Item::ItemHandle(temp, m_pUser);

	try
	{
		if (pi.infoNode.IsValid())
			temp->loadXmlData(pi.platform, pi.infoNode, pi.statusOverride, pi.pWildCard, pi.reset);

		temp->loadXmlData(pi.platform, pi.rootNode, pi.statusOverride, pi.pWildCard, pi.reset);
		m_pUser->getToolManager()->findJSTools(temp);

		addItem(id.toInt64(), handle);

		if (!temp->isDeleted())
			onNewItem(id);
	}
	catch (gcException &except)
	{
		Warning("Parse XML failed on item with error: {0}\n", except);
		safe_delete(temp);
	}

	return temp;
}

void ItemManager::updateItem(UserCore::Item::ItemInfo* itemInfo, ParseInfo& pi)
{
	uint32 newSO = pi.statusOverride&~(UM::ItemInfoI::STATUS_DELETED);

	if (pi.infoNode.IsValid())
		itemInfo->loadXmlData(pi.platform, pi.infoNode, newSO, pi.pWildCard, pi.reset);

	itemInfo->loadXmlData(pi.platform, pi.rootNode, newSO, pi.pWildCard, pi.reset);
	itemInfo->processUpdateXml(pi.rootNode);

	itemInfo->delSFlag(UM::ItemInfoI::STATUS_STUB|UM::ItemInfoI::STATUS_DELETED);

	m_pUser->getToolManager()->findJSTools(itemInfo);
}

void ItemManager::parseLoginXml(const XML::gcXMLElement &gameNode, const XML::gcXMLElement &devNode)
{
	m_pUser->getToolManager()->initJSEngine();

	ParseInfo pi(UM::ItemInfoI::STATUS_ONACCOUNT);

	if (gameNode.IsValid())
	{
		ParseInfo pi(UM::ItemInfoI::STATUS_ONACCOUNT);
		pi.rootNode = gameNode;

		parseGamesXml(pi);
	}

	if (devNode.IsValid())
	{
		ParseInfo pi(UM::ItemInfoI::STATUS_DEVELOPER);
		pi.rootNode = devNode.FirstChildElement("games");

		parseGamesXml(pi);
	}

	postParseLoginXml();
	m_pUser->getToolManager()->destroyJSEngine();
}

void ItemManager::parseLoginXml2(const XML::gcXMLElement &gamesNode, const XML::gcXMLElement &platformNodes)
{
	m_pUser->getToolManager()->initJSEngine();
	
	InfoMaps maps;
	generateInfoMaps(gamesNode, &maps);

	ParseInfo pi(UM::ItemInfoI::STATUS_ONACCOUNT, 0, false, &maps);

	platformNodes.for_each_child("platform", [this, &pi](const XML::gcXMLElement &platform)
	{
		platform.GetAtt("id", pi.platform);
		pi.rootNode = platform.FirstChildElement("games");

		if (!m_pUser->platformFilter(platform, PlatformType::Item))
			parseGamesXml(pi);

		parseKnownBranches(platform.FirstChildElement("games"));
	});

	processLeftOvers(maps, false);

	postParseLoginXml();
	m_pUser->getToolManager()->destroyJSEngine();
}

void ItemManager::parseGamesXml(ParseInfo& pi)
{
	if (!pi.rootNode.IsValid())
		return;

	uint32 beforeCount = BaseManager::getCount();

	ParseInfo gamePi(pi);

	pi.rootNode.for_each_child("game", [&](const XML::gcXMLElement &game)
	{
		const std::string id = game.GetAtt("siteareaid");
	
		if (id.empty())
			return;

		DesuraId pid = getParentId(game, pi.infoNode);
		DesuraId gid(id.c_str(), "games");

		XML::gcXMLElement infoNode;

		if (pi.maps)
		{
			InfoMap::iterator it = pi.maps->gameMap.find(gid);

			if (it != pi.maps->gameMap.end())
			{
				infoNode = it->second.first;
				pi.maps->gameMap.erase(it);
			}
		}

		gamePi.rootNode = game;
		gamePi.infoNode = infoNode;

		parseGameXml(gid, gamePi);
	});

	uint32 afterCount = BaseManager::getCount() - beforeCount;

	if (afterCount > 0)
	{
		m_pUser->onItemsAddedEvent(afterCount);
		onUpdateEvent();
	}
}

DesuraId ItemManager::getParentId(const XML::gcXMLElement &gameNode, const XML::gcXMLElement &infoNode)
{
	gcString expansion;
	gameNode.GetChild("expansion", expansion);

	if (expansion.size() == 0 || expansion == "0")
		infoNode.GetChild("expansion", expansion);

	DesuraId parentId;

	if (expansion.size() > 0 && expansion != "0")
		parentId = DesuraId(expansion.c_str(), "games");

	if (parentId.isOk())
		return parentId;

	return DesuraId();
}

void ItemManager::parseGameXml(DesuraId id, ParseInfo &pi)
{
	if (pi.pWildCard)
	{
		//reset local wildcards
		pi.pWildCard->updateInstallWildcard("INSTALL_PATH", "INSTALL_PATH");
		
		//map parent install path to normal install path incase we int the install from a child
		pi.pWildCard->updateInstallWildcard("PARENT_INSTALL_PATH", "%INSTALL_PATH%");
	}

	DesuraId pid = getParentId(pi.rootNode, pi.infoNode);

	if (pid.isOk() && pi.pWildCard)
	{
		UserCore::Item::ItemInfo *par = findItemInfoNorm(pid);
		if (par)
			pi.pWildCard->updateInstallWildcard("PARENT_INSTALL_PATH", par->getPath());
	}

	UserCore::Item::ItemInfo* temp = findItemInfoNorm(id);
			
	if (temp)
	{
		if (pid.isOk() && temp->getParentId() != pid)
			temp->setParentId(pid);

		updateItem(temp, pi);
	}
	else
	{
		temp = createNewItem(pid, id, pi);

		if (!temp)
			return;
	}

	ParseInfo gamePi(pi);

	gamePi.rootNode = pi.rootNode.FirstChildElement("mods");
	gamePi.infoNode = nullptr;

	parseModsXml(temp, gamePi);
}

void ItemManager::parseModsXml(UserCore::Item::ItemInfo* parent, ParseInfo &pi)
{
	if (!pi.rootNode.IsValid() || !parent)
		return;

	ParseInfo modPi(pi);

	pi.rootNode.for_each_child("mod", [&](const XML::gcXMLElement &mod)
	{
		const std::string id = mod.GetAtt("siteareaid");
		DesuraId internId(id.c_str(), "mods");

		if (!internId.isOk())
			return;

		XML::gcXMLElement infoNode;

		if (pi.maps)
		{
			InfoMap::iterator it = pi.maps->modMap.find(internId);

			if (it != pi.maps->modMap.end())
			{
				infoNode = it->second.first;
				pi.maps->modMap.erase(it);
			}			
		}

		modPi.rootNode = mod;
		modPi.infoNode = infoNode;
		
		parseModXml(parent, internId, modPi);
	});
}

void ItemManager::parseModXml(UserCore::Item::ItemInfo* parent, DesuraId id, ParseInfo &pi)
{
	if (pi.pWildCard)
	{
		//reset local wildcards
		pi.pWildCard->updateInstallWildcard("INSTALL_PATH", "INSTALL_PATH");
		pi.pWildCard->updateInstallWildcard("PARENT_INSTALL_PATH", parent->getPath());
	}

	//check to see if it all ready exists.
	UserCore::Item::ItemInfo * temp = findItemInfoNorm(id);

	if (temp)
		updateItem(temp, pi);
	else
		createNewItem(parent->getId(), id, pi);
}

void ItemManager::setFavorite(DesuraId id, bool fav)
{
	gcTrace("ItemId {0}, Fav {1}", id, fav);
	sqlite3x::sqlite3_connection db(getItemInfoDb(m_szAppPath.c_str()).c_str());

	try
	{
		gcString szCmd;

		if (fav)
			szCmd = gcString("INSERT INTO favorite VALUES (?,?);");
		else
			szCmd = gcString("DELETE FROM favorite WHERE internalid=? AND userid=?;");

		sqlite3x::sqlite3_command cmd(db, szCmd.c_str());
		cmd.bind(1, (long long int)id.toInt64());
		cmd.bind(2, (int)m_pUser->getUserId());
		
		cmd.executenonquery(); 
	}
	catch (std::exception)
	{
	}

	m_FavLock.lock();

	if (fav)
	{
		m_vFavList.push_back(id);
	}
	else
	{
		for (size_t x=0; x<m_vFavList.size(); x++)
		{
			if (m_vFavList[x] == id)
			{
				m_vFavList.erase(m_vFavList.begin()+x);
				break;
			}
		}
	}

	m_FavLock.unlock();

	onFavoriteUpdateEvent(id);
}

void ItemManager::setNew(DesuraId &id)
{
	sqlite3x::sqlite3_connection db(getItemInfoDb(m_szAppPath.c_str()).c_str());

	try
	{
		const char* cmdSql = "INSERT INTO newItems VALUES (?,?, datetime('now'));";

		if (m_bFirstLogin)
			cmdSql = "INSERT INTO newItems VALUES (?,?, datetime('now', '-10 day'));";

		sqlite3x::sqlite3_command cmd(db, cmdSql);
		cmd.bind(1, (long long int)id.toInt64());
		cmd.bind(2, (int)m_pUser->getUserId());
		cmd.executenonquery();
	}
	catch (std::exception &)
	{
	}
}

void ItemManager::setRecent(DesuraId id)
{
	sqlite3x::sqlite3_connection db(getItemInfoDb(m_szAppPath.c_str()).c_str());

	try
	{
		sqlite3x::sqlite3_command cmd(db, "SELECT count(*) FROM recent WHERE internalid=? AND userid=? ;");
		cmd.bind(1, (long long int)id.toInt64());
		cmd.bind(2, (int)m_pUser->getUserId());

		if (cmd.executeint() == 1)
		{
			sqlite3x::sqlite3_command cmd(db, "UPDATE recent SET time=datetime('now') WHERE internalid=? AND userid=? ;");
			cmd.bind(1, (long long int)id.toInt64());
			cmd.bind(2, (int)m_pUser->getUserId());
			cmd.executenonquery();
		}
		else
		{
			sqlite3x::sqlite3_command cmd(db, "SELECT count(*) FROM recent WHERE userid=?;");
			cmd.bind(1, (int)m_pUser->getUserId());

			if (cmd.executeint() >= 5)
			{
				sqlite3x::sqlite3_command cmd(db, "SELECT internalid, time FROM recent WHERE userid=? ORDER BY time ACS LIMIT 1;");
				cmd.bind(1, (int)m_pUser->getUserId());

				sqlite3x::sqlite3_reader reader = cmd.executereader();
				reader.read();

				DesuraId id(reader.getint64(0));
				std::string time = reader.getstring(1);

				{
					sqlite3x::sqlite3_command cmd(db, "DELETE FROM recent WHERE internalid=? AND userid=? AND time=?;");
					cmd.bind(1, (long long int)id.toInt64());
					cmd.bind(2, (int)m_pUser->getUserId());
					cmd.bind(3, time);
					cmd.executenonquery();
				}
			}

			{
				sqlite3x::sqlite3_command cmd(db, "INSERT INTO recent VALUES (?,?, datetime('now'));");
				cmd.bind(1, (long long int)id.toInt64());
				cmd.bind(2, (int)m_pUser->getUserId());
				cmd.executenonquery();
			}
		}
	}
	catch (std::exception &)
	{
	}

	onRecentUpdateEvent(id);
}





UserCore::Item::ItemInfo* ItemManager::findItemInfoNorm(DesuraId id)
{
	UserCore::Item::ItemHandle* handle = findItemHandleNorm(id);

	if (handle)
		return handle->getItemInfoNorm();

	return nullptr;
}

UserCore::Item::ItemInfoI* ItemManager::findItemInfo(DesuraId id)
{
	return findItemInfoNorm(id);
}

UserCore::Item::ItemHandle* ItemManager::findItemHandleNorm(DesuraId id)
{
	return BaseManager::findItem(id.toInt64());
}

UserCore::Item::ItemHandleI* ItemManager::findItemHandle(DesuraId id)
{
	return findItemHandleNorm(id);
}



void ItemManager::setInstalledMod(DesuraId parentId, DesuraId modId)
{
	UserCore::Item::ItemInfo* parent = findItemInfoNorm(parentId);

	if (!parent)
		return;

#ifdef DESURA_DIABLO2_HACK
	DesuraId d2Id(50, DesuraId::TYPE_GAME);
	DesuraId lodId(13824, DesuraId::TYPE_GAME);

	if (parentId == d2Id || parentId == lodId)
	{
		UserCore::Item::ItemInfo* d2 = findItemInfoNorm(d2Id);
		UserCore::Item::ItemInfo* lod = findItemInfoNorm(lodId);

		if (d2)
			d2->setInstalledModId(modId);

		if (lod)
			lod->setInstalledModId(modId);

		return;
	}
#endif

	parent->setInstalledModId(modId);
}


UserCore::Item::ItemTaskGroup* ItemManager::newTaskGroup(uint32 type)
{
	UserCore::Item::ItemTaskGroup* g = new UserCore::Item::ItemTaskGroup(this, (UserCore::Item::ItemTaskGroupI::ACTION)type, 1);
	return g;
}

void ItemManager::checkItems()
{
	UserCore::Item::ItemTaskGroup* group = nullptr;


	uint32 count = getCount();

	for (uint32 x=0; x<count; x++)
	{
		UserCore::Item::ItemHandleI* item = getItemHandle(x);

		if (!item || item->startUpCheck() == false)
			continue;

		if (!group)
			group = newTaskGroup(UserCore::Item::ItemTaskGroupI::A_VERIFY);

		group->addItem(item->getItemInfo());
	}	

	if (!group)
		return;

	group->sort([](UserCore::Item::ItemHandleI* a, UserCore::Item::ItemHandleI* b) -> bool {

		bool ap = HasAnyFlags(a->getItemInfo()->getStatus(), (UserCore::Item::ItemInfoI::STATUS_DOWNLOADING|UserCore::Item::ItemInfoI::STATUS_INSTALLING));
		bool bp = HasAnyFlags(b->getItemInfo()->getStatus(), (UserCore::Item::ItemInfoI::STATUS_DOWNLOADING|UserCore::Item::ItemInfoI::STATUS_INSTALLING));

		if ( (ap^bp) == false )
			return (a->getItemInfo()->getId().toInt64() < b->getItemInfo()->getId().toInt64()); 

		return ap;
	});

	group->start();
	group->finalize();
}


DesuraId ItemManager::addLink(const char* name, const char* exe, const char* args)
{
	if (!name || !exe)
		return DesuraId();

	DesuraId id(UTIL::MISC::RSHash_CSTR(name), DesuraId::TYPE_LINK);
	UserCore::Item::ItemInfo* info = findItemInfoNorm(id);

	if (info)
		return DesuraId();

	info = new UserCore::Item::ItemInfo(m_pUser, id);
	UserCore::Item::ItemHandle* handle = new UserCore::Item::ItemHandle(info, m_pUser);

	info->setName(name);
	info->setLinkInfo(exe, args);

	addItem(id.toInt64(), handle);
	onNewItem(id);

	uint32 count = 1;
	m_pUser->onItemsAddedEvent(count);
	onUpdateEvent();

	return id;
}

void ItemManager::updateLink(DesuraId id, const char* args)
{
	UserCore::Item::ItemInfo* info = findItemInfoNorm(id);

	if (!info)
		return;

	std::vector<UserCore::Item::Misc::ExeInfoI*> list;
	info->getExeList(list);

	if (list.size() == 1)
		list[0]->setUserArgs(args);
}


void ItemManager::parseKnownBranches(const XML::gcXMLElement &gamesNode)
{
	auto parseBranch = [this](DesuraId id, const XML::gcXMLElement &branch)
	{
		uint32 branchId = 0;
		branch.GetAtt("id", branchId);

		if (branchId == 0)
			return;

		m_BranchLock.lock();
		m_mBranchMapping[MCFBranch::BranchFromInt(branchId)] = id;
		m_BranchLock.unlock();
	};

	gamesNode.for_each_child("game", [this, &parseBranch](const XML::gcXMLElement &game)
	{
		const std::string szId = game.GetAtt("siteareaid");
	
		if (!szId.empty())
			return;

		DesuraId id(szId.c_str(), "games");

		auto parseBranchLocal = parseBranch;

		game.FirstChildElement("branches").for_each_child("branch", [&parseBranchLocal, &id](const XML::gcXMLElement &branch)
		{
			parseBranchLocal(id, branch);
		});

		game.FirstChildElement("mods").for_each_child("mod", [&parseBranchLocal](const XML::gcXMLElement &mod)
		{
			const std::string szId = mod.GetAtt("siteareaid");
	
			if (!szId.empty())
				return;

			DesuraId id(szId.c_str(), "mods");

			auto parseBranchLocalMod = parseBranchLocal;

			mod.FirstChildElement("branches").for_each_child("branch", [&parseBranchLocalMod, &id](const XML::gcXMLElement &branch)
			{
				parseBranchLocalMod(id, branch);
			});
		});
	});
}

bool ItemManager::isKnownBranch(MCFBranch branch, DesuraId id)
{
	bool res = false;

	m_BranchLock.lock();

	auto it = m_mBranchMapping.find(branch);

	if (it != m_mBranchMapping.end())
		res = (it->second.toInt64() == id.toInt64());

	m_BranchLock.unlock();

	return res;
}

bool ItemManager::isDelayLoading()
{
	bool delayLoading = m_pUser->isDelayLoading();

	if (delayLoading)
	{
		m_uiDelayCount++;

		if (m_uiDelayCount < 25)
			return true;
	}

	m_uiDelayCount = 0;
	return false;
}

void ItemManager::onNewItem(DesuraId id)
{
	bool isDelayLoading = m_pUser->isDelayLoading();

	if (isDelayLoading)
	{
		m_uiDelayCount++;

		if (m_uiDelayCount >= 25)
		{
			onUpdateEvent();
			m_uiDelayCount = 0;
		}
	}
	else
	{
		setNew(id);
		onNewItemEvent(id);
	}
}

void ItemManager::regenLaunchScripts()
{
#ifdef NIX
	m_pUser->getThreadPool()->forceTask(new UserCore::Task::RegenLaunchScriptsTask(m_pUser));
#endif
}

void ItemManager::saveItem(UserCore::Item::ItemInfoI* pItem)
{
	if (!m_bEnableSave)
		return;

	auto pItemNorm = dynamic_cast<UserCore::Item::ItemInfo*>(pItem);

	VERIFY_OR_RETURN(pItemNorm, );

	try
	{
		gcString szItemDb = getItemInfoDb(m_szAppPath.c_str());
		sqlite3x::sqlite3_connection db(szItemDb.c_str());
		pItemNorm->saveDbFull(&db);
	}
	catch (std::exception &e)
	{
		Warning("Failed to save item to db: {0}\n", e.what());
	}
}

void ItemManager::enableSave()
{
	if (m_bEnableSave)
		return;

	m_bEnableSave = true; 
	saveItems(); 
}
