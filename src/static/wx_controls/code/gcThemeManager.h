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
	gcImageHandle getImageHandle(const char* path) override;
	wxBitmap getSprite(wxImage& img, const char* spriteId, const char* spriteName) override;

	const char* getThemeFolder() override;
	const char* getWebPage(const char* id) override;
	const char* getImage(const char* id) override;

	Color getColor(const char* name, const char* id) override;

	gcRefPtr<SpriteRectI> getSpriteRect(const char* id, const char* rectId) override;

	void loadFromFolder(const char* folder) override;
	void loadTheme(const char* name) override;

	//make sure to destroy them
	void getThemeStubList(std::vector<ThemeStubI*> &vList) override;

protected:
	void newImgHandle(uint32 hash);
	void desposeImgHandle(uint32 hash);

	std::mutex m_WaitMutex;
	friend class gcImageHandle;
};


#endif //DESURA_GCTHEMEMANAGER_H
