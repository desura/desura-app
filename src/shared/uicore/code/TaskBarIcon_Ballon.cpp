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
#include "TaskBarIcon.h"

#include "MainApp.h"
#include "ButtonStrip.h"

#include <branding/branding.h>

enum
{
	BALLON_NONE = 0,
	BALLON_GIFTS,
	BALLON_APPUPDATE,
	BALLON_GAMEUPDATE,
};



bool sortItems(UserCore::Item::ItemInfoI* left, UserCore::Item::ItemInfoI* right) 
{ 
	return strcmp(left->getName(), right->getName()) > 0;
}


void TaskBarIcon::onBallonClick(wxTaskBarIconEvent &event)
{
	if (m_iLastBallon == BALLON_GIFTS)
	{
		g_pMainApp->showNews();
	}
	else if (m_iLastBallon == BALLON_APPUPDATE)
	{
		g_pMainApp->handleInternalLink(DesuraId(m_AppUpdateVer.build, 0), ACTION_APPUPDATELOG);
	}
	else if (m_iLastBallon == BALLON_GAMEUPDATE)
	{
		g_pMainApp->loadUrl(GetWebCore()->getUrl(WebCore::Updates).c_str(), COMMUNITY);
	}

	m_iLastBallon = BALLON_NONE;
}

void TaskBarIcon::onAppUpdate(UserCore::Misc::UpdateInfo &info)
{
	gcWString msg(Managers::GetString(L"#TB_APPUPDATE"), info.build);

	m_iLastBallon = BALLON_APPUPDATE;
	m_AppUpdateVer = info;

	ShowBalloon(Managers::GetString(L"#TB_APPUPDATE_TITLE"), msg.c_str());
}

void TaskBarIcon::onAppUpdateComplete(UserCore::Misc::UpdateInfo &info)
{

}

void TaskBarIcon::onAppUpdateProg(uint32& prog)
{
	if (m_uiLastProg == prog)
		return;

	if (prog == 0 || prog >= 100)
		m_szTooltip = PRODUCT_NAME;
	else
		m_szTooltip = gcString(PRODUCT_NAME " - Update Download Progress: {0}%", prog);

#ifdef WIN32
	SetIcon(m_wxIcon, m_szTooltip);
#else
	gcWString path(L"{0}/desura.png", UTIL::OS::getDataPath());
	wxIcon i(path, wxBITMAP_TYPE_PNG);
	SetIcon(i, m_szTooltip);	
#endif
}


void TaskBarIcon::onUpdate()
{
	doBallonMsg();
	updateIcon();
}

void TaskBarIcon::doBallonMsg()
{
	gcWString msg;

	std::sort(m_vNextUpdateList.begin(), m_vNextUpdateList.end(), sortItems);

	for (size_t x=0; x<m_vNextUpdateList.size(); x++)
	{
		if (!m_vNextUpdateList[x])
			continue;

		msg += gcWString(L" - {0}\n", m_vNextUpdateList[x]->getName());
	}

	if (msg.size() > 0)
	{
		swapUpdateList();
		m_iLastBallon = BALLON_GAMEUPDATE;
		ShowBalloon(Managers::GetString(L"#TB_TITLE"), msg.c_str());
	}
}

void TaskBarIcon::showGiftPopup(const std::vector<std::shared_ptr<UserCore::Misc::NewsItem>>& itemList)
{
	gcWString msg;

	std::vector<std::shared_ptr<UserCore::Misc::NewsItem>> vLocal(itemList);
	std::sort(vLocal.begin(), vLocal.end(), [](std::shared_ptr<UserCore::Misc::NewsItem> a, std::shared_ptr<UserCore::Misc::NewsItem> b)
	{
		return strcmp(a->szTitle.c_str(), b->szTitle.c_str()) > 0;
	});

	for (auto i : vLocal)
	{
		if (i->hasBeenShown)
			continue;

		msg += gcWString(L" - {0}\n", i->szTitle.c_str());
	}

	m_iLastBallon = BALLON_GIFTS;
	ShowBalloon(Managers::GetString(L"#TB_GIFTTITLE"), msg.c_str());
}

