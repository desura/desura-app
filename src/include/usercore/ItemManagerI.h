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


#ifndef DESURA_ITEMMANAGERI_H
#define DESURA_ITEMMANAGERI_H
#ifdef _WIN32
#pragma once
#endif

class WildcardManager;

namespace XML
{
	class gcXMLElement;
}

namespace UserCore
{

	namespace Item
	{
		class ItemHandleI;
		class ItemInfoI;
	}

	class ItemManagerI
	{
	public:
		virtual ~ItemManagerI(){}

		//! Loads the items from the file
		//!
		virtual void loadItems()=0;

		//! Saves the items to the file
		//!
		virtual void saveItems()=0;

		//! Is the item localy installed
		//!
		//! @param internID item id
		//! @return True if installed, false if not
		//!
		virtual bool isInstalled(DesuraId id)=0;

		//! Removes an item from the item list
		//!
		//! @param internId Item id
		//!
		virtual void removeItem(DesuraId id)=0;


		//! Gets item info from the web and saves it into the user
		//!
		//! @param internId Item id
		//! @param pWildCard Wildcard manager
		//! @param reset used for installs. Tells it to only look at the first install path
		//!
		virtual void retrieveItemInfo(DesuraId id, uint32 statusOveride = 0, WildcardManager* pWildCard = nullptr, MCFBranch mcfBranch = MCFBranch(), MCFBuild mcfBuild = MCFBuild(), bool reset = false)=0;

		//! Gets item info from the web and saves it into the user using a new thread
		//!
		//! @param internId Item id
		//! @param addToAccount add the item to the user account (shows up on games list)
		//!
		virtual void retrieveItemInfoAsync(DesuraId id, bool addToAccount = false)=0;

		//! Gets the developer item count
		//!
		//! @return Dev count
		//!
		virtual uint32 getDevItemCount()=0;

		//! Gets a ItemInfo from the list
		//!
		//! @param id Item id
		//! @return ItemInfo
		//!
		virtual UserCore::Item::ItemInfoI* findItemInfo(DesuraId id)=0;


		//! Gets a ItemHandle from the list
		//!
		//! @param id Item id
		//! @return ItemHandle
		//!
		virtual UserCore::Item::ItemHandleI* findItemHandle(DesuraId ide)=0;

		//! Gets the item count
		//!
		//! @return Item Count
		//!
		virtual uint32 getCount()=0;

		//! Gets the item at index
		//!
		//! @param index Index of item to get
		//! @return Item
		//!
		virtual UserCore::Item::ItemInfoI* getItemInfo(uint32 index)=0;
		virtual UserCore::Item::ItemHandleI* getItemHandle(uint32 index)=0;

		//! Get custom install path for an item
		//!
		//! @param itemId Item id
		//! @param buff Out buffer
		//!
		virtual void getCIP(DesuraId id, char** buff)=0;


		//! Gets a list of all items
		//!
		//! @param aList Item vector. Do not delete the items on the list
		//!
		virtual void getAllItems(std::vector<UserCore::Item::ItemInfoI*> &aList)=0;

		//! Gets a list of games
		//!
		//! @param gList Game vector. Do not delete the items on the list
		//!
		virtual void getGameList(std::vector<UserCore::Item::ItemInfoI*> &gList, bool includeDeleted = false)=0;

		//! Gets a list of mods for a game
		//!
		//! @param gameId Id of the game
		//! @param mList Mod vector. Do not delete the items on the list
		//!
		virtual void getModList(DesuraId gameId, std::vector<UserCore::Item::ItemInfoI*> &mList, bool includeDeleted = false)=0;

		//! Gets a list of developer items
		//!
		//! @param dList Developer vector. Do not delete the items on the list
		//!
		virtual void getDevList(std::vector<UserCore::Item::ItemInfoI*> &dList)=0;

		//! Gets a list of favorite items
		//!
		//! @param fList Favorite vector. Do not delete the items on the list
		//!
		virtual void getFavList(std::vector<UserCore::Item::ItemInfoI*> &fList)=0;

		//! Gets a list of recent items
		//!
		//! @param rList Recent vector. Do not delete the items on the list
		//!
		virtual void getRecentList(std::vector<UserCore::Item::ItemInfoI*> &rList)=0;

		//! Gets a list of link items
		//!
		//! @param lList Link vector. Do not delete the items on the list
		//!
		virtual void getLinkList(std::vector<UserCore::Item::ItemInfoI*> &lList)=0;


		//! Gets a list of new items
		//!
		//! @param tList Item vector. Do not delete the items on the list
		//!
		virtual void getNewItems(std::vector<UserCore::Item::ItemInfoI*> &tList)=0;
	

		//! Call this when an item (i.e. mod) needs to update
		//!
		//! @param itemsNode Item xml
		//!
		virtual void itemsNeedUpdate(const XML::gcXMLElement &itemsNode)=0;
		virtual void itemsNeedUpdate2(const XML::gcXMLElement &platformsNode)=0;


