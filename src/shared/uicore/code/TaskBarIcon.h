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

#ifndef DESURA_TASKBAR_ICON_H
#define DESURA_TASKBAR_ICON_H
#ifdef _WIN32
#pragma once
#endif


#include "wx_controls/gcTaskBar.h"
#include "wx_controls/gcCustomMenu.h"
#include "wx/menu.h"
#include "Managers.h"

#define MODCORE_ICON "appicon.ico"


class TBIBaseMenu;

class TaskBarIcon : public gcTaskBarIcon //, gcCustomMenu
{
friend class windowReg;

public:
	TaskBarIcon(wxWindow *parent, UserCore::ItemManagerI* pItemManager = nullptr);
	~TaskBarIcon();

	void regEvents();
	void deregEvents();

	void showGiftPopup(const std::vector<std::shared_ptr<UserCore::Misc::NewsItem>>& itemList);

protected:
	friend class LanguageTestDialog;
	wxMenu* CreatePopupMenu();
	wxMenu* CreatePopupMenu(bool bOffline);

	void onBallonClick(wxTaskBarIconEvent &event);
	void onMenuSelect( wxCommandEvent& event );
    void onLeftButtonDClick(wxTaskBarIconEvent& event);
#ifdef NIX
    void onLeftButtonDown(wxTaskBarIconEvent& event);
#endif
	void onUpdate();

	void onItemChanged(UserCore::Item::ItemInfoI::ItemInfo_s& info);
	void onItemChangedNonGui(UserCore::Item::ItemInfoI::ItemInfo_s& info);

	void tagItems();
	void onItemsAdded(uint32&);
	void onUserUpdate();

	bool findUpdateItem(UserCore::Item::ItemInfoI* item);
	void removeUpdateItem(UserCore::Item::ItemInfoI* item);
	void swapUpdateList();

	void doBallonMsg();
	void updateIcon();

	uint32 getUpdateCount();

	void onAppUpdateComplete(UserCore::Misc::UpdateInfo &info);
	void onAppUpdate(UserCore::Misc::UpdateInfo &info);
	void onAppUpdateProg(uint32& prog);


private:
	void showMainWindow();

	Event<UserCore::Item::ItemInfoI::ItemInfo_s> onItemChangedEvent;

	uint32 m_iLastBallon;
	uint32 m_uiLastCount;
	uint32 m_uiLastProg;

	wxIcon m_wxIcon;
	wxWindow* m_wxParent;

	std::vector<UserCore::Item::ItemInfoI*> m_vNextUpdateList;
	std::vector<UserCore::Item::ItemInfoI*> m_vUpdatedList;

	UserCore::Misc::UpdateInfo m_AppUpdateVer;

	gcImageHandle m_imgCount;

#ifdef WIN32
	TBIBaseMenu* m_pModMenu;
#endif
	TBIBaseMenu* m_pGameMenu;
	TBIBaseMenu* m_pWindowMenu;
	TBIBaseMenu* m_pUpdateMenu;

	gcString m_szTooltip;

	friend void testapppopup_cc_func(std::vector<gcString> &vArgList);
};



#endif //DESURA_TASKBAR_ICON_H
