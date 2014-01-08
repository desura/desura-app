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

#ifndef DESURA_TOOLMANAGER_SQL_H
#define DESURA_TOOLMANAGER_SQL_H
#ifdef _WIN32
#pragma once
#endif


#define COUNT_TOOLINFO "select count(*) from sqlite_master where name='toolinfo';"
#define CREATE_TOOLINFO "create table toolinfo(	internalid INTEGER PRIMARY KEY, "	\
												"name TEXT,"						\
												"nameid TEXT,"						\
												"url TEXT,"							\
												"exe TEXT,"							\
												"args TEXT,"						\
												"flags INTEGER,"					\
												"downloadsize INTEGER,"				\
												"result INTEGER,"					\
												"hash TEXT"							\
												");"

#define TOOLINFO_DB "toolinfo.sqlite"

inline gcString getToolInfoDb(const char* appDataPath)
{
	return gcString("{0}{1}{2}", appDataPath, DIRS_STR, TOOLINFO_DB);
}

inline void createToolInfoDbTables(const char* appDataPath)
{
	sqlite3x::sqlite3_connection db(getToolInfoDb(appDataPath).c_str());

	if (db.executeint(COUNT_TOOLINFO) == 0)
		db.executenonquery(CREATE_TOOLINFO);
}

#endif //DESURA_TOOLMANAGER_H
