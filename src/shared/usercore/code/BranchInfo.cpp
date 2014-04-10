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
#include "BranchInfo.h"

#include "sqlite3x.hpp"
#include "XMLMacros.h"

#include "BranchInstallInfo.h"

#ifdef WIN32
  #include <Wincrypt.h>
#endif

using namespace UserCore::Item;

BranchInfo::BranchInfo(MCFBranch branchId, DesuraId itemId, BranchInstallInfo* bii, uint32 platformId, uint32 userid)
	: m_InstallInfo(bii)
	, m_ItemId(itemId)
	, m_uiBranchId(branchId)
	, m_uiUserId(userid)
	, m_uiFlags(0)
{
	if (platformId != 0)
	{
		switch (platformId)
		{
		default:
		case 100:
			m_uiFlags |= BF_WINDOWS_32;
			break;
		case 110:
			m_uiFlags |= BF_LINUX_32;
			break;
		case 120:
			m_uiFlags |= BF_LINUX_64;
			break;
		};
	}
}

BranchInfo::~BranchInfo()
{
}

bool BranchInfo::isWindows()
{
	//default to windows
	bool res = HasAnyFlags(m_uiFlags, BF_WINDOWS_32|BF_WINDOWS_64) || (!isLinux() && !isMacOsX());
	return res;
}

bool BranchInfo::isLinux()
{
	bool res = HasAnyFlags(m_uiFlags, BF_LINUX_32|BF_LINUX_64);
	return res;
}

bool BranchInfo::isMacOsX()
{
	return HasAnyFlags(m_uiFlags, BF_MACOSX);
}

bool BranchInfo::is32Bit()
{
	//default to 32 bit
	bool res = HasAnyFlags(m_uiFlags, BF_WINDOWS_32|BF_LINUX_32) || isMacOsX() || !is64Bit();
	return res;
}

bool BranchInfo::is64Bit()
{
	bool res = HasAnyFlags(m_uiFlags, BF_WINDOWS_64|BF_LINUX_64) || isMacOsX();
	return res;
}

bool BranchInfo::isAvaliable()
{
	return HasAnyFlags(m_uiFlags, BF_FREE|BF_ONACCOUNT|BF_DEMO|BF_TEST) && !(HasAnyFlags(m_uiFlags, BF_REGIONLOCK|BF_MEMBERLOCK) && !HasAnyFlags(m_uiFlags, BF_ONACCOUNT));
}

bool BranchInfo::isDownloadable()
{
	return isAvaliable() && !HasAnyFlags(m_uiFlags, BF_NORELEASES);
}

bool BranchInfo::isPreOrder()
{
	return isAvaliable() && HasAllFlags(m_uiFlags, BF_PREORDER);
}

bool BranchInfo::isPreOrderAndNotPreload()
{
	return isAvaliable() && HasAllFlags(m_uiFlags, BF_PREORDER | BF_NORELEASES);
}

bool BranchInfo::hasAcceptedEula()
{
	return (m_szEulaUrl.size() == 0 || HasAnyFlags(m_uiFlags, BF_ACCEPTED_EULA));
}

bool BranchInfo::hasCDKey()
{
	return HasAllFlags(m_uiFlags, BF_CDKEY);
}

bool BranchInfo::isSteamGame()
{
	return HasAllFlags(m_uiFlags, BF_STEAMGAME);
}

void BranchInfo::acceptEula()
{
	m_uiFlags |= BF_ACCEPTED_EULA;
}

void BranchInfo::deleteFromDb(sqlite3x::sqlite3_connection* db)
{
	try
	{
		sqlite3x::sqlite3_command cmd(*db, "DELETE FROM branchinfo WHERE branchid=? AND internalid=?;");
		cmd.bind(1, (int)m_uiBranchId);
		cmd.bind(2, (long long int)m_ItemId.toInt64());
		cmd.executenonquery();
	}
	catch (...)
	{
	}

	try
	{
		sqlite3x::sqlite3_command cmd(*db, "DELETE FROM tools WHERE branchid=?;");
		cmd.bind(1, (int)m_uiBranchId);
		cmd.executenonquery();
	}
	catch (...)
	{
	}
}

void BranchInfo::saveDb(sqlite3x::sqlite3_connection* db)
{
	saveDbFull(db);
}