bool TaskBarIcon::findUpdateItem(UserCore::Item::ItemInfoI* item)
{
	if (!item)
		return false;

	for (size_t x=0; x<m_vNextUpdateList.size(); x++)
	{
		if (!m_vNextUpdateList[x])
			continue;

		if (m_vNextUpdateList[x] == item)
			return true;
	}

	for (size_t x=0; x<m_vUpdatedList.size(); x++)
	{
		if (!m_vUpdatedList[x])
			continue;

		if (m_vUpdatedList[x] == item)
			return true;
	}

	return false;
}

void TaskBarIcon::removeUpdateItem(UserCore::Item::ItemInfoI* item)
{
	if (!item)
		return;

	for (size_t x=0; x<m_vNextUpdateList.size(); x++)
	{
		if (!m_vNextUpdateList[x])
			continue;

		if (m_vNextUpdateList[x] == item)
		{
			m_vNextUpdateList.erase(m_vNextUpdateList.begin()+x);
			break;
		}
	}

	for (size_t x=0; x<m_vUpdatedList.size(); x++)
	{
		if (!m_vUpdatedList[x])
			continue;

		if (m_vUpdatedList[x] == item)
		{
			m_vUpdatedList.erase(m_vUpdatedList.begin()+x);
			break;
		}
	}
}

void TaskBarIcon::swapUpdateList()
{
	for (size_t x=0; x<m_vNextUpdateList.size(); x++)
	{
		if (!m_vNextUpdateList[x])
			continue;

		m_vUpdatedList.push_back(m_vNextUpdateList[x]);
	}

	m_vNextUpdateList.erase(m_vNextUpdateList.begin(), m_vNextUpdateList.end());
}

void TaskBarIcon::onItemChangedNonGui(UserCore::Item::ItemInfoI::ItemInfo_s& info)
{
	onItemChangedEvent(info);
}

void TaskBarIcon::onItemChanged(UserCore::Item::ItemInfoI::ItemInfo_s& info)
{
	if (!GetUserCore())
		return;

	if (info.changeFlags & UserCore::Item::ItemInfoI::CHANGED_STATUS)
	{
		UserCore::Item::ItemInfoI* item = GetUserCore()->getItemManager()->findItemInfo(info.id);
	
		if (!item)
			return;

		if (item->getStatus() & UserCore::Item::ItemInfoI::STATUS_UPDATEAVAL)
		{
			if (!findUpdateItem(item))
				m_vNextUpdateList.push_back(item);
		}
		else
		{
			removeUpdateItem(item);
		}
	}

	updateIcon();
}


void TaskBarIcon::tagItems()
{
	if (!GetUserCore())
		return;

	auto updateDelegate = [this](UserCore::Item::ItemInfoI* game)
	{
		const uint32 hasFlags = UserCore::Item::ItemInfoI::STATUS_DELETED;
		const uint32 notFlags = UserCore::Item::ItemInfoI::STATUS_ONACCOUNT | UserCore::Item::ItemInfoI::STATUS_ONCOMPUTER | UserCore::Item::ItemInfoI::STATUS_DEVELOPER;

		*game->getInfoChangeEvent() -= delegate(this, &TaskBarIcon::onItemChangedNonGui);

		if (HasAnyFlags(game->getStatus(), hasFlags))
			return;

		if (!HasAnyFlags(game->getStatus(), notFlags))
			return;

		*game->getInfoChangeEvent() += delegate(this, &TaskBarIcon::onItemChangedNonGui);
	};

	std::vector<UserCore::Item::ItemInfoI*> gList;
	GetUserCore()->getItemManager()->getGameList(gList, true);

	for (auto game : gList)
	{
		updateDelegate(game);

#ifndef UI_HIDE_MODS
		std::vector<UserCore::Item::ItemInfoI*> mList;
		GetUserCore()->getItemManager()->getModList(game->getId(), mList, true);

		for (auto mod : mList)
		{
			updateDelegate(mod);
		}
#endif
	}
}
