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
#include "TBI_GameMenu.h"

#include "wx_controls/gcCustomMenu.h"
#include "MainApp.h"


wxMenu* TBIGameMenu::createMenu(uint32 &lastMenuId)
{
	gcMenu* menu = new gcMenu();
	m_IdMapList.clear();

	auto pItemManager = getItemManager();

	if (!pItemManager)
		return menu;

	std::vector<gcRefPtr<UserCore::Item::ItemInfoI>> gList;
	pItemManager->getGameList(gList);

	std::sort(gList.begin(), gList.end(), [](gcRefPtr<UserCore::Item::ItemInfoI> left, gcRefPtr<UserCore::Item::ItemInfoI> right){
		return strcmp(left->getName(), right->getName()) <= 0;
	});

	for (size_t x=0; x<gList.size(); x++)
	{
		auto item = gList[x];

		if (!item)
			continue;

		if ((item->getStatus() & UserCore::Item::ItemInfoI::STATUS_DEVELOPER) && !(item->getStatus() & (UserCore::Item::ItemInfoI::STATUS_INSTALLING|UserCore::Item::ItemInfoI::STATUS_DOWNLOADING|UserCore::Item::ItemInfoI::STATUS_UPDATING|UserCore::Item::ItemInfoI::STATUS_READY|UserCore::Item::ItemInfoI::STATUS_INSTALLED)))
			continue;


		wxMenuItem* menuItem = new gcMenuItem(menu, lastMenuId, item->getName());
		menu->Append(menuItem);

		m_IdMapList[lastMenuId] = item->getId();
		loadIcon(menuItem, item->getIcon());

		lastMenuId++;
	}

	if (menu->GetMenuItemCount() == 0)
	{
		wxMenuItem* noItems = new gcMenuItem(menu, lastMenuId, Managers::GetString(L"#TB_NOITEMS"));
		lastMenuId++;

		menu->Append(noItems);
		noItems->Enable(false);
	}

	return menu;
}

void TBIGameMenu::onMenuSelect(wxCommandEvent& event)
{
	if (m_IdMapList.find(event.GetId()) != m_IdMapList.end())
		g_pMainApp->handleInternalLink(m_IdMapList[event.GetId()], ACTION_LAUNCH);
}

const wchar_t* TBIGameMenu::getMenuName()
{
	return Managers::GetString(L"#TB_GAMES");
}
