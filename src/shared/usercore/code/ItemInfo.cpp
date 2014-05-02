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
#include "ItemInfo.h"

#include "XMLMacros.h"
#include "managers/WildcardManager.h"


#include "UserTasks.h"
#include "User.h"

#include "sqlite3x.hpp"
#include "sql/ItemInfoSql.h"

#include "McfManager.h"

#include "BranchInfo.h"
#include "BranchInstallInfo.h"


using namespace UserCore::Item;
namespace UM = UserCore::Item;

using namespace UserCore::Item;

template <>
std::string TraceClassInfo(ItemInfo *pClass)
{
	return gcString("pid: {0}, id: {1}, short: {2}, status: {3}", pClass->getParentId(), pClass->getId(), pClass->getShortName(), pClass->getStatus());
}


ItemInfo::ItemInfo(UserCore::UserI *user, DesuraId id, UTIL::FS::UtilFSI* pFileSystem)
	: m_pUserCore(user)
	, m_iId(id)
	, m_pFileSystem(pFileSystem)
{
}

ItemInfo::ItemInfo(UserCore::UserI *user, DesuraId id, DesuraId parid, UTIL::FS::UtilFSI* pFileSystem)
	: m_pUserCore(user)
	, m_iId(id)
	, m_iParentId(parid)
	, m_pFileSystem(pFileSystem)
{
}

ItemInfo::~ItemInfo()
{
	safe_delete(m_vBranchList);
}


void ItemInfo::deleteFromDb(sqlite3x::sqlite3_connection* db)
{
	if (!m_bAddedToDb)
		return;

	gcTrace("");

	try
	{
		sqlite3x::sqlite3_command cmd(*db, "DELETE FROM iteminfo WHERE internalid=?;");
		cmd.bind(1, (long long int)m_iId.toInt64());
		cmd.executenonquery();
	}
	catch (...)
	{
	}

	for (size_t x=0; x<m_vBranchList.size(); x++)
	{
		m_vBranchList[x]->deleteFromDb(db);
	}

	std::for_each(m_mBranchInstallInfo.begin(), m_mBranchInstallInfo.end(), [&db](std::pair<uint32, BranchInstallInfo*> p)
	{
		p.second->deleteFromDb(db);
	});

	try
	{
		sqlite3x::sqlite3_command cmd(*db, "DELETE FROM exe WHERE itemid=?;");
		cmd.bind(1, (long long int)m_iId.toInt64());
		cmd.executenonquery();
	}
	catch (...)
	{
	}
}


bool ItemInfo::shouldSaveDb(sqlite3x::sqlite3_connection* db)
{
	if (!db)
		return false;

	bool isDeleted = HasAllFlags(getStatus(), ItemInfoI::STATUS_DELETED);
	bool isOnAccount = HasAllFlags(getStatus(), ItemInfoI::STATUS_ONACCOUNT);
	bool isOnComp = HasAnyFlags(getStatus(), ItemInfoI::STATUS_ONCOMPUTER|ItemInfoI::STATUS_INSTALLED);

	if (isDeleted || (isOnAccount && !isOnComp))
	{
		deleteFromDb(db);
		return false;
	}

	return true;
}

void ItemInfo::saveDb(sqlite3x::sqlite3_connection* db)
{
	if (!shouldSaveDb(db))
		return;

	if (!db)
		return;

	sqlite3x::sqlite3_command cmd(*db, "SELECT count(*) FROM iteminfo WHERE internalid=?;");
	cmd.bind(1, (long long int)m_iId.toInt64());

	int count = cmd.executeint();
		
	if (count == 0)
	{
		saveDbFull(db);
	}
	else
	{
		sqlite3x::sqlite3_command cmd(*db, "UPDATE iteminfo SET "
											"statusflags=?,"
											"percent=?,"
											"icon=?,"
											"logo=?,"
											"iconurl=?,"
											"logourl=?,"
											"ibranch=?,"
											"lastbranch=? WHERE internalid=?;");
		
		uint32 status = m_iStatus&(~ItemInfoI::STATUS_DEVELOPER);

		cmd.bind(1, (int)status); //status
		cmd.bind(2, (int)m_iPercent); //percent
		
		cmd.bind(3, UTIL::OS::getRelativePath(m_szIcon)); //icon
		cmd.bind(4, UTIL::OS::getRelativePath(m_szLogo)); //logo
		cmd.bind(5, m_szIconUrl); //icon
		cmd.bind(6, m_szLogoUrl); //logo

		cmd.bind(7, (int)m_INBranch);
		cmd.bind(8, (int)m_LastBranch);

		cmd.bind(9, (long long int)m_iId.toInt64()); //internal id
		cmd.executenonquery();

		for (size_t x=0; x<m_vBranchList.size(); x++)
		{
			m_vBranchList[x]->saveDb(db);
		}

		std::for_each(m_mBranchInstallInfo.begin(), m_mBranchInstallInfo.end(), [&db](std::pair<uint32, BranchInstallInfo*> p)
		{
			p.second->saveDb(db);
		});
	}
}

void ItemInfo::saveDbFull(sqlite3x::sqlite3_connection* db)
{
	if (!shouldSaveDb(db))
		return;

	if (!db)
		return;

	m_bAddedToDb = true;

	uint32 status = m_iStatus&(~ItemInfoI::STATUS_DEVELOPER);

	sqlite3x::sqlite3_command cmd(*db, "REPLACE INTO iteminfo VALUES (?,?,?,?,?, ?,?,?,?,?, ?,?,?,?,?, ?,?,?);");

	cmd.bind(1, (long long int)m_iId.toInt64());		// id
	cmd.bind(2, (long long int)m_iParentId.toInt64());	// parent id
	cmd.bind(3, (int)m_iPercent);	//
	cmd.bind(4, (int)status);		//
	cmd.bind(5, m_szRating);		//

	cmd.bind(6, m_szDev);			//
	cmd.bind(7, m_szName);			//
	cmd.bind(8, m_szShortName);		//
	cmd.bind(9, m_szProfile);		//
	cmd.bind(10, m_szDevProfile);	//

	cmd.bind(11, UTIL::OS::getRelativePath(m_szIcon));			//
	cmd.bind(12, m_szIconUrl);		//
	cmd.bind(13, UTIL::OS::getRelativePath(m_szLogo));			//
	cmd.bind(14, m_szLogoUrl);		//
	cmd.bind(15, m_szPublisher);

	cmd.bind(16, m_szPublisherProfile);
	cmd.bind(17, (int)m_INBranch);
	cmd.bind(18, (int)m_LastBranch);

	cmd.executenonquery();

	for (size_t x=0; x<m_vBranchList.size(); x++)
	{
		m_vBranchList[x]->saveDbFull(db);
	}

	std::for_each(m_mBranchInstallInfo.begin(), m_mBranchInstallInfo.end(), [&db](std::pair<uint32, BranchInstallInfo*> p)
	{
		p.second->saveDb(db);
	});
}

