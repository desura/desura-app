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
#include "Theme.h"
#include "XMLMacros.h"

Theme::Theme(const char* name) 
	: ControlList(true)
	, ImageList(true)
	, SpriteList(true)
	, WebList(true)
{
	m_szName = gcString(name);
}

const char* Theme::getImage(const char* id)
{
	ThemeImageInfo* img = ImageList::findItem(id);

	if (img)
		return img->path.c_str();

	return nullptr;
}

const char* Theme::getWebPage(const char* id)
{
	ThemeWebInfo* web = WebList::findItem(id);

	if (web)
		return web->path.c_str();

	return nullptr;
}

Color Theme::getColor(const char* name, const char* id)
{
	ThemeControlInfo* col = ControlList::findItem(name);

	if (col)
		return col->getColor(name, id);

	if (strcmp(name, "default")!=0)
	{
		Warning("Cant find color {0} for control {1}, getting default\n", id, name);
		return getColor("default", id);
	}

	Warning("Cant find default color.\n");
	return Color();
}


SpriteRect* Theme::getSpriteRect(const char* id, const char* rectId)
{
	ThemeSpriteInfo* sprite = SpriteList::findItem(id);

	if (!sprite)
		return nullptr;

	return sprite->findItem(rectId);
}

void Theme::LoadImages(const UTIL::FS::Path& path, const XML::gcXMLElement &xmlEl)
{
	xmlEl.for_each_child("image", [this, &path](const XML::gcXMLElement &xmlChild)
	{
		const std::string name = xmlChild.GetAtt("name");
		const std::string val = xmlChild.GetText();

		if (name.empty() || val.empty())
			return;

		std::string outVal = UTIL::STRING::sanitizeFileName(val);
		ThemeImageInfo* img = ImageList::findItem(name.c_str());

		if (!img)
		{
			img = new ThemeImageInfo(name.c_str());
			addItem(img);
		}

		gcString fullPath("{0}{2}images{2}app{2}{1}", path.getFolderPath(), outVal, DIRS_STR);
		img->path = fullPath;
	});
}

void Theme::LoadWeb(const UTIL::FS::Path& path, const XML::gcXMLElement &xmlEl)
{
	gcString urlPath(path.getFolderPath());

	for (size_t x=0; x<urlPath.size(); x++)
	{
		if (urlPath[x] == '\\')
			urlPath[x] = '/';
	}

	xmlEl.for_each_child("page", [this, urlPath](const XML::gcXMLElement &xmlChild)
	{
		const std::string name = xmlChild.GetAtt("name");
		const std::string val = xmlChild.GetText();

		if (name.empty() || val.empty())
			return;

		std::string outVal = UTIL::STRING::sanitizeFileName(val);

#ifdef WIN32
		gcString fullPath("file:///{0}/html/{1}", urlPath, outVal);
#else
		gcString fullPath("file://{0}/html/{1}", urlPath, outVal);
#endif

		ThemeWebInfo* web = WebList::findItem(name.c_str());

		if (!web)
		{
			web = new ThemeWebInfo(name.c_str());
			addItem(web);
		}

		web->path = fullPath;
	});
}

void Theme::LoadSprites(const XML::gcXMLElement &xmlEl)
{
	xmlEl.for_each_child("sprite", [this](const XML::gcXMLElement &xmlChild)
	{
		const std::string name = xmlChild.GetAtt("name");
			
		if (name.empty())
			return;

		ThemeSpriteInfo* sprite = SpriteList::findItem(name.c_str());

		if (!sprite)
		{
			sprite = new ThemeSpriteInfo(name.c_str());
			addItem(sprite);
		}

		xmlChild.for_each_child("rect", [sprite](const XML::gcXMLElement &xmlRect)
		{
			const std::string rName = xmlRect.GetAtt("name");

			const XML::gcXMLElement pos = xmlRect.FirstChildElement("pos");
			const XML::gcXMLElement size = xmlRect.FirstChildElement("size");

			if (rName.empty() || !pos.IsValid() || !size.IsValid())
				return;

			const std::string x = pos.GetAtt("x");
			const std::string y = pos.GetAtt("y");

			const std::string w = size.GetAtt("w");
			const std::string h = size.GetAtt("h");

			if (x.empty() || y.empty() || w.empty() || h.empty())
				return;

			SpriteRect* rect = sprite->findItem(rName.c_str());
		
			if (!rect)
			{
				rect = new SpriteRect(rName.c_str());
				sprite->addItem(rect);
			}

			rect->x = Safe::atoi(x.c_str());
			rect->y = Safe::atoi(y.c_str());
			rect->w = Safe::atoi(w.c_str());
			rect->h = Safe::atoi(h.c_str());
		});
	});
}

void Theme::LoadControls(const XML::gcXMLElement &xmlEl)
{
	xmlEl.for_each_child("control", [this](const XML::gcXMLElement &xmlChild)
	{
		const std::string name = xmlChild.GetAtt("name");

		if (name.empty())
			return;
		
		ThemeControlInfo* control = ControlList::findItem(name.c_str());

		if (!control)
		{
			control = new ThemeControlInfo(name.c_str());
			addItem(control);
		}

		xmlChild.for_each_child("color", [control](const XML::gcXMLElement &xmlCol)
		{
			const std::string id = xmlCol.GetAtt("id");
			const std::string val = xmlCol.GetText();

			if (id.empty() || val.empty())
				return;

			ThemeColorInfo* col = control->findItem(id.c_str());

			if (!col)
			{
				col = new ThemeColorInfo(id.c_str());
				control->add(col);
			}

			col->color = Color(val.c_str());
		});
	});
}

void Theme::parseFile(const char* file)
{
	XML::gcXMLDocument doc(file);

	if (!doc.IsValid())
		throw gcException(ERR_BADXML);

	auto mcNode = doc.GetRoot("theme");

	if (!mcNode.IsValid())
		throw gcException(ERR_XML_NOPRIMENODE);

	mcNode.GetChild("creator", m_szDev);
	mcNode.GetChild("name", m_szPName);

	UTIL::FS::Path path(file, "", true);

	auto iNode = mcNode.FirstChildElement("images");

	if (iNode.IsValid())
		LoadImages(path, iNode);

	auto wNode = mcNode.FirstChildElement("web");

	if (wNode.IsValid())
		LoadWeb(path, wNode);

	auto sNode = mcNode.FirstChildElement("sprites");

	if (sNode.IsValid())
		LoadSprites(sNode);

	auto cNode = mcNode.FirstChildElement("controls");

	if (cNode.IsValid())
		LoadControls(cNode);

	ThemeControlInfo* control = ControlList::findItem("default");

	if (!control)
	{
		control = new ThemeControlInfo("default");

		ThemeColorInfo* col1 = new ThemeColorInfo("bg");
		ThemeColorInfo* col2 = new ThemeColorInfo("fg");

		col1->color = Color(0);
		col2->color = Color(0xFFFFFF);

		control->add(col1);
		control->add(col2);

		ControlList::addItem( control );
	}
}