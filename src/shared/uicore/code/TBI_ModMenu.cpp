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
#include "TBI_ModMenu.h"

#include "wx_controls/gcCustomMenu.h"
#include "MainApp.h"

wxMenu* TBIModMenu::createMenu(uint32 &lastMenuId)
{
	gcMenu* menu = new gcMenu();
	m_IdMapList.clear();

	auto pItemManager = getItemManager();

	if (!pItemManager)
		return menu;

	std::vector<UserCore::Item::ItemInfoI*> gList;
	pItemManager->getGameList(gList);

	std::sort(gList.begin(), gList.end(), [](UserCore::Item::ItemInfoI* left, UserCore::Item::ItemInfoI* right){
		return strcmp(left->getName(), right->getName()) <= 0;
	});

	for (size_t x=0; x<gList.size(); x++)
	{
		UserCore::Item::ItemInfoI *game = gList[x];

		std::vector<UserCore::Item::ItemInfoI*> mList;
		pItemManager->getModList(game->getId(), mList);

		if (mList.size() == 0)
			continue;

		gcMenu* gameMenu = new gcMenu();
		
		std::sort(mList.begin(), mList.end(), [](UserCore::Item::ItemInfoI* left, UserCore::Item::ItemInfoI* right){
			return strcmp(left->getName(), right->getName()) <= 0;
		});

		for (size_t y=0; y<mList.size(); y++)
		{
			UserCore::Item::ItemInfoI *mod = mList[y];

			wxMenuItem* menuItem = new gcMenuItem(gameMenu, lastMenuId, mod->getName());
			gameMenu->Append(menuItem);

			m_IdMapList[lastMenuId] = mod->getId();
			loadIcon(menuItem, mod->getIcon());

			lastMenuId++;
		}

		wxMenuItem* gameItem = new gcMenuItem(gameMenu, wxID_ANY, game->getName(), "", wxITEM_NORMAL, gameMenu);
		loadIcon(gameItem, game->getIcon());

		menu->Append(gameItem);
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

void TBIModMenu::onMenuSelect(wxCommandEvent& event)
{
	if (m_IdMapList.find(event.GetId()) != m_IdMapList.end())
		g_pMainApp->handleInternalLink(m_IdMapList[event.GetId()], ACTION_LAUNCH);
}

const wchar_t* TBIModMenu::getMenuName()
{
	return Managers::GetString(L"#TB_MODS");
}
