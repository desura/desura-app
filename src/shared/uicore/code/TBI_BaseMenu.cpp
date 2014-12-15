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
#include "TBI_BaseMenu.h"

#include "Managers.h"
#include "MainApp.h"

gcRefPtr<UserCore::ItemManagerI> TBIBaseMenu::getItemManager()
{
	if (m_pItemManager)
		return m_pItemManager;

	auto userCore = GetUserCore();

	if (userCore)
		return userCore->getItemManager();

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
