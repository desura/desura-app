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
		class BranchInfoI;
	}
}

class DesuraJSItemInfo : public DesuraJSBase<DesuraJSItemInfo>
{
public:
	DesuraJSItemInfo();

	virtual bool preExecuteValidation(const char* function, uint32 functionHash, JSObjHandle object, size_t argc, JSObjHandle* argv);


private:
	gcString getItemId(gcRefPtr<UserCore::Item::ItemInfoI> item);
	gcString getItemParentId(gcRefPtr<UserCore::Item::ItemInfoI> item);
	gcString getItemInstalledModId(gcRefPtr<UserCore::Item::ItemInfoI> item);

	int32 getItemChangedFlags(gcRefPtr<UserCore::Item::ItemInfoI> item);
	int32 getItemStatus(gcRefPtr<UserCore::Item::ItemInfoI> item);
	int32 getItemPercent(gcRefPtr<UserCore::Item::ItemInfoI> item);
	int32 getItemPermissions(gcRefPtr<UserCore::Item::ItemInfoI> item);
	int32 getItemOptions(gcRefPtr<UserCore::Item::ItemInfoI> item);

	bool isLink(gcRefPtr<UserCore::Item::ItemInfoI> item);
	bool isItemValid(gcRefPtr<UserCore::Item::ItemInfoI> item);
	bool isItemLaunchable(gcRefPtr<UserCore::Item::ItemInfoI> item);
	bool isItemUpdating(gcRefPtr<UserCore::Item::ItemInfoI> item);
	bool isItemInstalled(gcRefPtr<UserCore::Item::ItemInfoI> item);
	bool isItemDownloadable(gcRefPtr<UserCore::Item::ItemInfoI> item);

	gcString getItemGenre(gcRefPtr<UserCore::Item::ItemInfoI> item);
	gcString getItemRating(gcRefPtr<UserCore::Item::ItemInfoI> item);
	gcString getItemDev(gcRefPtr<UserCore::Item::ItemInfoI> item);
	gcString getItemName(gcRefPtr<UserCore::Item::ItemInfoI> item);
	gcString getItemShortName(gcRefPtr<UserCore::Item::ItemInfoI> item);
	gcString getItemStatusStr(gcRefPtr<UserCore::Item::ItemInfoI> item);

	gcString getItemPath(gcRefPtr<UserCore::Item::ItemInfoI> item);
	gcString getItemInsPrimary(gcRefPtr<UserCore::Item::ItemInfoI> item);
	gcString getItemIcon(gcRefPtr<UserCore::Item::ItemInfoI> item);
	gcString getItemLogo(gcRefPtr<UserCore::Item::ItemInfoI> item);
	gcString getItemIconUrl(gcRefPtr<UserCore::Item::ItemInfoI> item);
	gcString getItemLogoUrl(gcRefPtr<UserCore::Item::ItemInfoI> item);
	gcString getItemProfile(gcRefPtr<UserCore::Item::ItemInfoI> item);
	gcString getItemDevProfile(gcRefPtr<UserCore::Item::ItemInfoI> item);
	gcString getItemInstalledVersion(gcRefPtr<UserCore::Item::ItemInfoI> item);

	int32 getItemBranchCount(gcRefPtr<UserCore::Item::ItemInfoI> item);
	gcRefPtr<UserCore::Item::BranchInfoI> getItemBranchByIndex(gcRefPtr<UserCore::Item::ItemInfoI> item, int32 index);
	gcRefPtr<UserCore::Item::BranchInfoI> getItemBranchById(gcRefPtr<UserCore::Item::ItemInfoI> item, int32 id);
	gcRefPtr<UserCore::Item::BranchInfoI> getItemCurrentBranch(gcRefPtr<UserCore::Item::ItemInfoI> item);

	int32 getItemInstallSize(gcRefPtr<UserCore::Item::ItemInfoI> item);
	int32 getItemDownloadSize(gcRefPtr<UserCore::Item::ItemInfoI> item);
	int32 getItemInstalledBuild(gcRefPtr<UserCore::Item::ItemInfoI> item);
	int32 getItemInstalledBranch(gcRefPtr<UserCore::Item::ItemInfoI> item);
	int32 getItemType(gcRefPtr<UserCore::Item::ItemInfoI> item);


	bool isItemFavorite(gcRefPtr<UserCore::Item::ItemInfoI> item);
	void setItemFavorite(gcRefPtr<UserCore::Item::ItemInfoI> item, bool setFav);

	void addItemOptionFlags(gcRefPtr<UserCore::Item::ItemInfoI> item, int32 options);
	void delItemOptionFlags(gcRefPtr<UserCore::Item::ItemInfoI> item, int32 options);


	std::vector<MapElementI*> getItemExeInfo(gcRefPtr<UserCore::Item::ItemInfoI> item);
	void setItemUserArgs(gcRefPtr<UserCore::Item::ItemInfoI> item, std::vector<std::map<gcString, gcString>> args);

	bool isItemBusy(gcRefPtr<UserCore::Item::ItemInfoI> item);
	bool createItemDesktopShortcut(gcRefPtr<UserCore::Item::ItemInfoI> item);
	bool createItemMenuEntry(gcRefPtr<UserCore::Item::ItemInfoI> item);

	uint32 m_uiValidItemHash;
};

#endif //DESURA_GCJSITEMINFO_H