void ItemInfo::loadDb(sqlite3x::sqlite3_connection* db)
{
	if (!db)
		return;

	m_bAddedToDb = true;

	sqlite3x::sqlite3_command cmd(*db, "SELECT * FROM iteminfo WHERE internalid=?;");
	cmd.bind(1, (long long int)m_iId.toInt64());
	sqlite3x::sqlite3_reader reader = cmd.executereader();
	
	reader.read();

	// reader.getint(0); //internal id
	// reader.getint(1); //parent id

	m_iPercent		= reader.getint(2);				 //percent
	m_iStatus		= reader.getint(3);				 //status flags
	m_szRating		= gcString(reader.getstring(4)); //rating

	m_szDev			= gcString(reader.getstring(5)); //developer
	m_szName		= gcString(reader.getstring(6)); //name
	m_szShortName	= gcString(reader.getstring(7)); //shortname
	m_szProfile		= gcString(reader.getstring(8)); //profile url
	m_szDevProfile	= gcString(reader.getstring(9)); //dev profile

	m_szIcon		= UTIL::OS::getAbsPath(reader.getstring(10)); //icon
	m_szIconUrl		= gcString(reader.getstring(11)); //icon url
	m_szLogo		= UTIL::OS::getAbsPath(reader.getstring(12)); //logo
	m_szLogoUrl		= gcString(reader.getstring(13)); //logo url

	m_szPublisher	= reader.getstring(14);
	m_szPublisherProfile = reader.getstring(15);

	m_INBranch		= MCFBranch::BranchFromInt(reader.getint(16));
	m_LastBranch	= MCFBranch::BranchFromInt(reader.getint(17));


	if (HasAnyFlags(m_iStatus, UserCore::Item::ItemInfoI::STATUS_ONACCOUNT))
	{
		m_bWasOnAccount = true;
		m_iStatus &= ~UserCore::Item::ItemInfoI::STATUS_ONACCOUNT;
	}

	delSFlag(ItemInfoI::STATUS_UPDATEAVAL);

	{
		std::vector<uint32> vIdList;

		sqlite3x::sqlite3_command cmd(*db, "SELECT biid FROM installinfo WHERE itemid=?;");
		cmd.bind(1, (long long int)m_iId.toInt64());
		sqlite3x::sqlite3_reader reader = cmd.executereader();
	
		while (reader.read())
		{
			vIdList.push_back(reader.getint(0));
		}

		for (size_t x=0; x<vIdList.size(); x++)
		{
			BranchInstallInfo* bii = new BranchInstallInfo(MCFBranch::BranchFromInt(vIdList[x]), this, m_pFileSystem);

			try
			{
				bii->loadDb(db);
			}
			catch (std::exception &)
			{
				safe_delete(bii);
				continue;
			}

			m_mBranchInstallInfo[vIdList[x]] = bii;
		}
	}

	{
		std::vector<std::pair<uint32,uint32> > vIdList;

		sqlite3x::sqlite3_command cmd(*db, "SELECT branchid, biid FROM branchinfo WHERE internalid=?;");
		cmd.bind(1, (long long int)m_iId.toInt64());
		sqlite3x::sqlite3_reader reader = cmd.executereader();
	
		while (reader.read())
		{
			vIdList.push_back(std::pair<uint32, uint32>(reader.getint(0), reader.getint(1)));
		}

		for (size_t x=0; x<vIdList.size(); x++)
		{
			auto it = m_mBranchInstallInfo.find(vIdList[x].second);

			if (it == m_mBranchInstallInfo.end())
				m_mBranchInstallInfo[vIdList[x].second] = new BranchInstallInfo(vIdList[x].second, this, m_pFileSystem);

			BranchInfo* bi = new BranchInfo(MCFBranch::BranchFromInt(vIdList[x].first), m_iId, m_mBranchInstallInfo[vIdList[x].second], 0, m_pUserCore->getUserId());
			bi->onBranchInfoChangedEvent += delegate(this, &ItemInfo::onBranchInfoChanged);

			try
			{
				bi->loadDb(db);
			}
			catch (std::exception &)
			{
				safe_delete(bi);
				continue;
			}

			if (vIdList[x].first == m_INBranch)
				m_INBranchIndex = x;

			m_vBranchList.push_back(bi);
		}
	}

	setIconUrl(m_szIconUrl.c_str());
	setLogoUrl(m_szLogoUrl.c_str());

	bool isInstalling = HasAnyFlags(getStatus(), ItemInfoI::STATUS_INSTALLING|ItemInfoI::STATUS_DOWNLOADING|ItemInfoI::STATUS_VERIFING);

	if (!isInstalling)
	{
		addSFlag(ItemInfoI::STATUS_ONCOMPUTER);

		if (getStatus() & ItemInfoI::STATUS_INSTALLCOMPLEX)
		{
			UserCore::MCFManagerI *mm = getUserCore()->getInternal()->getMCFManager();

			gcString path = mm->getMcfPath(this);

			if (m_pFileSystem->isValidFile(UTIL::FS::PathWithFile(path)))
			{
				if (!isDownloadable())
					addSFlag(ItemInfoI::STATUS_INSTALLED);
			}
			else
			{
				delSFlag(ItemInfoI::STATUS_INSTALLED);
			}
		}
		else
		{
			BranchInfo* bi = getCurrentBranchFull();

			if (!bi && HasAnyFlags(getStatus(), ItemInfoI::STATUS_LINK) && m_mBranchInstallInfo.find(BUILDID_PUBLIC) != end(m_mBranchInstallInfo))
			{
				m_vBranchList.push_back(new UserCore::Item::BranchInfo(MCFBranch::BranchFromInt(0), getId(), m_mBranchInstallInfo[BUILDID_PUBLIC], 0, m_pUserCore->getUserId()));
				bi = m_vBranchList[0];
				bi->setLinkInfo(getName());

				m_INBranchIndex = 0;
				m_INBranch = MCFBranch::BranchFromInt(0);
			}

			if (bi && m_pFileSystem->isValidFile(UTIL::FS::PathWithFile(bi->getInstallInfo()->getInstallCheck())) )
			{
				if (!isDownloadable())
					addSFlag(ItemInfoI::STATUS_INSTALLED);
			}
			else
			{
				delSFlag(ItemInfoI::STATUS_INSTALLED);
			}
		}
	}

	if (HasAnyFlags(getStatus(), ItemInfoI::STATUS_INSTALLED) && !isDownloadable())
		addSFlag(ItemInfoI::STATUS_READY);

	triggerCallBack();
}

void ItemInfo::loadBranchXmlData(const XML::gcXMLElement &branch)
{
	gcTrace("");

	uint32 id = 0;
	branch.GetAtt("id", id);

	if (id == 0)
		return;

	BranchInfo* bi = nullptr;
	bool found = false;

	for (size_t x=0; x<m_vBranchList.size(); x++)
	{
		if (m_vBranchList[x]->getBranchId() == id)
		{
			if (id == m_INBranch)
				m_INBranchIndex = x;

			bi = m_vBranchList[x];
			found = true;
		}
	}

	if (!bi)
	{
		uint32 platformId = 100;
		branch.GetAtt("platformid", platformId);

		auto it = m_mBranchInstallInfo.find(platformId);

		if (it == m_mBranchInstallInfo.end())
			m_mBranchInstallInfo[platformId] = new BranchInstallInfo(platformId, this, m_pFileSystem);

		bi = new BranchInfo(MCFBranch::BranchFromInt(id), m_iId, m_mBranchInstallInfo[platformId], platformId, m_pUserCore->getUserId());
		bi->onBranchInfoChangedEvent += delegate(this, &ItemInfo::onBranchInfoChanged);
	}

	bi->loadXmlData(branch);

	if (!found)
	{
		m_vBranchList.push_back(bi);

		size_t x=m_vBranchList.size()-1;
		if (m_vBranchList[x]->getBranchId() == m_INBranch)
			m_INBranchIndex = x;
	}
}