void BranchInfo::saveDbFull(sqlite3x::sqlite3_connection* db)
{
	{
		sqlite3x::sqlite3_command cmd(*db, "REPLACE INTO branchinfo VALUES (?,?,?,?,?, ?,?,?,?,?, ?,?);");

		cmd.bind(1, (int) m_uiBranchId);
		cmd.bind(2, (long long int)m_ItemId.toInt64());
		cmd.bind(3, m_szName);				//name
		cmd.bind(4, (int) m_uiFlags);		//flags
		cmd.bind(5, m_szEulaUrl);

		cmd.bind(6, m_szEulaDate);
		cmd.bind(7, m_szPreOrderDate);
		cmd.bind(8, "");
		cmd.bind(9, UTIL::OS::getRelativePath(m_szInstallScript));
		cmd.bind(10, (int) m_uiInstallScriptCRC);

		cmd.bind(11, (int) m_uiGlobalId);
		cmd.bind(12, (int) m_InstallInfo->getBiId());

		cmd.executenonquery();
	}

	{
		sqlite3x::sqlite3_command cmd(*db, "DELETE from cdkey WHERE branchid=? and userid=?;");
		cmd.bind(1, (int)m_uiBranchId);
		cmd.bind(2, (int)m_uiUserId);
		cmd.executenonquery();
	}

	{
		sqlite3x::sqlite3_command cmd(*db, "INSERT INTO cdkey VALUES (?,?,?);");

		std::lock_guard<std::mutex> guard(m_BranchLock);

		for (auto key : m_vCDKeyList)
		{
			cmd.bind(1, (int)m_uiBranchId);
			cmd.bind(2, (int)m_uiUserId);
			cmd.bind(3, encodeCDKey(key));

			cmd.executenonquery();
		}
	}



	{
		sqlite3x::sqlite3_command cmd(*db, "DELETE from tools WHERE branchid=?;");
		cmd.bind(1, (int)m_uiBranchId);
		cmd.executenonquery();
	}

	{
		sqlite3x::sqlite3_command cmd(*db, "INSERT INTO tools VALUES (?,?);");

		for (auto tool : m_vToolList)
		{
			cmd.bind(1, (int)m_uiBranchId);
			cmd.bind(2, (long long int)tool.toInt64());
			cmd.executenonquery();
		}
	}
}

void BranchInfo::loadDb(sqlite3x::sqlite3_connection* db)
{
	if (!db)
		return;

	sqlite3x::sqlite3_command cmd(*db, "SELECT * FROM branchinfo WHERE branchid=? AND internalid=?;");
	cmd.bind(1, (int)m_uiBranchId);
	cmd.bind(2, (long long int)m_ItemId.toInt64());

	sqlite3x::sqlite3_reader reader = cmd.executereader();
	
	reader.read();

	m_szName		= gcString(reader.getstring(2));		//name
	m_uiFlags		= reader.getint(3);						//flags
	m_szEulaUrl		= reader.getstring(4);
	m_szEulaDate	= reader.getstring(5);
	m_szPreOrderDate = reader.getstring(6);
	//7 is normally cd key but is moved to new table
	m_szInstallScript = UTIL::OS::getAbsPath(reader.getstring(8));
	m_uiInstallScriptCRC = reader.getint(9);
	m_uiGlobalId = MCFBranch::BranchFromInt(reader.getint(10), true);

	m_uiFlags &= ~BF_ONACCOUNT;

	{
		sqlite3x::sqlite3_command cmd(*db, "SELECT * FROM tools WHERE branchid=?;");
		cmd.bind(1, (int)m_uiBranchId);
		sqlite3x::sqlite3_reader reader = cmd.executereader();
	
		while (reader.read())
			m_vToolList.push_back(DesuraId(reader.getint64(1)));
	}

	{
		sqlite3x::sqlite3_command cmd(*db, "SELECT key FROM cdkey WHERE branchid=? and userid=?;");
		cmd.bind(1, (int)m_uiBranchId);
		cmd.bind(2, (int)m_uiUserId);
		sqlite3x::sqlite3_reader reader = cmd.executereader();

		std::lock_guard<std::mutex> guard(m_BranchLock);

		while (reader.read())
		{
			auto key = decodeCDKey(reader.getstring(0));

			if (!key.empty())
				m_vCDKeyList.push_back(key);
		}
			
	}
}


