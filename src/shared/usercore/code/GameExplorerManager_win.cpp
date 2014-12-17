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

#include "Common.h"
#include "GameExplorerManager.h"

#include "usercore\UserCoreI.h"
#include "IPCServiceMain.h"

#include "sqlite3x.hpp"
#include "sql\GameExplorerSql.h"

#include "user.h"

using namespace UserCore;

GameExplorerManager::GameExplorerManager(gcRefPtr<UserI> user)
	: BaseManager<Misc::GameExplorerInfo>()
	, m_pUser(user)
{
	try
	{
		createGameExplorerDbTables(m_pUser->getAppDataPath());
	}
	catch (std::exception &e)
	{
		Warning("Failed to create game explorer db: {0}\n", e.what());
	}
}

bool GameExplorerManager::shouldInstallItems()
{
	gcString disable(m_pUser->getCVarValue("gc_disable_wge"));
	return (disable == "false" || disable == "0");
}

void GameExplorerManager::addItem(DesuraId item)
{
	if (!shouldInstallItems())
		return;

	if (!item.isOk())
		return;

	auto gei = BaseManager::findItem(item.toInt64());

	if (!gei)
	{
		gei = gcRefPtr<Misc::GameExplorerInfo>::create(item, m_pUser);
		BaseManager<Misc::GameExplorerInfo>::addItem(gei);
	}

	if (!gei->isInstallable() || !gei->needsInstall())
		return;

	gei->generateDll();
	gei->installDll();
}

void GameExplorerManager::removeItem(DesuraId item)
{
	auto gei = BaseManager::findItem(item.toInt64());

	if (gei)
		gei->removeDll();
}

void GameExplorerManager::loadItems()
{
	if (m_pUser->getInternal()->getServiceMain())
		m_pUser->getInternal()->getServiceMain()->addDesuraToGameExplorer();

	sqlite3x::sqlite3_connection db(getGameExplorerDb(m_pUser->getAppDataPath()).c_str());

	try
	{
		if (db.executeint(COUNT_GEINFO) != 0)
		{
			sqlite3x::sqlite3_command cmd(db, "SELECT internalid FROM gameexplorer;");
			sqlite3x::sqlite3_reader reader = cmd.executereader();

			while(reader.read())
			{
				DesuraId id(reader.getint64(0));
				BaseManager<Misc::GameExplorerInfo>::addItem(gcRefPtr<Misc::GameExplorerInfo>::create(id, m_pUser));
			}
		}
	}
	catch (std::exception &e)
	{
		Warning("Failed to load game explorer db: {0}\n", e.what());
		return;
	}

	std::vector<gcRefPtr<UserCore::Item::ItemInfoI>> vList;
	m_pUser->getItemManager()->getAllItems(vList);

	bool shouldInstall = shouldInstallItems();

	for (size_t x=0; x<vList.size(); x++)
	{
		auto gei = BaseManager::findItem(vList[x]->getId().toInt64());

		if (!gei)
		{
			gei = gcRefPtr<Misc::GameExplorerInfo>::create(vList[x]->getId(), m_pUser);
			BaseManager<Misc::GameExplorerInfo>::addItem(gei);
		}

		try
		{
			if (gei->isInstallable())
			{
				if (shouldInstall && gei->needsInstall())
				{
					gei->generateDll();
					gei->installDll();
				}
			}
			else
			{
				gei->generateDll();
				gei->removeDll();
			}
		}
		catch (gcException &e)
		{
			Warning("Failed to generate dll for game explorer: {0}", e);
		}
	}
}

void GameExplorerManager::saveItems()
{
	std::string dbPath = getGameExplorerDb(m_pUser->getAppDataPath());

	try
	{
		sqlite3x::sqlite3_connection db(dbPath.c_str());
		sqlite3x::sqlite3_transaction trans(db);

		for (size_t x=0; x<getCount(); x++)
			getItem(x)->saveToDb(&db);

		trans.commit();
	}
	catch (std::exception &e)
	{
		Warning("Failed to save game explorer db: {0}\n", e.what());
	}
}
