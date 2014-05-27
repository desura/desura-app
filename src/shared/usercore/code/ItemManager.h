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

#ifndef DESURA_ITEMMANAGER_H
#define DESURA_ITEMMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "usercore/ItemManagerI.h"
#include "BaseManager.h"
#include "usercore/ItemInfoI.h"
#include "UpdateThread_Old.h"

#include "ItemInfo.h"
#include "ItemHandle.h"

namespace XML
{
	class gcXMLElement;
}

class InfoMaps;

namespace UserCore
{

	namespace Item
	{
		class ItemTaskGroup;
	}

	class ItemManager : public ItemManagerI, public BaseManager<UserCore::Item::ItemHandle>
	{
	public:
		ItemManager(gcRefPtr<User> user);
		~ItemManager();

		void cleanup();

		typedef std::map<uint64, gcRefPtr<UserCore::Item::ItemHandle>>::iterator itemIterator;

		void loadItems() override;
		void saveItems() override;

		bool isInstalled(DesuraId id) override;
		void removeItem(DesuraId id) override;

		void getCIP(DesuraId id, char** buff) override;
		void retrieveItemInfo(DesuraId id, uint32 statusOveride = 0, gcRefPtr<WildcardManager> pWildCard = gcRefPtr<WildcardManager>(), MCFBranch branch = MCFBranch(), MCFBuild mcfBuild = MCFBuild(), bool reset = false) override;
		void retrieveItemInfoAsync(DesuraId id, bool addToAccount) override;

		uint32 getDevItemCount() override;

		gcRefPtr<UserCore::Item::ItemInfoI> findItemInfo(DesuraId id) override;
		gcRefPtr<UserCore::Item::ItemHandleI> findItemHandle(DesuraId id) override;

		uint32 getCount() override;
		gcRefPtr<UserCore::Item::ItemInfoI> getItemInfo(uint32 index) override;
		gcRefPtr<UserCore::Item::ItemHandleI> getItemHandle(uint32 index) override;

		void getAllItems(std::vector<gcRefPtr<UserCore::Item::ItemInfoI>> &aList) override;
		void getGameList(std::vector<gcRefPtr<UserCore::Item::ItemInfoI>> &gList, bool includeDeleted = false) override;
		void getModList(DesuraId gameId, std::vector<gcRefPtr<UserCore::Item::ItemInfoI>> &mList, bool includeDeleted = false) override;
		void getDevList(std::vector<gcRefPtr<UserCore::Item::ItemInfoI>> &dList) override;
		void getLinkList(std::vector<gcRefPtr<UserCore::Item::ItemInfoI>> &lList) override;
		void getFavList(std::vector<gcRefPtr<UserCore::Item::ItemInfoI>> &fList) override;
		void getRecentList(std::vector<gcRefPtr<UserCore::Item::ItemInfoI>> &rList) override;
		void getNewItems(std::vector<gcRefPtr<UserCore::Item::ItemInfoI>> &tList) override;

		void itemsNeedUpdate(const XML::gcXMLElement &itemsNode) override;
		void itemsNeedUpdate2(const XML::gcXMLElement &platformsNode) override;

		EventV& getOnUpdateEvent() override;
		Event<DesuraId>& getOnRecentUpdateEvent() override;
		Event<DesuraId>& getOnFavoriteUpdateEvent() override;
		Event<DesuraId>& getOnNewItemEvent() override;

		void setFavorite(DesuraId id, bool fav) override;
		void setRecent(DesuraId id) override;
		void setInstalledMod(DesuraId parentId, DesuraId modId) override;

		void checkItems() override;

		void setNew(DesuraId &id);

		gcRefPtr<UserCore::Item::ItemInfo> findItemInfoNorm(DesuraId id);
		gcRefPtr<UserCore::Item::ItemHandle> findItemHandleNorm(DesuraId id);

		void enableSave();
		gcRefPtr<UserCore::Item::ItemTaskGroup> newTaskGroup(uint32 type);

		DesuraId addLink(const char* name, const char* exe, const char* args) override;
		void updateLink(DesuraId id, const char* args) override;

