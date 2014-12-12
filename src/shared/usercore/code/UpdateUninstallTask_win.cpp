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
#include "UpdateUninstallTask_win.h"

#include "usercore/UserCoreI.h"
#include "usercore/ItemManagerI.h"
#include "service_pipe/IPCServiceMain.h"
#include "user.h"

using namespace UserCore;


void UpdateUninstallTask::doTask()
{
	auto service = m_pUser->getInternal()->getServiceMain();
	auto itemManager = m_pUser->getItemManager();

	gcAssert(service && itemManager);

	std::vector<gcRefPtr<UserCore::Item::ItemInfoI>> aList;
	itemManager->getAllItems(aList);

	std::vector<int64> vUninstallIds;
	getAllUninstallIds(vUninstallIds);

	for (auto i : aList)
	{
		if (!i->isInstalled())
			continue;

		auto id = i->getId().toInt64();
		auto it = std::find(begin(vUninstallIds), end(vUninstallIds), id);
		auto found = it != end(vUninstallIds);

		if (found)
			vUninstallIds.erase(it);
		else
			service->setUninstallRegKey(id, i->getInstallSize());
	}

	for (auto id : vUninstallIds)
	{
		service->removeUninstallRegKey(id);
	}
}

void UpdateUninstallTask::getAllUninstallIds(std::vector<int64> &vIds)
{
	std::vector<std::string> vRegKeys;
	UTIL::WIN::getAllRegKeys("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall", vRegKeys);

	for (auto key : vRegKeys)
	{
		size_t pos = key.find("Desura_");

		if (pos != 0)
			continue;

		std::string szId = key.substr(7, -1);

		DesuraId id(Safe::atoll(szId.c_str()));

		if (!id.isOk())
			continue;

		vIds.push_back(id.toInt64());
	}
}
