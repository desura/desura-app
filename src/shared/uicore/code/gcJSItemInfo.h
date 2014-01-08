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

#ifndef DESURA_GCJSITEMINFO_H
#define DESURA_GCJSITEMINFO_H
#ifdef _WIN32
#pragma once
#endif

#include "gcJSBase.h"

namespace UserCore
{
	namespace Item
	{
		class ItemInfoI;
	}
}

class DesuraJSItemInfo : public DesuraJSBase<DesuraJSItemInfo>
{
public:
	DesuraJSItemInfo();

	virtual bool preExecuteValidation(const char* function, uint32 functionHash, JSObjHandle object, size_t argc, JSObjHandle* argv);

	
private:
	gcString getItemId(UserCore::Item::ItemInfoI* item);
	gcString getItemParentId(UserCore::Item::ItemInfoI* item);
	gcString getItemInstalledModId(UserCore::Item::ItemInfoI* item);

	int32 getItemChangedFlags(UserCore::Item::ItemInfoI* item);
	int32 getItemStatus(UserCore::Item::ItemInfoI* item);
	int32 getItemPercent(UserCore::Item::ItemInfoI* item);
	int32 getItemPermissions(UserCore::Item::ItemInfoI* item);
	int32 getItemOptions(UserCore::Item::ItemInfoI* item);

	bool isLink(UserCore::Item::ItemInfoI* item);
	bool isItemValid(UserCore::Item::ItemInfoI* item);
	bool isItemLaunchable(UserCore::Item::ItemInfoI* item);
	bool isItemUpdating(UserCore::Item::ItemInfoI* item);
	bool isItemInstalled(UserCore::Item::ItemInfoI* item);
	bool isItemDownloadable(UserCore::Item::ItemInfoI* item);

	gcString getItemGenre(UserCore::Item::ItemInfoI* item);
	gcString getItemRating(UserCore::Item::ItemInfoI* item);
	gcString getItemDev(UserCore::Item::ItemInfoI* item);
	gcString getItemName(UserCore::Item::ItemInfoI* item);
	gcString getItemShortName(UserCore::Item::ItemInfoI* item);
	gcString getItemStatusStr(UserCore::Item::ItemInfoI* item);

	gcString getItemPath(UserCore::Item::ItemInfoI* item);
	gcString getItemInsPrimary(UserCore::Item::ItemInfoI* item);
	gcString getItemIcon(UserCore::Item::ItemInfoI* item);
	gcString getItemLogo(UserCore::Item::ItemInfoI* item);
	gcString getItemIconUrl(UserCore::Item::ItemInfoI* item);
	gcString getItemLogoUrl(UserCore::Item::ItemInfoI* item);
	gcString getItemProfile(UserCore::Item::ItemInfoI* item);
	gcString getItemDevProfile(UserCore::Item::ItemInfoI* item);
	gcString getItemInstalledVersion(UserCore::Item::ItemInfoI* item);

	int32 getItemBranchCount(UserCore::Item::ItemInfoI* item);
	void* getItemBranchByIndex(UserCore::Item::ItemInfoI* item, int32 index);
	void* getItemBranchById(UserCore::Item::ItemInfoI* item, int32 id);
	void* getItemCurrentBranch(UserCore::Item::ItemInfoI* item);

	int32 getItemInstallSize(UserCore::Item::ItemInfoI* item);
	int32 getItemDownloadSize(UserCore::Item::ItemInfoI* item);
	int32 getItemInstalledBuild(UserCore::Item::ItemInfoI* item);
	int32 getItemInstalledBranch(UserCore::Item::ItemInfoI* item);
	int32 getItemType(UserCore::Item::ItemInfoI* item);


	bool isItemFavorite(UserCore::Item::ItemInfoI* item);
	void setItemFavorite(UserCore::Item::ItemInfoI* item, bool setFav);

	void addItemOptionFlags(UserCore::Item::ItemInfoI* item, int32 options);
	void delItemOptionFlags(UserCore::Item::ItemInfoI* item, int32 options);


	std::vector<MapElementI*> getItemExeInfo(UserCore::Item::ItemInfoI* item);
	void setItemUserArgs(UserCore::Item::ItemInfoI* item, std::vector<std::map<gcString, gcString>> args);

	bool isItemBusy(UserCore::Item::ItemInfoI* item);
	bool createItemDesktopShortcut(UserCore::Item::ItemInfoI* item);
	bool createItemMenuEntry(UserCore::Item::ItemInfoI* item);

	uint32 m_uiValidItemHash;
};

#endif //DESURA_GCJSITEMINFO_H
