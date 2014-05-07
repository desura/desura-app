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

#include <branding/branding.h>
#include "sqlite3x.hpp"
#include "sql/ItemInfoSql.h"

gcFixedString<255> g_szAppDataPath;

void UnInstallRegKey_SetAppDataPath(const char* path)
{
	g_szAppDataPath = path;
}

class UninstallInfo
{
public:
	DesuraId id;

	gcString version;
	gcString displayName;
	gcString installDir;
	gcString icon;
	gcString developer;
	gcString profile;

	uint32 build;
	uint32 branch;
};

void CreateIco(DesuraId id, std::string &icon)
{
	std::string desuraInstallPath = UTIL::OS::getConfigValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\Desura\\DesuraApp\\InstallPath");

	UTIL::FS::Path path(desuraInstallPath, "", false);

	if (!UTIL::FS::isValidFolder(path))
		path = UTIL::FS::Path(g_szAppDataPath.c_str(), "", false);

	path += "icons";
	path += UTIL::FS::File(gcString("{0}.ico", id.toInt64()));

	UTIL::FS::recMakeFolder(path);

	if (UTIL::MISC::convertToIco(icon, path.getFullPath()))
		icon = path.getFullPath();
}


bool DoGetUninstallInfo(DesuraId id, UninstallInfo &info)
{
	sqlite3x::sqlite3_connection db(getItemInfoDb(g_szAppDataPath.c_str()).c_str());

	{
		sqlite3x::sqlite3_command cmd(db, "SELECT developer, name, profile, icon, ibranch FROM iteminfo WHERE internalid=?;");
		cmd.bind(1, (long long int)id.toInt64());
		sqlite3x::sqlite3_reader reader = cmd.executereader();

		reader.read();

		info.developer = gcString(reader.getstring(0));
		info.displayName = gcString(reader.getstring(1));
		info.profile = gcString(reader.getstring(2));
		info.icon = gcString(reader.getstring(3));
		info.branch = MCFBranch::BranchFromInt(reader.getint(4));
	}

	int biid = 0;

	{
		sqlite3x::sqlite3_command cmd(db, "SELECT biid FROM branchinfo WHERE branchid=?;");
		cmd.bind(1, (int)info.branch);
		biid = cmd.executeint();
	}

	{
		sqlite3x::sqlite3_command cmd(db, "SELECT installpath, iprimpath, ibuild FROM installinfo WHERE itemid=? AND biid=?;");
		cmd.bind(1, (long long int)id.toInt64());
		cmd.bind(2, (int)biid);

		sqlite3x::sqlite3_reader reader = cmd.executereader();
		reader.read();

		info.installDir = reader.getstring(0);
		gcString primePath = reader.getstring(1);
		info.build = MCFBuild::BuildFromInt(reader.getint(2));

		if (primePath.size() > 0)
			info.installDir = primePath;
	}

	if (info.branch == 0 || info.build == 0)
		return false;

	if (UTIL::FS::isValidFile(info.icon))
		CreateIco(id, info.icon);

	{
		sqlite3x::sqlite3_command cmd(db, "SELECT name FROM branchinfo WHERE branchid=? AND internalid=?;");
		cmd.bind(1, (int)info.branch);
		cmd.bind(2, (long long int)id.toInt64());

		sqlite3x::sqlite3_reader reader = cmd.executereader();

		reader.read();
		info.version = gcString(reader.getstring(0));
	}
}

bool GetUninstallInfo(DesuraId id, UninstallInfo &info)
{
	info.id = id;

	uint32 nTryCount = 0;

	while (true)
	{
		try
		{
			DoGetUninstallInfo(id, info);
			break;
		}
		catch (std::exception &e)
		{
			if (nTryCount < 3)
			{
				++nTryCount;
				gcSleep(500);
			}		
			else
			{
				WarningS("Failed to get item {1} for uninstall update: {0}\n", e.what(), id.toInt64());
				break;
			}
		}
	};

	return true;
}

void SetUninstallRegKey(UninstallInfo &info, uint64 installSize)
{
	gcString base("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Desura_{0}\\", info.id.toInt64());

	gcString name		= base + "DisplayName";
	gcString version	= base + "DisplayVersion";
	gcString publisher	= base + "Publisher";

	gcString vMinor		= base + "VersionMinor";
	gcString vMajor		= base + "VersionMajor";

	gcString about		= base + "URLInfoAbout";
	gcString help		= base + "HelpLink";
	gcString path		= base + "InstallLocation";
	gcString size		= base + "EstimatedSize";

	gcString uninstall	= base + "UninstallString";
	gcString verify		= base + "ModifyPath";
	gcString noModify	= base + "NoModify";

	gcString company	= base + "RegCompany";
	gcString icon		= base + "DisplayIcon";
	gcString date		= base + "InstallDate";

	time_t rawtime;
	struct tm timeinfo;

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	gcString today("{0}{1}{2}", (1900 + timeinfo.tm_year), timeinfo.tm_mon+1, timeinfo.tm_mday);

	std::string desuraExe = UTIL::OS::getConfigValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\Desura\\DesuraApp\\InstallPath") + "\\desura.exe";

	if (!UTIL::FS::isValidFile(info.icon))
		info.icon = desuraExe;

	gcString uninstallExe("\"{0}\" desura://uninstall/{1}/{2}", desuraExe, info.id.getTypeString(), info.id.getItem());
	gcString verifyExe("\"{0}\" desura://verify/{1}/{2}", desuraExe, info.id.getTypeString(), info.id.getItem());

	UTIL::OS::setConfigValue(name, PRODUCT_NAME ": " + info.displayName);
	UTIL::OS::setConfigValue(version, info.version);
	UTIL::OS::setConfigValue(publisher, info.developer);

	UTIL::OS::setConfigValue(vMinor, info.build);
	UTIL::OS::setConfigValue(vMajor, info.branch);

	UTIL::OS::setConfigValue(about, info.profile);
	UTIL::OS::setConfigValue(help, "http://www.desura.com/groups/desura/forum");
	UTIL::OS::setConfigValue(path, info.installDir);

	if (installSize != 0)
		UTIL::OS::setConfigValue(size, (uint32)(installSize/1024));

	UTIL::OS::setConfigValue(uninstall, uninstallExe, true);
	UTIL::OS::setConfigValue(verify, verifyExe, true);
	UTIL::OS::setConfigValue(noModify, 1);

	UTIL::OS::setConfigValue(company, info.developer);
	UTIL::OS::setConfigValue(icon, info.icon);

	UTIL::OS::setConfigValue(date, today);
}

void RemoveUninstallRegKey(DesuraId id)
{
	gcString regKey("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Desura_{0}", id.toInt64());
	UTIL::WIN::delRegTree(regKey.c_str());
}

bool SetUninstallRegKey(DesuraId id, uint64 installSize)
{
	UninstallInfo info;

	if (!GetUninstallInfo(id, info))
		return false;

	SetUninstallRegKey(info, installSize);
	return true;
}