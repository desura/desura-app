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