void ItemInfo::loadXmlData(uint32 platform, const XML::gcXMLElement &xmlNode, uint16 statusOveride, WildcardManager* pWildCard, bool reset)
{
	gcTrace("");

	if (!xmlNode.IsValid())
		throw gcException(ERR_BADXML);

	pauseCallBack();

	auto statNode = xmlNode.FirstChildElement("status");
	if (statNode.IsValid())
	{
		bool isDev = (m_iStatus&ItemInfoI::STATUS_DEVELOPER)?true:false;

		statNode.GetAtt("id", m_iStatus);

		if (isDev)
			m_iStatus |= ItemInfoI::STATUS_DEVELOPER;
	}

	bool installed = HasAllFlags(m_iStatus, ItemInfoI::STATUS_INSTALLED);
	bool verifying = HasAllFlags(m_iStatus, ItemInfoI::STATUS_VERIFING);

	addSFlag(statusOveride);
	delSFlag(ItemInfoI::STATUS_INSTALLED);	//need this otherwise installpath and install check dont get set
	
	processInfo(xmlNode);

	xmlNode.FirstChildElement("branches").for_each_child("branch", [this](const XML::gcXMLElement &branch)
	{
		loadBranchXmlData(branch);
	});

	//the only time settings should be present if the xml came from the api
	auto setNode = xmlNode.FirstChildElement("settings");

	if (setNode.IsValid() && pWildCard)
		processSettings(platform, setNode, pWildCard, reset);

	gcString installCheckFile;

	BranchInfo* bi = getCurrentBranchFull();

	if (bi)
		installCheckFile = bi->getInstallInfo()->getInstallCheck();

	if (getStatus() & ItemInfoI::STATUS_INSTALLCOMPLEX)
	{
		UserCore::MCFManagerI *mm = getUserCore()->getInternal()->getMCFManager();
		installCheckFile = mm->getMcfPath(this);
	}

	if (m_pFileSystem->isValidFile(UTIL::FS::PathWithFile(installCheckFile)) && (installed || !isDownloadable()))
	{
		addSFlag(ItemInfoI::STATUS_INSTALLED);

		if (!verifying)
			addSFlag(ItemInfoI::STATUS_READY);
	}

	//work out best branch
	if (isInstalled() && isDownloadable() && m_INBranch == 0)
	{
		std::vector<uint32> vBranchList;

		for (size_t x=0; x<getBranchCount(); x++)
		{
			UserCore::Item::BranchInfoI* bi = getBranch(x);

			if (bi->getFlags() & (UserCore::Item::BranchInfoI::BF_MEMBERLOCK|UserCore::Item::BranchInfoI::BF_REGIONLOCK) )
				continue;

			if (bi->getFlags()&UserCore::Item::BranchInfoI::BF_NORELEASES)
				continue;

			if (bi->getFlags()&(UserCore::Item::BranchInfoI::BF_DEMO|UserCore::Item::BranchInfoI::BF_TEST))
				continue;

			if (!(bi->getFlags()&UserCore::Item::BranchInfoI::BF_ONACCOUNT) && !(bi->getFlags()&UserCore::Item::BranchInfoI::BF_FREE))
				continue;

			vBranchList.push_back(x);
		}

		if (vBranchList.size() == 1)
		{
			m_INBranchIndex = vBranchList[0];
			m_INBranch = m_vBranchList[m_INBranchIndex]->getBranchId();
			m_vBranchList[m_INBranchIndex]->getInstallInfo()->setInstalledMcf(m_vBranchList[m_INBranchIndex]->getLatestBuild());
		}
		else
		{
			addSFlag(UserCore::Item::ItemInfoI::STATUS_LINK);
		}
	}
	else if (isInstalled() && !isDownloadable())
	{
		addSFlag(UserCore::Item::ItemInfoI::STATUS_LINK);
		delSFlag(UserCore::Item::ItemInfoI::STATUS_DELETED);
	}

	m_iChangedFlags |= ItemInfoI::CHANGED_INFO;
	broughtCheck();

	m_pUserCore->getItemManager()->saveItem(this);

	resumeCallBack();
	triggerCallBack();
}


void ItemInfo::processInfo(const XML::gcXMLElement &xmlEl)
{
	gcTrace("");

	//desura info
	xmlEl.GetChild("name", this, &ItemInfo::setName);
	xmlEl.GetChild("nameid", m_szShortName);
	xmlEl.GetChild("summary", m_szDesc);
	xmlEl.GetChild("url", m_szProfile);
	xmlEl.GetChild("style", m_szGenre);
	xmlEl.GetChild("theme", m_szTheme);
	xmlEl.GetChild("rating", m_szRating);
	xmlEl.GetChild("eula", m_szEULAUrl);

	bool isDev = false;

	if (xmlEl.GetChild("devadmin", isDev) && isDev)
		addSFlag(STATUS_DEVELOPER);

	auto logoNode= xmlEl.FirstChildElement("boxart");

	if (logoNode.IsValid())
	{
		const std::string icon= logoNode.GetText();

		if (UTIL::MISC::isWebURL(icon.c_str()))
			setLogoUrl(icon.c_str());
		else
			setLogo(icon.c_str());
	}

	auto iconNode= xmlEl.FirstChildElement("icon");
	if (iconNode.IsValid())
	{
		const std::string icon= iconNode.GetText();

		if (UTIL::MISC::isWebURL(icon.c_str()))
			setIconUrl(icon.c_str());
		else
			setIcon(icon.c_str());
	}

	//add support for install info here;



	bool downloadable = false;
	if (xmlEl.GetChild("downloadable", downloadable))
	{
		if (downloadable)
			delSFlag(ItemInfoI::STATUS_NONDOWNLOADABLE);
		else
			addSFlag(ItemInfoI::STATUS_NONDOWNLOADABLE);
	}

	uint32 dlc = 0;
	if (xmlEl.GetChild("expansion", dlc))
	{
		if (dlc)
			addSFlag(ItemInfoI::STATUS_DLC);
		else
			delSFlag(ItemInfoI::STATUS_DLC);
	}

	auto devNode = xmlEl.FirstChildElement("developer");
	if (devNode.IsValid())
	{
		devNode.GetChild("name", m_szDev);
		devNode.GetChild("url", m_szDevProfile);
	}

	auto pubNode = xmlEl.FirstChildElement("publisher");
	if (pubNode.IsValid())
	{
		pubNode.GetChild("name", m_szPublisher);
		pubNode.GetChild("url", m_szPublisherProfile);
	}
}

