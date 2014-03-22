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
#include "TBI_BaseMenu.h"

#include "Managers.h"
#include "MainApp.h"

UserCore::ItemManagerI* TBIBaseMenu::getItemManager()
{
	if (m_pItemManager)
		return m_pItemManager;

	if (GetUserCore())
		return GetUserCore()->getItemManager();

	return nullptr;
}

void TBIBaseMenu::loadIcon(wxMenuItem *item, const char* path)
{
	gcString szPath;

	if (path && UTIL::FS::isValidFile(UTIL::FS::PathWithFile(path)))
	{
		szPath = gcString(path);
	}
	else
	{
		szPath = gcString(GetGCThemeManager()->getImage("icon_default"));
	}

	if (UTIL::FS::isValidFile(UTIL::FS::PathWithFile(szPath)))
	{
		wxImage icon = wxImage(szPath.c_str());

		if (icon.IsOk())
			item->SetBitmap(wxBitmap(icon.Scale(16, 16)));
	}
}