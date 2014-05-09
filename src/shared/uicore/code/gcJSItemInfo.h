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
	void* getItemBranchByIndex(gcRefPtr<UserCore::Item::ItemInfoI> item, int32 index);
	void* getItemBranchById(gcRefPtr<UserCore::Item::ItemInfoI> item, int32 id);
	void* getItemCurrentBranch(gcRefPtr<UserCore::Item::ItemInfoI> item);

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