void BranchInfo::loadXmlData(const XML::gcXMLElement &xmlNode)
{
	xmlNode.GetChild("name", m_szName);
	xmlNode.GetChild("price", m_szCost);
	xmlNode.GetChild("eula", m_szEulaUrl);

	auto eNode = xmlNode.FirstChildElement("eula");

	if (eNode.IsValid())
	{
		const std::string date = eNode.GetAtt("date");

		if (!date.empty() && m_szEulaDate != date)
		{
			m_uiFlags &= ~BF_ACCEPTED_EULA; 
			m_szEulaDate = date;
		}
	}

	gcString preload;
	xmlNode.GetChild("preload", preload);

	if (m_szPreOrderDate.size() > 0 && (preload.size() == 0 || preload == "0"))
	{
		m_szPreOrderDate = "";
		m_uiFlags &= ~BF_PREORDER;

		onBranchInfoChangedEvent();
	}
	else if (preload != "0")
	{
		m_szPreOrderDate = preload;
		m_uiFlags |= BF_PREORDER;

		onBranchInfoChangedEvent();
	}


	bool nameon = false;
	bool free = false;
	bool onaccount = false;
	bool regionlock = false;
	bool memberlock = false;
	bool demo = false;
	bool test = false;
	bool cdkey = false;
	gcString cdkeyType;

	xmlNode.GetChild("nameon", nameon);
	xmlNode.GetChild("free", free);
	xmlNode.GetChild("onaccount", onaccount);
	xmlNode.GetChild("regionlock", regionlock);
	xmlNode.GetChild("inviteonly", memberlock);
	xmlNode.GetChild("demo", demo);
	xmlNode.GetChild("test", test);
	xmlNode.GetChild("cdkey", cdkey);
	xmlNode.FirstChildElement("cdkey").GetAtt("type", cdkeyType);

	uint32 global = -1;
	xmlNode.GetChild("global", global);

	if (global != -1)
		m_uiGlobalId = MCFBranch::BranchFromInt(global, true);

	if (nameon)
		m_uiFlags |= BF_DISPLAY_NAME;

	if (free)
		m_uiFlags |= BF_FREE;

	if (onaccount)
		m_uiFlags |= BF_ONACCOUNT;

	if (regionlock)
		m_uiFlags |= BF_REGIONLOCK;

	if (memberlock)
		m_uiFlags |= BF_MEMBERLOCK;

	if (demo)
		m_uiFlags |= BF_DEMO;

	if (test)
		m_uiFlags |= BF_TEST;

	if (cdkey)
		m_uiFlags |= BF_CDKEY;

	if (cdkeyType == "steam")
		m_uiFlags |= BF_STEAMGAME;

	//no mcf no release
	auto mcfNode = xmlNode.FirstChildElement("mcf");
	if (!mcfNode.IsValid())
	{
		m_uiFlags |= BF_NORELEASES;
	}
	else
	{
		uint32 build = -1;
		mcfNode.GetChild("build", build);

		m_uiLatestBuild = MCFBuild::BuildFromInt(build);
	}

	auto toolsNode = xmlNode.FirstChildElement("tools");

	if (toolsNode.IsValid())
	{
		m_vToolList.clear();

		toolsNode.for_each_child("tool", [this](const XML::gcXMLElement &xmlTool)
		{
			const std::string id = xmlTool.GetText();

			if (!id.empty())
				m_vToolList.push_back(DesuraId(id.c_str(), "tools"));
		});
	}

	auto scriptNode = xmlNode.FirstChildElement("installscript");

	if (scriptNode.IsValid())
		processInstallScript(scriptNode);
}

void BranchInfo::processInstallScript(const XML::gcXMLElement &xmlElement)
{
	uint32 crc = 0;
	xmlElement.GetAtt("crc", crc);

	if (UTIL::FS::isValidFile(m_szInstallScript))
	{
		if (crc != 0 && m_uiInstallScriptCRC == (uint32)crc)
			return;
	}
	else
	{
		m_szInstallScript = UTIL::OS::getAppDataPath(gcWString(L"{0}\\{1}\\install_script.js", m_ItemId.getFolderPathExtension(), getBranchId()).c_str());
	}

	gcString base64 = xmlElement.GetText();

	try
	{
		UTIL::FS::recMakeFolder(UTIL::FS::Path(m_szInstallScript, "", true));
		UTIL::FS::FileHandle fh(m_szInstallScript.c_str(), UTIL::FS::FILE_WRITE);
		UTIL::STRING::base64_decode(base64, [&fh](const unsigned char* data, uint32 size) -> bool
		{
			fh.write((const char*)data, size);
			return true;
		});

		m_uiInstallScriptCRC = crc;
	}
	catch (gcException &e)
	{
		Warning("Failed to save install script for {0} branch {1}: {2}\n", m_ItemId.toInt64(), m_uiBranchId, e);
		m_szInstallScript = "";
	}
}

