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
#include "ItemHandle.h"

#include "User.h"

DesuraId g_ElevatedGames [] = 
{
	DesuraId(196, DesuraId::TYPE_GAME),
	DesuraId(),
};




namespace UserCore
{
namespace Item
{


void ItemHandle::doLaunch(Helper::ItemLaunchHelperI* helper)
{
	preLaunchCheck();
	
	bool needElevation = false;

	size_t x=0;
	while (g_ElevatedGames[x].isOk())
	{
		if (getItemInfo()->getId() == g_ElevatedGames[x] || getItemInfo()->getParentId() == g_ElevatedGames[x])
		{
			needElevation = true;
			break;
		}

		x++;
	}
	
	UserCore::Item::Misc::ExeInfoI* ei = getItemInfo()->getActiveExe();

	gcString args;
	gcString ea(ei->getExeArgs());
	gcString ua(ei->getUserArgs());
	
	if (ea.size() > 0)
		args += " " + ea;

	if (ua.size() > 0)
		args += " " + ua;

	m_pUserCore->getItemManager()->setRecent(getItemInfo()->getId());

	auto pUserEx = m_pUserCore->getInternal();
	HWND hMainWin;

	if (pUserEx)
		hMainWin = pUserEx->getMainWindowHandle();

	bool res = UTIL::WIN::launchExe(ei->getExe(), args.c_str(), needElevation, hMainWin);

	if (!res)
		throw gcException(ERR_LAUNCH, GetLastError(), gcString("Failed to create {0} process. [{1}: {2}].\n", getItemInfo()->getName(), GetLastError(), ei->getExe()));
}

bool ItemHandle::createDesktopShortcut()
{
	gcString workingDir = UTIL::OS::getDesktopPath();
	gcString path("{0}\\{1}.lnk", workingDir, UTIL::WIN::sanitiseFileName(getItemInfo()->getName()));
	gcString link("desura://launch/{0}/{1}", getItemInfo()->getId().getTypeString(), getItemInfo()->getShortName());

	gcString icon(getItemInfo()->getIcon());

	if (UTIL::FS::isValidFile(icon))
	{
		gcString out(icon);
		out += ".ico";

		if (UTIL::MISC::convertToIco(icon.c_str(), out.c_str()))
			icon = out;
		else
			icon = "";
	}
	else
	{
		icon = "";
	}

	if (icon == "")
		icon = UTIL::OS::getCurrentDir(L"\\desura.exe");

	UTIL::FS::delFile(path);
	UTIL::WIN::createShortCut(gcWString(path).c_str(), link.c_str(), workingDir.c_str(), "", false, (icon.size()>0)?icon.c_str():nullptr);

	return UTIL::FS::isValidFile(path);
}

bool ItemHandle::createMenuEntry()
{
	return false;
}

}
}