void ItemInfo::processSettings(uint32 platform, const XML::gcXMLElement &setNode, WildcardManager* pWildCard, bool reset)
{
	gcTrace("");

	bool hasBroughtItem = false;

	for (size_t x=0; x<m_vBranchList.size(); x++)
	{
		UserCore::Item::BranchInfoI* branch = m_vBranchList[x];

		if (HasAnyFlags(branch->getFlags(), UserCore::Item::BranchInfoI::BF_ONACCOUNT) && !HasAnyFlags(branch->getFlags(), UserCore::Item::BranchInfoI::BF_NORELEASES))
		{
			hasBroughtItem = true;
			break;
		}
	}

	bool installComplex = false;
	setNode.GetChild("installcomplex", installComplex);

	if (installComplex)
		addSFlag(ItemInfoI::STATUS_INSTALLCOMPLEX);
	
	auto it = m_mBranchInstallInfo.find(platform);

	if (it == m_mBranchInstallInfo.end() && !isDownloadable())
	{
		//this item is not downloadable thus has no branches. Create a install info for it.
		m_mBranchInstallInfo[platform] = new BranchInstallInfo(platform, this, m_pFileSystem);
		it = m_mBranchInstallInfo.find(platform);
	}

	if (it != m_mBranchInstallInfo.end())
	{
		uint32 flags = 0;
		char* cip = nullptr;

		getUserCore()->getItemManager()->getCIP(m_iId, &cip);

		ProcessResult pr = it->second->processSettings(setNode, pWildCard, reset, hasBroughtItem, cip);

		safe_delete(cip);

		if (pr.useCip || pr.found)
		{
			flags = ItemInfoI::STATUS_ONCOMPUTER;

			if ((!isDownloadable() || pr.notFirst) && m_pFileSystem->isValidFile(UTIL::FS::PathWithFile(pr.insCheck)))
			{
				flags |= ItemInfoI::STATUS_INSTALLED;
				setInstalledMcf(MCFBranch::BranchFromInt(0), MCFBuild::BuildFromInt(0));
			}
				
			if (!isDownloadable() || pr.notFirst)
				flags |= ItemInfoI::STATUS_LINK;

			addSFlag(flags);
		}

		if (HasAnyFlags(getStatus(), UserCore::Item::ItemInfoI::STATUS_LINK))
		{
			if (m_vBranchList.size() == 0)
				m_vBranchList.push_back(new UserCore::Item::BranchInfo(MCFBranch::BranchFromInt(0), getId(), it->second, 0, m_pUserCore->getUserId()));

			setInstalledMcf(MCFBranch::BranchFromInt(0), MCFBuild::BuildFromInt(0));
		}
	}
	else
	{
		Warning("Failed to find platform install settings!\n");
	}
}

void ItemInfo::setIcon(const char* icon)		
{
	if (!m_pFileSystem->isValidFile(m_szIcon))
		m_szIcon = "";

	if (!icon)
		return;

	if (!m_pFileSystem->isValidFile(icon))
		return;

	m_szIcon = UTIL::FS::PathWithFile(icon).getFullPath();
	m_iChangedFlags |= ItemInfoI::CHANGED_ICON;
	onInfoChange();
}

void ItemInfo::setLogo(const char* logo)		
{
	if (!m_pFileSystem->isValidFile(m_szLogo))
		m_szLogo = "";

	if (!logo)
		return;

	UTIL::FS::Path path = UTIL::FS::PathWithFile(logo);

	if (!m_pFileSystem->isValidFile(path))
		return;

	m_szLogo = path.getFullPath();
	m_iChangedFlags |= ItemInfoI::CHANGED_LOGO;
	onInfoChange();
}

void ItemInfo::setIconUrl(const char* url)		
{
	if (!url)
		return;

	bool changed = (m_szIconUrl != url);

	if (changed)
		m_szIconUrl = gcString(url);

	if (m_szIconUrl != "" && (changed || !m_pFileSystem->isValidFile(m_szIcon)) && getUserCore()->getInternal())
		getUserCore()->getInternal()->downloadImage(this, UserCore::Task::DownloadImgTask::ICON);
}

void ItemInfo::setLogoUrl(const char* url)		
{
	if (!url)
		return;

	bool changed = (m_szLogoUrl != url);

	if (changed)
		m_szLogoUrl = gcString(url);

	if (m_szLogoUrl != "" && (changed || !m_pFileSystem->isValidFile(m_szLogo)) && getUserCore()->getInternal())
		getUserCore()->getInternal()->downloadImage(this, UserCore::Task::DownloadImgTask::LOGO);
}

void ItemInfo::addToAccount()
{
	gcTrace("");

	if (this->getStatus() & ItemInfoI::STATUS_ONACCOUNT)
		return;

	if (!getUserCore()->getInternal())
		return;

	getUserCore()->getInternal()->changeAccount(getId(), UserCore::Task::ChangeAccountTask::ACCOUNT_ADD);

}

void ItemInfo::removeFromAccount()
{
	gcTrace("");

	delSFlag(ItemInfoI::STATUS_ONACCOUNT);

	if (!getUserCore()->getInternal())
		return;

	getUserCore()->getInternal()->changeAccount(getId(), UserCore::Task::ChangeAccountTask::ACCOUNT_REMOVE);
}

void ItemInfo::onInfoChange()
{
	if (!m_bPauseCallBack)
		triggerCallBack();
}

void ItemInfo::triggerCallBack()
{
	ItemInfo_s i;
	i.id = getId();
	i.changeFlags = m_iChangedFlags;

	if (HasAnyFlags(m_iChangedFlags, CHANGED_STATUS) || !getUserCore()->isDelayLoading())
		onInfoChangeEvent(i);
	
	m_iChangedFlags = 0;
}

void ItemInfo::addSFlag(uint32 flags)
{
	if (HasAllFlags(m_iStatus, flags))
		return;

	gcTrace("Flag {0}", flags);

	bool shouldTriggerUpdate = (m_iStatus&ItemInfoI::STATUS_DEVELOPER || HasAnyFlags(flags, (ItemInfoI::STATUS_INSTALLED|ItemInfoI::STATUS_ONCOMPUTER|ItemInfoI::STATUS_ONACCOUNT|ItemInfoI::STATUS_PAUSED|ItemInfoI::STATUS_UPDATEAVAL)));

	m_iStatus |= flags;
	m_iChangedFlags |= ItemInfoI::CHANGED_STATUS;

	if (flags & ItemInfoI::STATUS_VERIFING)
		delSFlag(ItemInfoI::STATUS_READY);

	//cant be ready and doing other things
	if (flags & ItemInfoI::STATUS_READY)
		delSFlag(ItemInfoI::STATUS_DOWNLOADING|ItemInfoI::STATUS_INSTALLING|ItemInfoI::STATUS_UPLOADING|ItemInfoI::STATUS_VERIFING);

	if (flags & ItemInfoI::STATUS_DOWNLOADING)
		delSFlag(ItemInfoI::STATUS_INSTALLING|ItemInfoI::STATUS_UPLOADING);

	if (flags & ItemInfoI::STATUS_INSTALLING)
		delSFlag(ItemInfoI::STATUS_DOWNLOADING|ItemInfoI::STATUS_UPLOADING);

	if (flags & ItemInfoI::STATUS_UPLOADING)
		delSFlag(ItemInfoI::STATUS_DOWNLOADING|ItemInfoI::STATUS_INSTALLING);

	if (flags & ItemInfoI::STATUS_VERIFING)
		delSFlag(ItemInfoI::STATUS_READY);

	if (HasAnyFlags(flags, ItemInfoI::STATUS_DELETED))
	{
		shouldTriggerUpdate = true;
		delSFlag(ItemInfoI::STATUS_INSTALLED | ItemInfoI::STATUS_ONACCOUNT | ItemInfoI::STATUS_ONCOMPUTER | ItemInfoI::STATUS_READY);
	}

	if (shouldTriggerUpdate)
	{
		uint32 num = 1;
		getUserCore()->getItemsAddedEvent()->operator()(num);
	}
	
	onInfoChange();
}