void BranchInfo::getToolList(std::vector<DesuraId> &toolList)
{
	toolList = m_vToolList;
}

gcString BranchInfo::encodeCDKey(const gcString& strRawKey)
{
	if (strRawKey.empty())
		return "";

#ifdef WIN32
	std::string reg = UTIL::OS::getConfigValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Cryptography\\MachineGuid", true);
	gcString key("{0}_{1}", reg, m_ItemId.toInt64());
#else
	gcString key("{0}", m_ItemId.toInt64());
#endif

	return UTIL::OS::UserEncodeString(key, strRawKey);
}

gcString BranchInfo::decodeCDKey(const gcString& strEncodedKey)
{
	if (strEncodedKey.empty())
		return "";
	
#ifdef WIN32
	std::string reg = UTIL::OS::getConfigValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Cryptography\\MachineGuid", true);
	gcString key("{0}_{1}", reg, m_ItemId.toInt64());
#else
	gcString key("{0}", m_ItemId.toInt64());
#endif

	return UTIL::OS::UserDecodeString(key, strEncodedKey);
}

void BranchInfo::setCDKey(gcString key)
{
	std::lock_guard<std::mutex> guard(m_BranchLock);

	if (std::find(begin(m_vCDKeyList), end(m_vCDKeyList), key) == end(m_vCDKeyList))
	{
		m_vCDKeyList.push_back(key);
		onBranchCDKeyChangedEvent();
	}
}

bool BranchInfo::isCDKeyValid()
{
	std::lock_guard<std::mutex> guard(m_BranchLock);
	return !m_vCDKeyList.empty();
}

DesuraId BranchInfo::getItemId()
{
	return m_ItemId;
}

void BranchInfo::addJSTool(DesuraId toolId)
{
	for (size_t x=0; x<m_vToolList.size(); x++)
	{
		if (m_vToolList[x] == toolId)
			return;
	}

	m_vToolList.push_back(toolId);
}

void BranchInfo::setLinkInfo(const char* name)
{
	m_szName = name;
	m_uiFlags = BF_FREE;

#ifdef WIN32
	m_uiFlags |= BF_WINDOWS_32|BF_WINDOWS_64;
#else
	m_uiFlags |= BF_LINUX_32|BF_LINUX_64;
#endif
}

BranchInstallInfo* BranchInfo::getInstallInfo()
{
	return m_InstallInfo;
}

void BranchInfo::getCDKey(std::vector<gcString> &vKeys) const
{
	std::lock_guard<std::mutex> guard(m_BranchLock);
	vKeys = m_vCDKeyList;
}


#ifdef WITH_GTEST

#include <gtest/gtest.h>
#include "sql/ItemInfoSql.h"
#include "ItemInfo.h"

namespace UnitTest
{
	class StubBranchItemInfo2 : public BranchItemInfoI
	{
	public:
		DesuraId getId() override
		{
			return DesuraId("2", "games");
		}

		uint32 getStatus() override
		{
			return m_nStatus;
		}

		uint32 m_nStatus = 0;
	};

	TEST(BranchInfo, CDKeyPerUser)
	{
		StubBranchItemInfo2 bi;
		BranchInstallInfo info(1, &bi, UTIL::FS::g_pDefaultUTILFS);

		BranchInfo a(MCFBranch::BranchFromInt(1), DesuraId("2", "games"), &info, 0, 123);
		BranchInfo b(MCFBranch::BranchFromInt(1), DesuraId("2", "games"), &info, 0, 123);
		BranchInfo c(MCFBranch::BranchFromInt(1), DesuraId("2", "games"), &info, 0, 456);

		a.setCDKey("A Test CD Key");

		{
			std::vector<gcString> vCDKeys;
			a.getCDKey(vCDKeys);

			ASSERT_EQ(1, vCDKeys.size());
			ASSERT_STREQ("A Test CD Key", vCDKeys[0].c_str());
		}


		sqlite3x::sqlite3_connection db(":memory:");
		createItemInfoDbTables(db);


		a.saveDbFull(&db);
		b.loadDb(&db);
		c.loadDb(&db);

		{
			std::vector<gcString> vCDKeys;
			b.getCDKey(vCDKeys);

			ASSERT_EQ(1, vCDKeys.size());
			ASSERT_STREQ("A Test CD Key", vCDKeys[0].c_str());
		}

		{
			std::vector<gcString> vCDKeys;
			c.getCDKey(vCDKeys);

			ASSERT_EQ(0, vCDKeys.size());
		}
	}
}


#endif