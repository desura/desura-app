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

	UserCore::UserI* user = GetUserCore();
	if (user)
	{
		messageCount = user->getPmCount();
		updateCount = user->getUpCount();
		cartCount = user->getCartCount();

		std::vector<UserCore::Item::ItemInfoI*> gList;
		user->getItemManager()->getGameList(gList);

		for (size_t x=0; x<gList.size(); x++)
		{
			if (HasAnyFlags(gList[x]->getStatus(), UserCore::Item::ItemInfoI::STATUS_UPDATEAVAL))
				gameUpdateCount++;

			std::vector<UserCore::Item::ItemInfoI*> mList;
			user->getItemManager()->getModList(gList[x]->getId(), mList);

			for (size_t y=0; y<mList.size(); y++)
			{
				if (HasAnyFlags(mList[y]->getStatus(), UserCore::Item::ItemInfoI::STATUS_UPDATEAVAL))
					gameUpdateCount++;
			}
		}
	}

	return messageCount + updateCount + gameUpdateCount + modUpdateCount + cartCount;
}


