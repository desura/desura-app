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
#include "CreateMCFThread.h"

#include "McfManager.h"
#include "ItemManager.h"
#include "util/gcTime.h"
#include "MCFDownloadProviders.h"

#include "User.h"

namespace UserCore
{
namespace Thread
{
CreateMCFThread::CreateMCFThread(DesuraId id, const char* path) : MCFThread( "CreateMCF Thread", id )
{
	m_szPath = gcString(path);
	m_bComplete = false;
}

void CreateMCFThread::waitForItemInfo()
{
	m_pUserCore->getItemManager()->retrieveItemInfoAsync(getItemId());

	size_t count = 0;

	while (!isStopped() && !getItemInfo() && count < 100)
	{
		count++;
		gcSleep(500);
	}
}

void CreateMCFThread::doRun()
{
	UserCore::Item::ItemInfo *item = getItemInfo();

	if (!item && !getUserCore()->isAdmin())
		throw gcException(ERR_BADITEM);

	if (!item)
	{
		waitForItemInfo();
		item = getItemInfo();
	}

	if (isStopped())
		return;

	if (!item)
		throw gcException(ERR_BADITEM);

	const char* val = getUserCore()->getCVarValue("gc_corecount");

	if (val && Safe::atoi(val) != 0)
		m_hMCFile->setWorkerCount(Safe::atoi(val));


	m_hMCFile->parseFolder(m_szPath.c_str(), true);

	if (m_hMCFile->getFileCount() == 0)
		throw gcException(ERR_ZEROFILECOUNT, gcString("There are no files in the folder {0}. \nPlease check to see if you have the correct folder and try again.", m_szPath));


	const gcString strNoCreatePatch = getUserCore()->getCVarValue("gc_mcfcreate_nopatch");

	if (item->getBranchCount() == 0 || strNoCreatePatch == "1" || strNoCreatePatch == "true")
	{
		m_hMCFile->setHeader(getItemId(), MCFBranch(), MCFBuild());
		m_hMCFile->getHeader()->setParent(0);
	}
	else
	{
		std::vector<UserCore::Item::BranchInfo*> vBranchList;

		try
		{
			retrieveBranchList(vBranchList);

			if (vBranchList.size() != 0)
				compareBranches(vBranchList);
		}
		catch (...)
		{
		}

		safe_delete(vBranchList);
	}


	if (isStopped())
		return;

	createMcf();

	if (isStopped())
		return;

	m_bComplete = true;
	onCompleteStrEvent(m_szFilePath);	
}

void CreateMCFThread::compareBranches(std::vector<UserCore::Item::BranchInfo*> &vBranchList)
{
	uint64 lastSize = 0;

	McfHandle lastMcf;

	m_hMCFile->setHeader(getItemId(), MCFBranch(), MCFBuild());
	m_hMCFile->getHeader()->setParent(0);


	for (size_t x=0; x<vBranchList.size(); x++)
	{
		if (isStopped())
			return;

		UserCore::Item::BranchInfo* bi = vBranchList[x];

		McfHandle tempMcf;
		tempMcf->setHeader(getItemId(), bi->getBranchId(), MCFBuild());
		tempMcf->getErrorEvent() += delegate(&onErrorEvent);
	
		try
		{
			auto dp = std::make_shared<MCFDownloadProviders>(getWebCore(), getUserCore()->getUserId());
			MCFDownloadProviders::forceLoad(tempMcf, dp);
		}
		catch (gcException &except)
		{
			Warning("CreateMCF: Failed to get download providers for mcf: {0}\n", except);
			continue;
		}

		if (tempMcf->getHeader()->getBuild() > 0)
		{
			try
			{
				tempMcf->dlHeaderFromWeb();
			}
			catch (gcException &except)
			{
				Warning("CreateMCF: Failed to get dlHeaderFromWeb for mcf: {0}\n", except);
				continue;
			}

			uint64 totalSize = 0;
			uint32 fileCount = 0;
			m_hMCFile->getPatchStats(tempMcf.handle(), &totalSize, &fileCount);

			if (lastSize == 0 || totalSize < lastSize)
			{
				lastSize = totalSize;
				lastMcf.setHandle(tempMcf.releaseHandle());

				// if there are no files just use this one
				if (fileCount == 0)
					break;
			}
		}
	}

	MCFBuild build = lastMcf->getHeader()->getBuild();
	MCFBranch branch = lastMcf->getHeader()->getBranch();

	m_hMCFile->setHeader(getItemId(), branch, build);

	if (build > 0)
		m_hMCFile->getHeader()->addFlags( MCFCore::MCFHeaderI::FLAG_PARTFILE );
	else
		m_hMCFile->getHeader()->delFlags( MCFCore::MCFHeaderI::FLAG_PARTFILE );

	m_hMCFile->getHeader()->setParent(lastMcf->getHeader()->getBuild());
	m_hMCFile->makePatch(lastMcf.handle());
}

void CreateMCFThread::createMcf()
{
	std::string timeStr = gcTime().to_iso_string();

	MCFBranch branch = m_hMCFile->getHeader()->getBranch();

	gcString file("NewMcf_b{0}_{1}.mcf", branch, timeStr);

	UserCore::MCFManagerI *mm = getUserCore()->getInternal()->getMCFManager();
	m_szFilePath = gcString("{0}{1}{2}", mm->getMcfSavePath(), DIRS_STR, getItemId().getFolderPathExtension(file.c_str()));


	m_hMCFile->setFile(m_szFilePath.c_str());
	m_hMCFile->getErrorEvent() += delegate(&onErrorEvent);
	m_hMCFile->getProgEvent() += delegate(&onMcfProgressEvent);
	m_hMCFile->saveMCF();
}




void CreateMCFThread::onPause()
{
	m_hMCFile->pause();
}

void CreateMCFThread::onUnpause()
{
	m_hMCFile->unpause();
}

void CreateMCFThread::onStop()
{
	//make sure we are not in a pause state
	m_hMCFile->unpause();

	//again events cause crashes let stop em in the tracks.
	m_hMCFile->stop();

	//need to join here so we can wait till the threads done with the file.
	join();

	if (!m_bComplete)
		UTIL::FS::delFile(UTIL::FS::PathWithFile(m_szFilePath));
}


void CreateMCFThread::retrieveBranchList(std::vector<UserCore::Item::BranchInfo*> &outList)
{
	XML::gcXMLDocument doc;
	getWebCore()->getItemInfo(getItemId(), doc, MCFBranch(), MCFBuild());

	auto uNode = doc.GetRoot("iteminfo");

	if (!uNode.IsValid())
		throw gcException(ERR_BADXML);

	uNode.FirstChildElement("platforms").for_each_child("platform", [this, &outList](const XML::gcXMLElement &platform)
	{
		this->processGames(outList, platform);
	});
}

void CreateMCFThread::processGames(std::vector<UserCore::Item::BranchInfo*> &outList, const XML::gcXMLElement &platform)
{
	platform.FirstChildElement("games").for_each_child("game", [this, &outList](const XML::gcXMLElement &game)
	{
		const std::string szId = game.GetAtt("siteareaid");
		DesuraId gid(szId.c_str(), "games");

		if (gid == this->getItemId())
			this->processBranches(outList, game);
	
		if (this->getItemId().getType() != DesuraId::TYPE_MOD)
			return;

		this->processMods(outList, game);
	});
}

void CreateMCFThread::processMods(std::vector<UserCore::Item::BranchInfo*> &outList, const XML::gcXMLElement &game)
{
	game.FirstChildElement("mods").for_each_child("mod", [this, &outList](const XML::gcXMLElement &mod)
	{
		const std::string szId = mod.GetAtt("siteareaid");
		DesuraId id(szId.c_str(), "mods");

		if (this->getItemId() != id)
			return;

		this->processBranches(outList, mod);
	});
}

void CreateMCFThread::processBranches(std::vector<UserCore::Item::BranchInfo*> &outList, const XML::gcXMLElement &item)
{
	item.FirstChildElement("branches").for_each_child("branch", [this, &outList](const XML::gcXMLElement &branch)
	{
		uint32 id = 0;
		branch.GetAtt("id", id);

		if (id == 0)
			return;

		UserCore::Item::BranchInfo *bi = new UserCore::Item::BranchInfo(MCFBranch::BranchFromInt(id), this->getItemId(), nullptr, 0, m_pUserCore->getUserId());
		bi->loadXmlData(branch);

		if (!HasAnyFlags(bi->getFlags(), UserCore::Item::BranchInfoI::BF_NORELEASES))
			outList.push_back(bi);
		else
			safe_delete(bi);
	});
}


}
}
