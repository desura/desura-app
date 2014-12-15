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


#ifndef DESURA_THEME_MANAGER_H
#define DESURA_THEME_MANAGER_H

#include "managers/Managers.h"
#include "Theme.h"


class ThemeManager : public ThemeManagerI
{
public:
	ThemeManager();
	~ThemeManager();

	const char* getThemeFolder() override;
	const char* getWebPage(const char* id) override;
	const char* getImage(const char* id) override;
	Color getColor(const char* name, const char* id) override;

	//! Gets a rectangle from a sprite section
	//!
	//! @param id Sprite name
	//! @param rectId Rectangle name
	//! @return Rectangle info
	//!
	gcRefPtr<SpriteRectI> getSpriteRect(const char* id, const char* rectId) override;

	void loadFromFolder(const char* folder) override;
	void loadTheme(const char* theme) override;

	void getThemeStubList(std::vector<ThemeStubI*> &vList) override;

private:
	std::shared_ptr<Theme> m_pTheme;

	gcString m_szPath;
	gcString m_szThemeFolder;
};

#endif
