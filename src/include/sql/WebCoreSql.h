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

#ifndef DESURA_WEBCORE_SQL_H
#define DESURA_WEBCORE_SQL_H
#ifdef _WIN32
#pragma once
#endif

#define CREATE_NAMECACHE "create table namecache(internalid INTEGER PRIMARY KEY, nameid INTEGER, hashid INTEGER, ttl DATE);"
#define COUNT_NAMECACHE "select count(*) from sqlite_master where name='namecache';"

#define CREATE_IMAGECACHE "create table imagecache(hash INTEGER PRIMARY KEY, path STRING, ttl DATE);"
#define COUNT_IMAGECACHE "select count(*) from sqlite_master where name='imagecache';"

#define WEBCORE_DB "webcore_cache.sqlite"

inline gcString getWebCoreDb(const char* appDataPath)
{
	return gcString("{0}{2}{1}", appDataPath, WEBCORE_DB, DIRS_STR);
}

inline void createWebCoreDbTables(const char* appDataPath)
{
	sqlite3x::sqlite3_connection db(getWebCoreDb(appDataPath).c_str());

	if (db.executeint(COUNT_NAMECACHE) == 0)
		db.executenonquery(CREATE_NAMECACHE);

	if (db.executeint(COUNT_IMAGECACHE) == 0)
		db.executenonquery(CREATE_IMAGECACHE);
}

#endif //DESURA_WEBCORE_SQL_H
