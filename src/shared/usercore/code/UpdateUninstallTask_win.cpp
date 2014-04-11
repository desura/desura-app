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
#include "UpdateUninstallTask_win.h"

#include "usercore/UserCoreI.h"
#include "usercore/ItemManagerI.h"
#include "service_pipe/IPCServiceMain.h"

using namespace UserCore;


void UpdateUninstallTask::doTask()
{
	auto service = m_pUser->getServiceMain();
	auto itemManager = m_pUser->getItemManager();

	gcAssert(service && itemManager);

	std::vector<UserCore::Item::ItemInfoI*> aList;
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