		bool isKnownBranch(MCFBranch branch, DesuraId id) override;
		bool isItemFavorite(DesuraId id) override;

		void regenLaunchScripts() override;

		void saveItem(gcRefPtr<UserCore::Item::ItemInfoI> pItem) override;

		gc_IMPLEMENT_REFCOUNTING(ItemManager);

	protected:
		class ParseInfo;

		void parseXml(uint16 statusOverride = 0);

		void parseLoginXml(const XML::gcXMLElement &gamesNode, const XML::gcXMLElement &devNodes);
		void parseLoginXml2(const XML::gcXMLElement &gamesNode, const XML::gcXMLElement &platformNodes);
		void postParseLoginXml();

		void parseGamesXml(ParseInfo &pi);
		void parseGameXml(DesuraId id, ParseInfo &pi);

		void parseModsXml(gcRefPtr<UserCore::Item::ItemInfo> parent, ParseInfo &pi);
		void parseModXml(gcRefPtr<UserCore::Item::ItemInfo> parent, DesuraId id, ParseInfo &pi);

		void parseItemUpdateXml(const char* area, const XML::gcXMLElement &itemsNode);

		gcRefPtr<UserCore::Item::ItemInfo> createNewItem(DesuraId pid, DesuraId id, ParseInfo &pi);
		void updateItem(gcRefPtr<UserCore::Item::ItemInfo> info, ParseInfo &pi);

		DesuraId getParentId(const XML::gcXMLElement &gameNode, const XML::gcXMLElement &infoNode);

		void processLeftOvers(InfoMaps &maps, bool addMissing);

		//! Finds the index of an item given the internal id
		//!
		//! @param internId Internal id of item
		//! @return Item index
		//!
		uint32 findItemIndex(DesuraId id);


		void loadDbItems();
		void saveDbItems(bool fullSave = false);
		void updateItemIds();

		friend class User;
		friend class UpdateThreadOld;

		EventV onUpdateEvent;
		Event<DesuraId> onRecentUpdateEvent;
		Event<DesuraId> onFavoriteUpdateEvent;
		Event<DesuraId> onNewItemEvent;

		void migrateOldItemInfo(const char* olddb, const char* newdb);
		void migrateStandaloneFiles();

		void generateInfoMaps(const XML::gcXMLElement &gamesNode, InfoMaps* maps);


		void parseKnownBranches(const XML::gcXMLElement &gamesNode);

		void onNewItem(DesuraId id);
		bool isDelayLoading();

		void loadFavList();

	private:
		uint32 m_uiDelayCount;
		gcString m_szAppPath;

		bool m_bEnableSave;
		bool m_bFirstLogin;

		gcRefPtr<User> m_pUser;

		std::mutex m_BranchLock;
		std::map<MCFBranch, DesuraId> m_mBranchMapping;

		std::mutex m_FavLock;
		std::vector<DesuraId> m_vFavList;
	};




	inline uint32 ItemManager::getCount()
	{
		return BaseManager::getCount();
	}

	inline gcRefPtr<UserCore::Item::ItemInfoI> ItemManager::getItemInfo(uint32 index)
	{
		auto handle = getItemHandle(index);

		if (handle)
			return handle->getItemInfo();

		return nullptr;
	}

	inline gcRefPtr<UserCore::Item::ItemHandleI> ItemManager::getItemHandle(uint32 index)
	{
		return BaseManager::getItem(index);
	}


	inline EventV& ItemManager::getOnUpdateEvent()
	{
		return onUpdateEvent;
	}

	inline Event<DesuraId>& ItemManager::getOnRecentUpdateEvent()
	{
		return onRecentUpdateEvent;
	}

	inline Event<DesuraId>& ItemManager::getOnFavoriteUpdateEvent()
	{
		return onFavoriteUpdateEvent;
	}

	inline Event<DesuraId>& ItemManager::getOnNewItemEvent()
	{
		return onNewItemEvent;
	}
}

#endif //DESURA_ITEMMANAGER_H
