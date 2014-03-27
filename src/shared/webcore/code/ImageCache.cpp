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
#include "ImageCache.h"

#include "sqlite3x.hpp"
#include "sql/WebCoreSql.h"


void ImageCache::init(const char* appDataPath)
{
	m_szAppDataPath = appDataPath;
	m_LastUpdateTime = time(nullptr);
}

void ImageCache::loadFromDb()
{
	try
	{
		sqlite3x::sqlite3_connection db(getWebCoreDb(m_szAppDataPath.c_str()).c_str());
		sqlite3x::sqlite3_command cmd(db, "SELECT * FROM imagecache WHERE ttl > DATETIME('NOW');");
		sqlite3x::sqlite3_reader reader = cmd.executereader();

		while (reader.read())
		{
			m_mImageMap[reader.getint(0)] = UTIL::OS::getAbsPath(reader.getstring(1));
		}
	}
	catch (std::exception)
	{
	}
}

void ImageCache::saveToDb()
{
	try 
	{
		std::string dbLoc = getWebCoreDb(m_szAppDataPath.c_str());
		sqlite3x::sqlite3_connection db(dbLoc.c_str());
		//sqlite3x::sqlite3_transaction trans(db);

		for (size_t x=0; x<m_vUpdateList.size(); x++)
		{
			sqlite3x::sqlite3_command cmd(db, "REPLACE INTO imagecache (path, hash, ttl) VALUES (?, ?, DATETIME('NOW', '+5 day'));");

			cmd.bind(1, UTIL::OS::getRelativePath(m_mImageMap[m_vUpdateList[x]]));
			cmd.bind(2, (int)m_vUpdateList[x]);

			cmd.executenonquery();
		}

		//trans.commit();

		m_vUpdateList.clear();
		m_LastUpdateTime = time(nullptr) + 5*60;
	}
	catch(std::exception &ex) 
	{
		Warning("Failed to update imagecache in webcore: {0}\n", ex.what());
	}
}

void ImageCache::updateImagePath(const char* path, uint32 hash)
{
	m_ImgMutex.lock();

	m_mImageMap[hash] = path;
	m_vUpdateList.push_back(hash);

	if (m_LastUpdateTime < time(nullptr))
		saveToDb();

	m_ImgMutex.unlock();
}

gcString ImageCache::getImagePath(uint32 hash)
{
	gcString res;

	m_ImgMutex.lock();

	std::map<uint32, gcString>::iterator it = m_mImageMap.find(hash);

	if (it != m_mImageMap.end())
		res = (*it).second;

	m_ImgMutex.unlock();

	return res;
}