void ItemInfo::delSFlag(uint32 flags)
{
	if (!HasAnyFlags(m_iStatus, flags))
		return;

	gcTrace("Flag {0}", flags);

	bool wasDeleted = isDeleted();

	m_iStatus &= (~flags);

	if (flags & STATUS_PAUSABLE)
		m_iStatus &= (~STATUS_PAUSED);

	if (flags & STATUS_INSTALLED)
		m_iStatus &= (~STATUS_READY);

	onInfoChange();
	DesuraId currentID = getId();

	if (!isDeleted() & wasDeleted)
		m_pUserCore->getItemManager()->getOnNewItemEvent()->operator()(currentID);
}

void ItemInfo::addPFlag(uint8 flags)
{
	gcTrace("Flag {0}", flags);
	m_iPermissions |= flags;
}

void ItemInfo::delPFlag(uint8 flags)
{
	gcTrace("Flag {0}", flags);
	m_iPermissions &= (~flags);
}

void ItemInfo::addOFlag(uint8 flags)
{
	gcTrace("Flag {0}", flags);
	m_iOptions |= flags;

	if (flags & ItemInfoI::OPTION_NOUPDATE)
		delOFlag(ItemInfoI::OPTION_PROMPTUPDATE | ItemInfoI::OPTION_AUTOUPDATE);

	if (flags & ItemInfoI::OPTION_PROMPTUPDATE)
		delOFlag(ItemInfoI::OPTION_NOUPDATE | ItemInfoI::OPTION_AUTOUPDATE);

	if (flags & ItemInfoI::OPTION_AUTOUPDATE)
		delOFlag(ItemInfoI::OPTION_PROMPTUPDATE | ItemInfoI::OPTION_NOUPDATE);
}

void ItemInfo::delOFlag(uint8 flags)
{
	gcTrace("Flag {0}", flags);
	m_iOptions &= (~flags);
}

void ItemInfo::setPercent(uint8 percent)
{
	if (m_iPercent == percent)
		return;

	m_iPercent = std::min(std::max((int)percent,0),100);
	m_iChangedFlags |= ItemInfoI::CHANGED_PERCENT;

	onInfoChange();
}

bool ItemInfo::isFavorite()
{
	return m_pUserCore->getItemManager()->isItemFavorite(getId());
}

void ItemInfo::setFavorite(bool fav)
{
	m_pUserCore->getItemManager()->setFavorite(getId(), fav);
}

void ItemInfo::updated()
{
	if (getCurrentBranchFull())
		getCurrentBranchFull()->getInstallInfo()->updated();

	delSFlag(ItemInfoI::STATUS_UPDATEAVAL);
}

bool ItemInfo::compare(const char* filter)
{
	gcString f(filter);
	gcString dev(m_szDev);

	std::transform(f.begin(), f.end(), f.begin(), tolower);
	std::transform(dev.begin(), dev.end(), dev.begin(), tolower);

	if (strstr(dev.c_str(), f.c_str())!=nullptr)
		return true;


	gcString name(m_szName);
	std::transform(name.begin(), name.end(), name.begin(), tolower);

	if (strstr(name.c_str(), f.c_str())!=nullptr)
		return true;


	gcString sname(m_szShortName);
	std::transform(sname.begin(), sname.end(), sname.begin(), tolower);

	if (strstr(sname.c_str(), f.c_str())!=nullptr)
		return true;


	gcString theme(m_szTheme);
	std::transform(theme.begin(), theme.end(), theme.begin(), tolower);
	if (strstr(theme.c_str(), f.c_str())!=nullptr)
		return true;

	gcString genre(m_szGenre);
	std::transform(genre.begin(), genre.end(), genre.begin(), tolower);


	if (strstr(genre.c_str(), f.c_str())!=nullptr)
		return true;

	return false;	
}


void ItemInfo::processUpdateXml(const XML::gcXMLElement &node)
{
	//gcTrace("");
	node.FirstChildElement("branches").for_each_child("branch", [this](const XML::gcXMLElement &branch)
	{
		uint32 id = 0;
		branch.GetAtt("id", id);

		if (id == 0)
			return;

		BranchInfo* bi = nullptr;

		for (size_t x=0; x<m_vBranchList.size(); x++)
		{
			if (m_vBranchList[x]->getBranchId() == id)
			{
				bi = m_vBranchList[x];
			}
		}

		if (!bi)
		{
			uint32 platformId = 100;
			branch.GetAtt("platformid", platformId);

			auto it = m_mBranchInstallInfo.find(platformId);

			if (it == m_mBranchInstallInfo.end())
				m_mBranchInstallInfo[platformId] = new BranchInstallInfo(platformId, this, m_pFileSystem);

			bi = new BranchInfo(MCFBranch::BranchFromInt(id), m_iId, m_mBranchInstallInfo[platformId], platformId, m_pUserCore->getUserId());
			bi->loadXmlData(branch);

			bi->onBranchInfoChangedEvent += delegate(this, &ItemInfo::onBranchInfoChanged);
			m_vBranchList.push_back(bi);
		}
		else
		{
			bi->loadXmlData(branch);
		}

		if (bi->getBranchId() == m_INBranch)
		{
			if (bi->getInstallInfo()->processUpdateXml(branch))
				addSFlag(ItemInfoI::STATUS_UPDATEAVAL);
		}	

	});

	broughtCheck();
}

void ItemInfo::broughtCheck()
{
	if (HasAnyFlags(getStatus(), ItemInfoI::STATUS_LINK) == false)
		return;

	bool brought = false;

	for (size_t x=0; x<m_vBranchList.size(); x++)
	{
		BranchInfo* bi = m_vBranchList[x];

		bool onAccount = HasAnyFlags(bi->getFlags(), BranchInfoI::BF_ONACCOUNT);
		bool isDemo = HasAnyFlags(bi->getFlags(), BranchInfoI::BF_DEMO|BranchInfoI::BF_TEST);
		
		if (onAccount && !isDemo)
		{
			brought = true;
			break;
		}
	}

	if (brought == false)
		return;

	BranchInfo* bi = getCurrentBranchFull();

	//if we have an installed mod, means we are complex and cant be forgoten about
	if (bi && bi->getInstallInfo()->hasInstalledMod())
		return;

	std::vector<UserCore::Item::ItemInfoI*> modList;
	m_pUserCore->getItemManager()->getModList(getId(), modList);

	for (size_t x=0; x<modList.size(); x++)
	{
		ItemHandleI* ih = m_pUserCore->getItemManager()->findItemHandle(modList[x]->getId());

		//If we are doing something with mods, just dont do any thing
		if (ih->isInStage())
			return;
	}

	uint32 delFlags = 
		ItemInfoI::STATUS_LINK|
		ItemInfoI::STATUS_READY|
		ItemInfoI::STATUS_INSTALLED|
		ItemInfoI::STATUS_ONCOMPUTER|
		ItemInfoI::STATUS_UPDATING|
		ItemInfoI::STATUS_DOWNLOADING|
		ItemInfoI::STATUS_INSTALLING|
		ItemInfoI::STATUS_VERIFING|
		ItemInfoI::STATUS_UPDATEAVAL|
		ItemInfoI::STATUS_PAUSED|
		ItemInfoI::STATUS_PAUSABLE;

	for (size_t x=0; x<modList.size(); x++)
	{
		ItemInfo* i = dynamic_cast<ItemInfo*>(modList[x]);

		if (!i)
			continue;

		i->delSFlag(delFlags);
		i->resetInstalledMcf();
	}

	//forget we are installed so they can install the full version
	delSFlag(delFlags|ItemInfoI::STATUS_NONDOWNLOADABLE);
}

