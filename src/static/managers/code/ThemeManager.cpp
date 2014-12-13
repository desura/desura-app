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
#include "ThemeManager.h"

#ifdef WIN32
	#include <windows.h>
#endif

#include "XMLMacros.h"


class ThemeStub : public ThemeStubI
{
public:
	ThemeStub(const char* name)
	{
		szName = name;
	}

	virtual const char* getName(){return szName.c_str();}

	virtual const char* getPrintName(){return szPrintName.c_str();}
	virtual const char* getAuthor(){return szAuthor.c_str();}
	virtual const char* getPreview(){return szPreview.c_str();}
	virtual const char* getVersion(){return szVersion.c_str();}

	gcString szName;
	gcString szPrintName;
	gcString szAuthor;
	gcString szPreview;
	gcString szVersion;

	virtual bool parseFile(const char* file);
	virtual void destroy(){delete this;}
};



ThemeManager::ThemeManager()
{
}

ThemeManager::~ThemeManager()
{
}

void ThemeManager::loadFromFolder(const char* folder)
{
	m_szPath = folder;
	loadTheme("default");
}

const char* ThemeManager::getThemeFolder()
{
	return m_szThemeFolder.c_str();
}

void ThemeManager::loadTheme(const char* theme)
{
	if (!theme || m_szPath.size() == 0)
		return;

	if (!m_pTheme)
		m_pTheme = std::make_shared<Theme>(theme);

	UTIL::FS::Path path(m_szPath, "theme.xml", false);
	path += theme;

	if (!UTIL::FS::isValidFile(path))
	{
		Warning("Theme {0} is not a valid file.\n", theme);
	}
	else
	{
		m_pTheme->parseFile(path.getFullPath().c_str());
		m_szThemeFolder = path.getFolderPath();
	}
}

const char* ThemeManager::getWebPage(const char* id)
{
	if (m_pTheme)
		return m_pTheme->getWebPage(id);

	return nullptr;
}

const char* ThemeManager::getImage(const char* id)
{
	if (m_pTheme)
		return m_pTheme->getImage(id);

	return nullptr;
}

Color ThemeManager::getColor(const char* name, const char* id)
{
	if (m_pTheme)
		return m_pTheme->getColor(name, id);

	return Color();
}

gcRefPtr<SpriteRectI> ThemeManager::getSpriteRect(const char* id, const char* rectId)
{
	if (m_pTheme)
		return m_pTheme->getSpriteRect(id, rectId);

	return nullptr;
}

void ThemeManager::getThemeStubList(std::vector<ThemeStubI*> &vList)
{
	std::vector<UTIL::FS::Path> folderList;
	UTIL::FS::getAllFolders(UTIL::FS::Path(m_szPath, "", false), folderList);

	for (size_t x=0; x<folderList.size(); x++)
	{
		UTIL::FS::Path xmlPath = folderList[x];
		xmlPath += UTIL::FS::File("theme.xml");

		if (!UTIL::FS::isValidFile(xmlPath))
			continue;

		ThemeStub* tTheme = new ThemeStub(xmlPath.getLastFolder().c_str());

		if (tTheme->parseFile(xmlPath.getFullPath().c_str()))
			vList.push_back(tTheme);
		else
			safe_delete(tTheme);
	}
}

bool ThemeStub::parseFile(const char* szFile)
{
	XML::gcXMLDocument doc(szFile);

	if (!doc.IsValid())
		return false;

	auto mcNode = doc.GetRoot("theme");

	if (!mcNode.IsValid())
		return false;

	mcNode.GetChild("creator", szAuthor);
	mcNode.GetChild("name", szPrintName);
	mcNode.GetChild("version", szVersion);

	gcString img;
	gcString folder;

	mcNode.GetChild("preview", img);
	img = UTIL::STRING::sanitizeFileName(img.c_str());

	UTIL::FS::Path path(szFile, "", true);
	szPreview = gcString("{0}{1}{2}", path.getLastFolder(), DIRS_STR, img);

	return (szName != "" && szAuthor != ""  && szPreview != ""  && szVersion != "" );
}

