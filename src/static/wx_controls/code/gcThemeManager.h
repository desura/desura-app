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

#ifndef DESURA_GCTHEMEMANAGER_H
#define DESURA_GCTHEMEMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "gcManagers.h"
#include "wx_controls/gcImageHandle.h"
#include "wx_controls/gcImage.h"
#include "util_thread/BaseThread.h"
#include "BaseManager.h"



class gcImageInfo : public BaseItem
{
public: 
	gcImageInfo(const char* path, const char* name) : BaseItem(name)
	{
		m_uiRefCount = 0;
		m_pImg = new gcImage(path);
	}

	~gcImageInfo()
	{
		safe_delete(m_pImg);
	}

	void incRef()
	{
		m_uiRefCount++;
	}

	void defRef()
	{
		m_uiRefCount--;
	}

	bool isEmpty()
	{
		return (m_uiRefCount == 0);
	}

	gcImage* getImg()
	{
		return m_pImg;
	}

private:
	uint32 m_uiRefCount;
	gcImage* m_pImg;
};


class gcThemeManager : public BaseManager<gcImageInfo>, public gcThemeManagerI
{
public:
	gcThemeManager();
	~gcThemeManager();

	//gets image handle. Alpha refers to if the image is the alpha version
	virtual gcImageHandle getImageHandle(const char* path);
	virtual wxBitmap getSprite(wxImage& img, const char* spriteId, const char* spriteName);

	virtual const char* getThemeFolder();
	virtual const char* getWebPage(const char* id);
	virtual const char* getImage(const char* id);

	virtual Color getColor(const char* name, const char* id);

	virtual SpriteRectI* getSpriteRect(const char* id, const char* rectId);

	virtual void loadFromFolder(const char* folder);
	virtual void loadTheme(const char* name);

	//make sure to destroy them
	virtual void getThemeStubList(std::vector<ThemeStubI*> &vList);

protected:
	void newImgHandle(uint32 hash);
	void desposeImgHandle(uint32 hash);

	std::mutex m_WaitMutex;
	friend class gcImageHandle;
};


#endif //DESURA_GCTHEMEMANAGER_H