		virtual void setFavorite(DesuraId id, bool fav)=0;

		virtual void setRecent(DesuraId id)=0;

		virtual void setInstalledMod(DesuraId parentId, DesuraId modId)=0;

		virtual void checkItems()=0;

		virtual EventV* getOnUpdateEvent()=0;
		virtual Event<DesuraId>* getOnRecentUpdateEvent()=0;
		virtual Event<DesuraId>* getOnFavoriteUpdateEvent()=0;
		virtual Event<DesuraId>* getOnNewItemEvent()=0;


		virtual DesuraId addLink(const char* name, const char* exe, const char* args)=0;
		virtual void updateLink(DesuraId id, const char* args)=0;

		//! Is this branch known but not existing due to filtering etc
		//!
		//! @param branch Branch id
		//! @param id Item id
		//!
		virtual bool isKnownBranch(MCFBranch branch, DesuraId id)=0;

		//! Is this a fav item
		//!
		virtual bool isItemFavorite(DesuraId id)=0;
	
		//! Linux only
		virtual void regenLaunchScripts()=0;

		//! Save item to the db
		virtual void saveItem(UserCore::Item::ItemInfoI* pItem)=0;
	};

#ifdef LINK_WITH_GMOCK
	class ItemManagerMock : public ItemManagerI
	{
	public:
		MOCK_METHOD0(loadItems, void());
		MOCK_METHOD0(saveItems, void());
		MOCK_METHOD1(isInstalled, bool(DesuraId id));
		MOCK_METHOD1(removeItem, void(DesuraId id));
		MOCK_METHOD6(retrieveItemInfo, void(DesuraId, uint32, WildcardManager*, MCFBranch, MCFBuild, bool));
		MOCK_METHOD2(retrieveItemInfoAsync, void(DesuraId id, bool addToAccount));
		MOCK_METHOD0(getDevItemCount, uint32());
		MOCK_METHOD1(findItemInfo, UserCore::Item::ItemInfoI*(DesuraId id));
		MOCK_METHOD1(findItemHandle, UserCore::Item::ItemHandleI*(DesuraId ide));
		MOCK_METHOD0(getCount, uint32());
		MOCK_METHOD1(getItemInfo, UserCore::Item::ItemInfoI*(uint32 index));
		MOCK_METHOD1(getItemHandle, UserCore::Item::ItemHandleI*(uint32 index));
		MOCK_METHOD2(getCIP, void(DesuraId id, char** buff));
		MOCK_METHOD1(getAllItems, void(std::vector<UserCore::Item::ItemInfoI*> &aList));
		MOCK_METHOD2(getGameList, void(std::vector<UserCore::Item::ItemInfoI*> &gList, bool includeDeleted));
		MOCK_METHOD3(getModList, void(DesuraId gameId, std::vector<UserCore::Item::ItemInfoI*> &mList, bool includeDeleted));
		MOCK_METHOD1(getDevList, void(std::vector<UserCore::Item::ItemInfoI*> &dList));
		MOCK_METHOD1(getFavList, void(std::vector<UserCore::Item::ItemInfoI*> &fList));
		MOCK_METHOD1(getRecentList, void(std::vector<UserCore::Item::ItemInfoI*> &rList));
		MOCK_METHOD1(getLinkList, void(std::vector<UserCore::Item::ItemInfoI*> &lList));
		MOCK_METHOD1(getNewItems, void(std::vector<UserCore::Item::ItemInfoI*> &tList));
		MOCK_METHOD1(itemsNeedUpdate, void(const XML::gcXMLElement &itemsNode));
		MOCK_METHOD1(itemsNeedUpdate2, void(const XML::gcXMLElement &platformsNode));
		MOCK_METHOD2(setFavorite, void(DesuraId id, bool fav));
		MOCK_METHOD1(setRecent, void(DesuraId id));
		MOCK_METHOD2(setInstalledMod, void(DesuraId parentId, DesuraId modId));
		MOCK_METHOD0(checkItems, void());
		MOCK_METHOD0(getOnUpdateEvent, EventV*());
		MOCK_METHOD0(getOnRecentUpdateEvent, Event<DesuraId>*());
		MOCK_METHOD0(getOnFavoriteUpdateEvent, Event<DesuraId>*());
		MOCK_METHOD0(getOnNewItemEvent, Event<DesuraId>*());
		MOCK_METHOD3(addLink, DesuraId(const char* name, const char* exe, const char* args));
		MOCK_METHOD2(updateLink, void(DesuraId id, const char* args));
		MOCK_METHOD2(isKnownBranch, bool(MCFBranch branch, DesuraId id));
		MOCK_METHOD1(isItemFavorite, bool(DesuraId id));
		MOCK_METHOD0(regenLaunchScripts, void());
		MOCK_METHOD1(saveItem, void(UserCore::Item::ItemInfoI* pItem));
	};

#endif
}


#endif //DESURA_ITEMMANAGERI_H