void ItemInfo::resetInstalledMcf()
{
	if (getCurrentBranchFull() && getCurrentBranchFull()->getInstallInfo())
		getCurrentBranchFull()->getInstallInfo()->resetInstalledMcf();

	m_LastBranch = MCFBranch();
	m_INBranch = MCFBranch();
	m_INBranchIndex = -1;

	onInfoChange();
}

bool ItemInfo::setInstalledMcf(MCFBranch branch, MCFBuild build)
{
	gcTrace("Branch {0}, Build {1}", branch, build);
	for (size_t x=0; x<m_vBranchList.size(); x++)
	{
		if (m_vBranchList[x]->getBranchId() == branch)
		{
			m_INBranchIndex = x;

			m_LastBranch = m_INBranch;
			m_INBranch = branch;
			
			if (build == 0)
				build = m_vBranchList[x]->getLatestBuild();

			if (m_vBranchList[x]->getInstallInfo()->setInstalledMcf(build))
				delSFlag(ItemInfoI::STATUS_UPDATEAVAL);

			onInfoChange();
			return true;
		}
	}

	return false;
}

void ItemInfo::overideInstalledBuild(MCFBuild build)
{
	if (!getCurrentBranchFull())
		return;

	getCurrentBranchFull()->getInstallInfo()->overideInstalledBuild(build);
}

bool ItemInfo::hasAcceptedEula()
{
	BranchInfoI* branch = getCurrentBranch();

	if (!branch)
		return true;

	return branch->hasAcceptedEula();
}

const char* ItemInfo::getEulaUrl()	
{
	BranchInfoI* branch = getCurrentBranch();

	if (branch)
	{
		const char* beula = branch->getEulaUrl();

		if (beula && gcString(beula).size() > 0)
			return beula;
	}

	return "";
}

void ItemInfo::acceptEula()
{
	gcTrace("");

	if (m_INBranchIndex == UINT_MAX)
		return;

	if (m_vBranchList[m_INBranchIndex])
		m_vBranchList[m_INBranchIndex]->acceptEula();
}

void ItemInfo::onBranchInfoChanged()
{
	m_iChangedFlags |= ItemInfoI::CHANGED_STATUS;
	onInfoChange();
}

void ItemInfo::setParentId(DesuraId id)
{
	if (id == m_iParentId)
		return;

	sqlite3x::sqlite3_connection db(getItemInfoDb(getUserCore()->getAppDataPath()).c_str());

	try
	{
		sqlite3x::sqlite3_command cmd(db, "DELETE FROM iteminfo WHERE internalid=?;");
		cmd.bind(1, (long long int)m_iId.toInt64());
		cmd.executenonquery();

		saveDbFull(&db);
	}
	catch (std::exception &e)
	{
		Warning("Failed to change {0} parent id: {1}\n", getName(), e.what());
	}

	m_iParentId = id;
}

void ItemInfo::migrateStandalone(MCFBranch branch, MCFBuild build)
{
	delSFlag(UserCore::Item::ItemInfoI::STATUS_LINK|UserCore::Item::ItemInfoI::STATUS_NONDOWNLOADABLE);
	setInstalledMcf(branch, build);
}

void ItemInfo::setLinkInfo(const char* exe, const char* args)
{
	if (getId().getType() != DesuraId::TYPE_LINK)
		return;

	UTIL::FS::Path path = UTIL::FS::PathWithFile(exe);

	if (m_mBranchInstallInfo.size() == 0)
		m_mBranchInstallInfo[BUILDID_PUBLIC] = new UserCore::Item::BranchInstallInfo(BUILDID_PUBLIC, this, m_pFileSystem);

	BranchInstallInfo *bii = m_mBranchInstallInfo[BUILDID_PUBLIC];

	if (m_vBranchList.size() == 0)
		m_vBranchList.push_back(new UserCore::Item::BranchInfo(MCFBranch::BranchFromInt(0), getId(), bii, 0, m_pUserCore->getUserId()));

	UserCore::Item::BranchInfo* bi = m_vBranchList[0];

	bii->setLinkInfo(path.getFolderPath().c_str(), exe, args);
	bi->setLinkInfo(getName());

	setInstalledMcf(MCFBranch::BranchFromInt(0), MCFBuild::BuildFromInt(0));

	m_iStatus = STATUS_LINK|STATUS_NONDOWNLOADABLE|STATUS_READY|STATUS_ONCOMPUTER|STATUS_INSTALLED;

#ifdef WIN32
	gcString savePathIco = UTIL::OS::getAppDataPath(gcWString(getId().getFolderPathExtension("icon.ico")).c_str());
	gcString savePathPng = UTIL::OS::getAppDataPath(gcWString(getId().getFolderPathExtension("icon.png")).c_str());

	try
	{
		m_pFileSystem->recMakeFolder(UTIL::FS::PathWithFile(savePathIco));
		UTIL::FS::FileHandle fh(savePathIco.c_str(), UTIL::FS::FILE_WRITE);

		UTIL::WIN::extractIcon(exe, [&fh](const unsigned char* buff, uint32 size) -> bool
		{
			fh.write((char*)buff, size);
			return true;
		});

		fh.close();

		UTIL::MISC::convertToPng(savePathIco, savePathPng);
		setIcon(savePathPng.c_str());
	}
	catch (...)
	{
	}
#endif
}

MCFBranch ItemInfo::getBestBranch(MCFBranch branch)
{
	BranchInfoI* bi = nullptr;

	if (branch.isGlobal())
	{
		std::vector<BranchInfo*> filterList;

		for (size_t x=0; x<m_vBranchList.size(); x++)
		{
			if (m_vBranchList[x]->getGlobalId() == branch)
				filterList.push_back(m_vBranchList[x]);
		}

		return selectBestBranch(filterList);
	}

	bi = getBranchById(branch);

	if (bi)
		return bi->getBranchId();

	return selectBestBranch(m_vBranchList);
}

MCFBranch ItemInfo::selectBestBranch(const std::vector<BranchInfo*> &list)
{
	if (list.size() == 1)
		return list[0]->getBranchId();

	std::vector<BranchInfo*> shortList;

	bool hasWinBranch = false;
	bool hasNixBranch = false;
	bool is64 = UTIL::OS::is64OS();

	auto shortListBranches = [&](bool ignoreDemo)
	{
		hasWinBranch = false;
		hasNixBranch = false;

		for (size_t x=0; x<list.size(); x++)
		{
			UserCore::Item::BranchInfoI* bi = list[x];

			if (!bi)
				continue;

			if (bi->isWindows())
				hasWinBranch = true;

			if (bi->isLinux())
				hasNixBranch = true;

			uint32 flags = bi->getFlags();

			bool noRelease = HasAllFlags(flags, UserCore::Item::BranchInfoI::BF_NORELEASES);
			bool isPreorder = bi->isPreOrder();

			if (noRelease && !isPreorder)
				continue;

			bool free = HasAnyFlags(flags, UserCore::Item::BranchInfoI::BF_FREE);
			bool onAccount = HasAllFlags(flags, UserCore::Item::BranchInfoI::BF_ONACCOUNT);
			bool locked = HasAnyFlags(flags, UserCore::Item::BranchInfoI::BF_MEMBERLOCK|UserCore::Item::BranchInfoI::BF_REGIONLOCK);

			if (!onAccount && (locked || !free))
				continue;
		
			bool isDemo = HasAnyFlags(flags, UserCore::Item::BranchInfoI::BF_DEMO);
			bool test = HasAnyFlags(flags, UserCore::Item::BranchInfoI::BF_TEST);
		
			if ((!ignoreDemo && isDemo) || test)
				continue;

			if (!is64 && list[x]->is64Bit())
				continue;

			shortList.push_back(list[x]);
		}
	};

	shortListBranches(true);

	if (shortList.size() == 0)
		shortListBranches(false);

#ifdef NIX
	//Remove all windows branches if we have a linux branch
	if (hasWinBranch && hasNixBranch)
	{
		std::vector<BranchInfo*> t = shortList;
		shortList.clear();

		for (size_t x=0; x<t.size(); x++)
		{
			if (t[x]->isLinux())
				shortList.push_back(t[x]);
		}
	}
#endif

	if (shortList.size() > 1 && is64)
	{
		std::vector<BranchInfo*> t = shortList;
		shortList.clear();

		for (size_t x=0; x<t.size(); x++)
		{
			if (t[x]->is64Bit())
				shortList.push_back(t[x]);
		}
		
		if (shortList.size() == 0) //filtered all :(
			shortList = t;
	}
	
	if (shortList.size() == 1)
		return shortList[0]->getBranchId();

	return MCFBranch::BranchFromInt(0);
}


