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
#include "TaskBarIcon.h"

#include "MainApp.h"

const wchar_t* g_szIconNames[] =
{
	L"IDI_ICONSMALL",
	L"IDI_ICONSMALL_1",
	L"IDI_ICONSMALL_2",
	L"IDI_ICONSMALL_3",
	L"IDI_ICONSMALL_4",
	L"IDI_ICONSMALL_5",
	L"IDI_ICONSMALL_6",
	L"IDI_ICONSMALL_7",
	L"IDI_ICONSMALL_8",
	L"IDI_ICONSMALL_9",
};


void TaskBarIcon::updateIcon()
{
	uint32 count = getUpdateCount();

	if (count == m_uiLastCount)
		return;

	m_uiLastCount = count;

#ifdef WIN32
	if (count < 10)
		m_wxIcon = wxIcon(wxICON(g_szIconNames[count]));
	else
		m_wxIcon = wxIcon(wxICON(IDI_ICONSMALL_P));

	SetIcon(m_wxIcon, m_szTooltip);
#endif // TODO LINUX
}


uint32 TaskBarIcon::getUpdateCount()
{
	uint32 gameUpdateCount = 0;
	uint32 modUpdateCount = 0;
	uint32 messageCount = 0;
	uint32 updateCount = 0;
	uint32 cartCount = 0;

	auto user = GetUserCore();

	if (user)
	{
		messageCount = user->getPmCount();
		updateCount = user->getUpCount();
		cartCount = user->getCartCount();

		auto im = user->getItemManager();

		if (im)
		{
			std::vector<gcRefPtr<UserCore::Item::ItemInfoI>> gList;
			im->getGameList(gList);

			for (auto& g : gList)
			{
				if (HasAnyFlags(g->getStatus(), UserCore::Item::ItemInfoI::STATUS_UPDATEAVAL))
					gameUpdateCount++;

				std::vector<gcRefPtr<UserCore::Item::ItemInfoI>> mList;
				im->getModList(g->getId(), mList);

				for (auto& m : mList)
				{
					if (HasAnyFlags(m->getStatus(), UserCore::Item::ItemInfoI::STATUS_UPDATEAVAL))
						gameUpdateCount++;
				}
			}
		}
	}

	return messageCount + updateCount + gameUpdateCount + modUpdateCount + cartCount;
}


