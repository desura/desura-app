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
#include "managers/CVar.h"
#include "gcJSSettings.h"

#include "MainApp.h"

#define STEAMPATH "HKEY_CURRENT_USER\\Software\\Valve\\Steam\\SteamPath"
#define NOSTEAM "Steam Not Installed"

#include <wx/filepicker.h>
#include "usercore/CIPManagerI.h"

#ifdef NIX
#include "util/UtilLinux.h"
#endif

#include "XMLMacros.h"

REGISTER_JSEXTENDER(DesuraJSSettings);

DesuraJSSettings::DesuraJSSettings() : DesuraJSBase("settings", "native_binding_settings.js")
{
	REG_SIMPLE_JS_FUNCTION( getValue,	DesuraJSSettings );
	REG_SIMPLE_JS_FUNCTION( getCoreCount,		DesuraJSSettings );

	REG_SIMPLE_JS_FUNCTION( getLanguages, DesuraJSSettings );
	REG_SIMPLE_JS_FUNCTION( getSteamNames,	DesuraJSSettings );
	REG_SIMPLE_JS_FUNCTION( getThemes, DesuraJSSettings );

	REG_SIMPLE_JS_VOIDFUNCTION( setValue, DesuraJSSettings );

	REG_SIMPLE_JS_FUNCTION( getCurrentCIPItems, DesuraJSSettings );
	REG_SIMPLE_JS_FUNCTION( getAllCIPItems,	DesuraJSSettings );
	REG_SIMPLE_JS_FUNCTION( isValidCIPPath, DesuraJSSettings );
	REG_SIMPLE_JS_FUNCTION( browseCIPPath, DesuraJSSettings );

	REG_SIMPLE_JS_VOIDFUNCTION( updateCIPList,		DesuraJSSettings );
	REG_SIMPLE_JS_VOIDFUNCTION( saveCIPList, DesuraJSSettings );

	REG_SIMPLE_JS_FUNCTION( isValidLinkBinary, DesuraJSSettings );
	REG_SIMPLE_JS_FUNCTION( browseLinkBinary, DesuraJSSettings );
}



gcString DesuraJSSettings::getValue(gcString name)
{
	auto cvar = GetCVarManager()->findCVar(name.c_str());
	if (!cvar)
		return "";

	return cvar->getExitString();
}

int32 DesuraJSSettings::getCoreCount()
{
	return UTIL::MISC::getCoreCount();
}

void DesuraJSSettings::setValue(gcString name, gcString val)
{
	auto cvar = GetCVarManager()->findCVar(name.c_str());

	if (cvar && val != gcString(cvar->getString()))
		cvar->setValue(val.c_str());
}

std::vector<MapElementI*> DesuraJSSettings::getThemes()
{
	std::vector<ThemeStubI*> m_vThemes;
	GetGCThemeManager()->getThemeStubList(m_vThemes);

	std::vector<MapElementI*> ret;

	for (size_t x=0; x<m_vThemes.size(); x++)
	{
		UTIL::FS::Path file(UTIL::OS::getDataPath(L"themes"), L"", false);
		UTIL::FS::Path themePath = UTIL::FS::PathWithFile(m_vThemes[x]->getPreview());

		file += themePath;
		file += themePath.getFile();

		if (!UTIL::FS::isValidFile(file))
			continue;

		gcString fileUrl("desura://themeimage/{0}", m_vThemes[x]->getPreview());
		fileUrl = UTIL::STRING::sanitizeFilePath(fileUrl, '/');

		std::map<gcString, gcString> map;

		map["name"] = m_vThemes[x]->getName();
		map["displayName"] = m_vThemes[x]->getPrintName();
		map["image"] = fileUrl;
		map["tooltip"] = m_vThemes[x]->getPrintName();
		map["author"] = m_vThemes[x]->getAuthor();
		map["version"] = m_vThemes[x]->getVersion();

		ret.push_back(new MapElement<std::map<gcString, gcString>>(map));
	}

	safe_delete(m_vThemes);
	return ret;
}


std::vector<std::map<gcString, gcString>> GetLanguages()
{
	std::vector<std::map<gcString, gcString>> ret;

	std::vector<UTIL::FS::Path> fileList;
	std::vector<std::string> filter;
	filter.push_back("xml");

	UTIL::FS::getAllFiles(UTIL::FS::Path(UTIL::OS::getDataPath(L"language"),
		L"", false), fileList, &filter);

	std::map<gcString, uint32> mSeenBefore;

	for (size_t x=0; x<fileList.size(); x++)
	{
		XML::gcXMLDocument doc(fileList[x].getFullPath().c_str());

		auto uNode = doc.GetRoot("lang");

		if (!uNode.IsValid())
			continue;

		gcString name = uNode.GetAtt("name");
		gcString skip = uNode.GetAtt("skip");

		if (name.size() == 0)
			continue;

		if (skip == "true")
			continue;

		gcString file(fileList[x].getFile().getFile());
		size_t pos = file.find_last_of(".");

		if (pos != std::string::npos)
			file[pos] = '\0';

		if (mSeenBefore[name])
			name += gcString("({0})", mSeenBefore[name]);

		mSeenBefore[name]++;

		std::map<gcString, gcString> map;

		map["file"] = file;
		map["name"] = name;

		ret.push_back(map);
	}

	return ret;
}

