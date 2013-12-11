/*
Desura is the leading indie game distribution platform
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include "Common.h"
#include "InstallInfo.h"

#include "XMLMacros.h"

namespace UserCore
{
namespace Misc
{

InstallInfo::InstallInfo(DesuraId id, DesuraId pid)
{
	m_bInstalled = false;
	m_iID = id;
	m_iParentID = pid;
}

InstallInfo::~InstallInfo()
{
}

void InstallInfo::loadXmlData(const XML::gcXMLElement &xmlNode, WildcardManager* pWildCard)
{
	WildcardManager lwc(pWildCard);

	auto wcNode = xmlNode.FirstChildElement("wcards");
	if (wcNode.IsValid())
	{
		lwc.parseXML(wcNode);
	}

	xmlNode.GetChild("name", m_szName);
	auto icsNode = xmlNode.FirstChildElement("settings").FirstChildElement("installlocations");

	if (!icsNode.IsValid())
		return;
		
	icsNode.for_each_child("installlocation", [&](const XML::gcXMLElement &xmlChild)
	{
		if (m_bInstalled)
			return;

		const std::string path = xmlChild.GetChild("path");
		const std::string check = xmlChild.GetChild("check");

		if (path.empty() || check.empty())
			return;

		char* CheckRes = NULL;
		char* PathRes = NULL;

		try
		{
			lwc.constructPath(check.c_str(), &CheckRes);
			lwc.constructPath(path.c_str(), &PathRes);

			if (CheckRes && PathRes && UTIL::FS::isValidFile(UTIL::FS::PathWithFile(CheckRes)))
			{
				m_szPath = PathRes;
				m_bInstalled = true;
			}
		}
		catch (gcException &e)
		{
			Debug(gcString("InstallInfo: Error parsing wildcards for installInfo: {0}\n", e));
		}

		safe_delete(CheckRes);
		safe_delete(PathRes);
	});
}

}
}