BranchInfoI* ItemInfo::getBranch(uint32 index)
{
	if ((int32)index >= m_vBranchList.size())
		return nullptr;

	return m_vBranchList[index];
}

BranchInfoI* ItemInfo::getCurrentBranch()
{
	if (m_INBranchIndex == UINT_MAX)
		return nullptr;

	return m_vBranchList[m_INBranchIndex];
}

BranchInfo* ItemInfo::getCurrentBranchFull()
{
	if (m_INBranchIndex == UINT_MAX)
		return nullptr;

	return m_vBranchList[m_INBranchIndex];
}

BranchInfoI* ItemInfo::getBranchById(uint32 id)
{
	for (size_t x=0; x<m_vBranchList.size(); x++)
	{
		if (m_vBranchList[x]->getBranchId() == id)
			return m_vBranchList[x];
	}

	return nullptr;
}


DesuraId ItemInfo::getInstalledModId(MCFBranch branch)
{
	if (!getCurrentBranchFull())
		return DesuraId();

	return getCurrentBranchFull()->getInstallInfo()->getInstalledModId();
}


BranchInstallInfo* ItemInfo::getBranchOrCurrent(MCFBranch branch)
{
	if (branch == 0)
		branch = m_INBranch;

	BranchInfo* bi = dynamic_cast<BranchInfo*>(getBranchById(branch));

	if (!bi && !isDownloadable() && m_mBranchInstallInfo.size() > 0)
		return m_mBranchInstallInfo.begin()->second;

	if (!bi)
		return nullptr;

	return bi->getInstallInfo();
}

const char* ItemInfo::getPath(MCFBranch branch)		
{
	BranchInstallInfo* bi = getBranchOrCurrent(branch);

	if (!bi)
		return nullptr;

	return bi->getPath();
}

const char* ItemInfo::getInsPrimary(MCFBranch branch)		
{
	BranchInstallInfo* bi = getBranchOrCurrent(branch);

	if (!bi)
		return nullptr;

	return bi->getInsPrimary();
}

void ItemInfo::setInstalledModId(DesuraId id, MCFBranch branch)
{
	BranchInstallInfo* bi = getBranchOrCurrent(branch);

	if (!bi)
		return;

	bi->setInstalledModId(id);
}

const char* ItemInfo::getInstalledVersion(MCFBranch branch)	
{
	BranchInstallInfo* bi = getBranchOrCurrent(branch);

	if (!bi)
		return nullptr;

	return bi->getInstalledVersion();
}

uint64 ItemInfo::getInstallSize(MCFBranch branch)
{
	BranchInstallInfo* bi = getBranchOrCurrent(branch);

	if (!bi)
		return 0;

	return bi->getInstallSize();
}

uint64 ItemInfo::getDownloadSize(MCFBranch branch)
{
	BranchInstallInfo* bi = getBranchOrCurrent(branch);

	if (!bi)
		return 0;

	return bi->getDownloadSize();
}

MCFBuild ItemInfo::getLastInstalledBuild(MCFBranch branch)
{
	BranchInstallInfo* bi = getBranchOrCurrent(branch);

	if (!bi)
		return MCFBuild();

	return bi->getLastInstalledBuild();
}

MCFBuild ItemInfo::getInstalledBuild(MCFBranch branch)
{
	BranchInstallInfo* bi = getBranchOrCurrent(branch);

	if (!bi)
		return MCFBuild();

	return bi->getInstalledBuild();
}

MCFBuild ItemInfo::getNextUpdateBuild(MCFBranch branch)
{
	BranchInstallInfo* bi = getBranchOrCurrent(branch);

	if (!bi)
		return MCFBuild();

	return bi->getNextUpdateBuild();
}

void ItemInfo::overrideMcfBuild(MCFBuild build, MCFBranch branch)
{
	BranchInstallInfo* bi = getBranchOrCurrent(branch);

	if (!bi)
		return;

	bi->overideMcfBuild(build);
}

uint32 ItemInfo::getExeCount(bool setActive, MCFBranch branch)
{
	BranchInstallInfo* bi = getBranchOrCurrent(branch);

	if (!bi)
		return 0;

	return bi->getExeCount(setActive);
}

void ItemInfo::getExeList(std::vector<UserCore::Item::Misc::ExeInfoI*> &list, MCFBranch branch)
{
	BranchInstallInfo* bi = getBranchOrCurrent(branch);

	if (!bi)
		return;

	bi->getExeList(list);
}

UserCore::Item::Misc::ExeInfoI* ItemInfo::getActiveExe(MCFBranch branch)
{
	BranchInstallInfo* bi = getBranchOrCurrent(branch);

	if (!bi)
		return nullptr;

	return bi->getActiveExe();
}

void ItemInfo::setActiveExe(const char* name, MCFBranch branch)
{
	BranchInstallInfo* bi = getBranchOrCurrent(branch);

	if (!bi)
		return;

	bi->setActiveExe(name);
}

void ItemInfo::softDelete()
{
	//make sure this flag is dead. Long live the flag
	delSFlag(ItemInfoI::STATUS_LINK);

	if (HasAnyFlags(getStatus(), ItemInfoI::STATUS_DEVELOPER))
		delSFlag(ItemInfoI::STATUS_INSTALLED | ItemInfoI::STATUS_ONACCOUNT | ItemInfoI::STATUS_ONCOMPUTER | ItemInfoI::STATUS_READY);
	else
		addSFlag(UserCore::Item::ItemInfoI::STATUS_DELETED);

	resetInstalledMcf();
}


#ifdef LINK_WITH_GTEST

namespace UnitTest
{
	using namespace ::testing;

	class ItemInfoThirdPartyFixture : public ::testing::Test
	{
	public:
		ItemInfoThirdPartyFixture()
			: i(&user, DesuraId(12884901920), &fs)
		{
			checkPath = [](const UTIL::FS::Path& path) -> bool
			{
				return path.getFile().getFile() == "Charlie.exe";
			};

			ON_CALL(fs, isValidFile(_)).WillByDefault(Invoke(checkPath));
			ON_CALL(user, getUserId()).WillByDefault(Return(1));
			ON_CALL(user, getItemsAddedEvent()).WillByDefault(Return(&m_ItemAddedEvent));
			ON_CALL(user, getItemManager()).WillByDefault(Return(&m_ItemManager));

			ON_CALL(m_ItemManager, getOnNewItemEvent()).WillByDefault(Return(&m_NewItemEvent));
		}