std::vector<MapElementI*> DesuraJSSettings::getLanguages()
{
	std::vector<MapElementI*> ret;

	for (auto lang : GetLanguages())
		ret.push_back(new MapElement<std::map<gcString, gcString>>(lang));

	return ret;
}

extern int GetSteamUsers(std::vector<gcString> &vUsers);

std::vector<gcString> DesuraJSSettings::getSteamNames()
{
	std::vector<gcString> ret;
	GetSteamUsers(ret);

	if (ret.size() == 0)
		ret.push_back(NOSTEAM);

	return ret;
}









std::vector<MapElementI*> DesuraJSSettings::getCurrentCIPItems()
{
	std::vector<UserCore::Misc::CIPItem> list;

	auto userCore = GetUserCore();

	if (userCore)
		userCore->getCIPManager()->getCIPList(list);

	std::vector<MapElementI*> ret;

	for (size_t x=0; x<list.size(); x++)
	{
		std::map<gcString, gcString> map;

		map["name"] = list[x].name;
		map["path"] = list[x].path;
		map["id"] = list[x].id.toString();

		ret.push_back(new MapElement<std::map<gcString, gcString>>(map));
	}

	return ret;
}

std::vector<MapElementI*> DesuraJSSettings::getAllCIPItems()
{
	std::vector<UserCore::Misc::CIPItem> list;

	auto userCore = GetUserCore();

	if (userCore)
		userCore->getCIPManager()->getItemList(list);

	std::vector<MapElementI*> ret;

	for (size_t x=0; x<list.size(); x++)
	{
		std::map<gcString, gcString> map;

		map["name"] = list[x].name;
		map["id"] = list[x].id.toString();

		ret.push_back(new MapElement<std::map<gcString, gcString>>(map));
	}

	return ret;
}

void DesuraJSSettings::updateCIPList()
{
	auto userCore = GetUserCore();

	if (userCore)
		userCore->getCIPManager()->refreshList();
}

void DesuraJSSettings::saveCIPList(std::vector<std::map<gcString, gcString>> savelist)
{
	std::vector<UserCore::Misc::CIPItem> list;

	if (!GetUserCore())
		return;

	GetUserCore()->getCIPManager()->getCIPList(list);

	for (size_t x=0; x<savelist.size(); x++)
	{
		for (size_t y=0; y<list.size(); y++)
		{
			if (list[y].id.toString() == savelist[x]["id"])
			{
				list.erase(list.begin()+y);
				break;
			}
		}

		DesuraId id(Safe::atoll(savelist[x]["id"].c_str()));

		if (id.isOk() == false)
			continue;

		GetUserCore()->getCIPManager()->updateItem(id, savelist[x]["path"]);
	}

	for (size_t x=0; x<list.size(); x++)
	{
		GetUserCore()->getCIPManager()->deleteItem(list[x].id);
	}
}

bool DesuraJSSettings::isValidCIPPath(gcString path)
{
	gcString appDir = UTIL::OS::getCurrentDir();
	return UTIL::FS::isValidFolder(path) && UTIL::FS::Path(appDir) != UTIL::FS::Path(path);
}

gcWString DesuraJSSettings::browseCIPPath(gcWString name, gcWString path)
{
	wxDirDialog p(g_pMainApp->getMainWindow(), gcWString(Managers::GetString(L"#CIP_BROWSE"), name), path, wxDIRP_DIR_MUST_EXIST);

	if (p.ShowModal() == wxID_OK)
		return gcWString(p.GetPath().c_str().AsWChar());

	return path;
}

bool DesuraJSSettings::isValidLinkBinary(gcString path)
{
	if (!UTIL::FS::isValidFile(path))
		return false;

#ifdef WIN32
	return (path.find_last_of(".exe") == path.size()-1);
#else
	char magicBytes[5] = {0};

	try
	{
		UTIL::FS::FileHandle fh(path.c_str(), UTIL::FS::FILE_READ);
		fh.read(magicBytes, 5);
	}
	catch (...)
	{
		return false;
	}

	return UTIL::OS::getFileType(magicBytes, 5) != UTIL::OS::BinType::UNKNOWN;
#endif
}

gcString DesuraJSSettings::browseLinkBinary(gcString name, gcString path)
{
	wxFileDialog p(g_pMainApp->getMainWindow(), gcWString(Managers::GetString(L"#CIP_LINK_BROWSE"), name), path);

	if (p.ShowModal() == wxID_OK)
		return gcString(p.GetPath().c_str().AsWChar());

	return path;
}
