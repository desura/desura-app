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
#include "LanguageManager.h"


#include <algorithm>
#include <string.h>
#include "XMLMacros.h"

#include <branding/branding.h>


LanguageManager::LanguageManager() 
	: BaseManager<LanguageString>( true )
{
	// load english files by default
	std::wstring path = UTIL::OS::getDataPath(L"language/english.xml");
	loadFromFile(UTIL::STRING::toStr(path).c_str());
}

LanguageManager::~LanguageManager()
{
}


const char* LanguageManager::getString(const char* name)
{
	LanguageString* temp = findItem(name);
		
	if (temp)
	{
		if (temp->str.size() == 0 && temp->ustr.size() > 0)
			temp->str = temp->ustr;

		return temp->str.c_str();
	}

	return name;
}

const wchar_t* LanguageManager::getString(const wchar_t* name)
{
	gcString n(name);

	LanguageString* temp = findItem(n.c_str());
		
	if (temp)
		return temp->ustr.c_str();
	
	return name;
}


bool LanguageManager::loadFromFile(const char* file)
{
	XML::gcXMLDocument doc(file);

	if (!doc.IsValid())
		return false;

	auto cNode = doc.GetRoot("lang");

	if (!cNode.IsValid())
		return false;

	auto parseString = [this](const XML::gcXMLElement &xmlChild)
	{
		const std::string name = xmlChild.GetAtt("name");
		const std::string val = xmlChild.GetText();

		if (name.empty() || val.empty())
			return;

		LanguageString* temp = dynamic_cast<LanguageString*>(this->BaseManager::findItem(name.c_str()));

		if (!temp)
		{
			temp = new LanguageString(name.c_str());
			this->addItem( temp );
		}

#ifdef DESURA_OFFICIAL_BUILD
		temp->ustr = val;
#else
		std::vector<std::string> res;
		UTIL::STRING::tokenize(gcString(val), res, "Desura");

		gcString out;

		for (size_t x=0; x<res.size(); x++)
		{
			out += res[x];

			if (x+1 != res.size())
				out += PRODUCT_NAME;
		}

		temp->ustr = out;
#endif
	};

#ifdef WIN32
	const char* szPlatform = "windows";
#else
	const char* szPlatform = "linux";
#endif

	cNode.FirstChildElement("strings").for_each_child("str", parseString);
	cNode.FirstChildElement(szPlatform).for_each_child("str", parseString);
	return true;
}

void LanguageManager::unloadAll()
{
	removeAll();
}