		void setUpDb(sqlite3x::sqlite3_connection &db, const std::vector<std::string> &vSqlCommands)
		{
			createItemInfoDbTables(db);

			for (auto s : vSqlCommands)
			{
				sqlite3x::sqlite3_command cmd(db, s.c_str());
				cmd.executenonquery();
			}
		}

		void resolveWildcard(WCSpecialInfo &info)
		{
			if (info.name == "PROGRAM_FILES")
			{
				info.result = "C:\\Program Files (x86)";
				info.handled = true;
			}
		}

		Event<DesuraId> m_NewItemEvent;
		Event<uint32> m_ItemAddedEvent;

		UserCore::UserMock user;
		UTIL::FS::UtilFSMock fs;
		UserCore::ItemManagerMock m_ItemManager;

		std::function<bool(const UTIL::FS::Path&)> checkPath;
		ItemInfo i;
	};

	static const std::vector<std::string> vSqlCommands =
	{
#ifdef WIN32
		"INSERT INTO exe VALUES(12884901920,100,'Play','C:\\Program Files (x86)\\charlie\\Charlie.exe','','',0);",
		"INSERT INTO installinfo VALUES(12884901920,100,'C:\\Program Files (x86)\\charlie','C:\\Program Files (x86)\\charlie\\Charlie.exe','',0,0,0);",
		"INSERT INTO installinfoex VALUES(12884901920,100,'C:\\Program Files (x86)\\charlie\\Charlie.exe');",
		"INSERT INTO iteminfo VALUES(12884901920,0,0,2129934,0,'dev-02','Charlie','charlie','','','','','','','dev-02','',0,0);",
		"INSERT INTO branchinfo VALUES(0, 12884901920, 'Charlie', 12292, '', 0, 0, '', '', -842150451, 0, 100);"
#elif NIX64
		"INSERT INTO exe VALUES(12884901920,120,'Play','C:\\Program Files (x86)\\charlie\\Charlie.exe','','',0);",
		"INSERT INTO installinfo VALUES(12884901920,120,'C:\\Program Files (x86)\\charlie','C:\\Program Files (x86)\\charlie\\Charlie.exe','',0,0,0);",
		"INSERT INTO installinfoex VALUES(12884901920,120,'C:\\Program Files (x86)\\charlie\\Charlie.exe');",
		"INSERT INTO iteminfo VALUES(12884901920,0,0,2129934,0,'dev-02','Charlie','charlie','','','','','','','dev-02','',0,0);"
		"INSERT INTO branchinfo VALUES(0, 12884901920, 'Charlie', 12292, '', 0, 0, '', '', -842150451, 0, 120);"
#else
		"INSERT INTO exe VALUES(12884901920,110,'Play','C:\\Program Files (x86)\\charlie\\Charlie.exe','','',0);",
		"INSERT INTO installinfo VALUES(12884901920,110,'C:\\Program Files (x86)\\charlie','C:\\Program Files (x86)\\charlie\\Charlie.exe','',0,0,0);",
		"INSERT INTO installinfoex VALUES(12884901920,110,'C:\\Program Files (x86)\\charlie\\Charlie.exe');",
		"INSERT INTO iteminfo VALUES(12884901920,0,0,2129934,0,'dev-02','Charlie','charlie','','','','','','','dev-02','',0,0);"
		"INSERT INTO branchinfo VALUES(0, 12884901920, 'Charlie', 12292, '', 0, 0, '', '', -842150451, 0, 110);"
#endif
	};

	TEST_F(ItemInfoThirdPartyFixture, ThirdPartyLoad)
	{
		sqlite3x::sqlite3_connection db(":memory:");
		
		setUpDb(db, vSqlCommands);
		i.loadDb(&db);

		ASSERT_TRUE(i.isInstalled());
		ASSERT_TRUE(i.isLaunchable());
		ASSERT_FALSE(i.isDownloadable());
	}

	const char* szThirdPartyInfo =
		"<game siteareaid=\"3\">"
		"	<name>Charlie</name>"
		"	<nameid>charlie</nameid>"
		"	<expansion>0</expansion>"
		"	<downloadable>0</downloadable>"
		"	<uploadable>0</uploadable>"
		"	<settings>"
		"		<executes>"
		"			<execute>"
		"				<name>Play</name>"
		"				<exe>%GAME_EXE%</exe>"
		"				<args></args>"
		"			</execute>"
		"		</executes>"
		"		<exe>%GAME_EXE%</exe>"
		"		<args></args>"
		"		<installlocations>"
		"			<installlocation>"
		"				<check>%PROGRAM_FILES%\\Charlie.exe</check>"
		"				<path>%PROGRAM_FILES%</path>"
		"			</installlocation>"
		"		</installlocations>"
		"	</settings>"
		"	<wcards>"
		"		<wcard name=\"GAME_PATH\" type=\"path\">%PROGRAM_FILES%\\charlie</wcard>"
		"		<wcard name=\"GAME_EXE\" type=\"exe\">%INSTALL_PATH%\\Charlie.exe</wcard>"
		"	</wcards>"
		"</game>";
			 

	TEST_F(ItemInfoThirdPartyFixture, ThirdPartyDeleteAndAdd)
	{
		sqlite3x::sqlite3_connection db(":memory:");

		setUpDb(db, vSqlCommands);
		i.loadDb(&db);

		ASSERT_TRUE(i.isInstalled());
		ASSERT_TRUE(i.isLaunchable());
		ASSERT_FALSE(i.isDownloadable());
		ASSERT_FALSE(i.isDeleted());
		ASSERT_TRUE(!!i.getCurrentBranch());

		i.softDelete();

		ASSERT_FALSE(i.isInstalled());
		ASSERT_FALSE(i.isLaunchable());
		ASSERT_FALSE(i.isDownloadable());
		ASSERT_TRUE(i.isDeleted());
		ASSERT_FALSE(!!i.getCurrentBranch());

		WildcardManager wildcard;

		{
			static const char* gs_szWildcardXml =
				"<wcards>"
				"<wcard name=\"PROGRAM_FILES\" type=\"special\">PROGRAM_FILES</wcard>"
				"</wcards>";

			tinyxml2::XMLDocument doc;
			doc.Parse(gs_szWildcardXml);
			wildcard.parseXML(doc.RootElement());
		}

		wildcard.onNeedSpecialEvent += delegate((ItemInfoThirdPartyFixture*)this, &ItemInfoThirdPartyFixture::resolveWildcard);
		wildcard.onNeedInstallSpecialEvent += delegate((ItemInfoThirdPartyFixture*)this, &ItemInfoThirdPartyFixture::resolveWildcard);

		EXPECT_CALL(m_ItemManager, getOnNewItemEvent()).Times(AtLeast(1)).WillRepeatedly(Return(&m_NewItemEvent));

		XML::gcXMLDocument doc;
		doc.LoadBuffer(szThirdPartyInfo, strlen(szThirdPartyInfo));

		i.loadXmlData(100, doc.GetRoot("game"), 0, &wildcard, false);

		ASSERT_TRUE(i.isInstalled());
		ASSERT_TRUE(i.isLaunchable());
		ASSERT_FALSE(i.isDownloadable());
		ASSERT_FALSE(i.isDeleted());

		ASSERT_TRUE(!!i.getCurrentBranch());
	}


}

#endif