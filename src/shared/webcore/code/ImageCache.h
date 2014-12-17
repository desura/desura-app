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

#ifndef DESURA_IMAGECACHE_H
#define DESURA_IMAGECACHE_H
#ifdef _WIN32
#pragma once
#endif

#include "util_thread/BaseThread.h"

class ImageCache
{
public:
	void init(const char* appDataPath);

	void loadFromDb();
	void saveToDb();

	void updateImagePath(const char* path, uint32 hash);
	gcString getImagePath(uint32 hash);

protected:


private:
	time_t m_LastUpdateTime;
	std::map<uint32, gcString> m_mImageMap;

	std::vector<uint32> m_vUpdateList;

	std::mutex m_ImgMutex;
	gcString m_szAppDataPath;
};

#endif //DESURA_IMAGECACHE_H
