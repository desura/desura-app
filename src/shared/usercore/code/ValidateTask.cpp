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
#include "ValidateTask.h"
#include "UserTasks.h"
#include "User.h"

#include "McfManager.h"
#include "MCFDownloadProviders.h"

#include "ToolManager.h"
#ifdef WIN32
#include "GameExplorerManager.h"
#else
#include "util/UtilLinux.h"
#endif

using namespace UserCore::ItemTask;

ValidateTask::ValidateTask(UserCore::Item::ItemHandle* handle, MCFBranch branch, MCFBuild build) 
	: BaseItemTask(UserCore::Item::ITEM_STAGE::STAGE_VALIDATE, "Validate", handle, branch, build)
{
	onErrorEvent += delegate(this, &ValidateTask::onError);
}

ValidateTask::~ValidateTask()
{
}

void ValidateTask::doRun()
{
	UserCore::MCFManagerI *mm = getUserCore()->getInternal()->getMCFManager();
	auto pItem = getItemInfo();

	if (!pItem)
		throw gcException(ERR_BADID);

	m_LastInsBuild = pItem->getInstalledBuild();

	MCFBuild build;
	MCFBranch branch;

	updateStatusFlags();

	m_hMCFile->setHeader(getItemId(), getMcfBranch(), getMcfBuild());
	m_hMCFile->getErrorEvent() += delegate(&onErrorEvent);
	m_hMCFile->getProgEvent() += delegate(this, &ValidateTask::onProgress);

	auto dp = std::make_shared<MCFDownloadProviders>(getWebCore(), getUserCore()->getUserId());
	MCFDownloadProviders::forceLoad(m_hMCFile, dp);

	m_bUnAuthed = dp->isUnAuthed();

	validateHeader(build, branch);

	UserCore::Item::BranchInfoI* curBranch = pItem->getCurrentBranch();

	if (isStopped())
		return;

	if (!curBranch)
		throw gcException(ERR_NULLHANDLE, "Current branch is nullptr");

	gcString savePath = mm->getMcfPath(getItemId(), curBranch->getBranchId(), build, m_bUnAuthed);
		
	if (savePath == "")
		savePath = mm->newMcfPath(getItemId(), curBranch->getBranchId(), build, m_bUnAuthed);

	if (isStopped())
		return;

	m_bLocalMcf = UTIL::FS::isValidFile(savePath);
	m_szInstallPath = getItemInfo()->getPath();

	setCurrentMcf(&m_hMCFile);
	m_hMCFile->dlHeaderFromWeb();
	setCurrentMcf(nullptr);

	m_hMCFile->setFile(savePath.c_str());

	if (isStopped())
		return;

	bool complex = getItemInfo()->isComplex();
	m_bLeaveLocalFiles = !HasAllFlags(getItemInfo()->getOptions(), UserCore::Item::ItemInfoI::OPTION_REMOVEFILES) && UTIL::FS::isValidFolder(m_szInstallPath) && !complex;

	if (m_bLocalMcf)
	{
		setAction(ACTION::CHECK_EXISTINGMCF);

		try
		{
			if (checkExistingMcf(savePath))
			{
				//complete
				onComplete(savePath);
				return;
			}
			else
			{
				//local mcf. Reset every thing
				m_hMCFile->getErrorEvent() += delegate(&onErrorEvent);
				m_hMCFile->getProgEvent() += delegate(this, &ValidateTask::onProgress);

				auto dp = std::make_shared<MCFDownloadProviders>(getWebCore(), getUserCore()->getUserId());
				MCFDownloadProviders::forceLoad(m_hMCFile, dp);

				m_bUnAuthed = dp->isUnAuthed();
			}
		}
		catch (gcException &e)
		{
			//something failed, delete the file and continue
			Warning(gcString("Failed to parse MCF file, removing it: {0}\n", e));
			UTIL::FS::delFile(savePath);
		}
	}

	if (isStopped())
		return;

	getUserCore()->updateUninstallInfo(getItemId(), m_hMCFile->getINSize());

	setAction(ACTION::CHECK_LOCALMCFS);
	copyLocalMcfs(branch, build);

	if (m_bLeaveLocalFiles)
	{
		setAction(ACTION::CHECK_LOCALFILES);
		copyLocalFiles();
	}

	if (m_bUnAuthed)
		m_hMCFile->getHeader()->addFlags(MCFCore::MCFHeaderI::FLAG_NOCLEANUP);

	setAction(ACTION::PRE_ALLOCATING);

	setCurrentMcf(&m_hMCFile);
	m_hMCFile->preAllocateFile();
	setCurrentMcf(nullptr);

	if (isStopped())
		return;

	m_hMCFile->saveMCFHeader();

	if (isStopped())
		return;

	onComplete(savePath);
}

void ValidateTask::setAction(ACTION action)
{
	m_Action = action;

	MCFCore::Misc::ProgressInfo p;
	onProgress(p);
}

void ValidateTask::updateStatusFlags()
{
	auto pItem = getItemInfo();
	uint32 flags = UserCore::Item::ItemInfoI::STATUS_DELETED|UserCore::Item::ItemInfoI::STATUS_LINK|UserCore::Item::ItemInfoI::STATUS_VERIFING|UserCore::Item::ItemInfoI::STATUS_PAUSED|UserCore::Item::ItemInfoI::STATUS_PRELOADED;

	pItem->getInternal()->setPercent(0);
	pItem->delSFlag(flags);

	uint32 num = 0;
	getUserCore()->getItemsAddedEvent()->operator()(num);

	MCFBuild build  = getMcfBuild();
	MCFBranch branch = getMcfBranch();

	m_bUpdating = pItem->isUpdating() && branch == pItem->getInstalledBranch() && (build == 0 || build == pItem->getNextUpdateBuild());

	if (m_bUpdating)
	{
		pItem->delSFlag(UserCore::Item::ItemInfoI::STATUS_READY);
		pItem->addSFlag(UserCore::Item::ItemInfoI::STATUS_UPDATING);
		pItem->delOFlag(UserCore::Item::ItemInfoI::OPTION_REMOVEFILES);
	}
	else
	{
		pItem->addSFlag(UserCore::Item::ItemInfoI::STATUS_ONCOMPUTER|UserCore::Item::ItemInfoI::STATUS_DOWNLOADING);
	}
}

void ValidateTask::validateHeader(MCFBuild &build, MCFBranch &branch)
{
	auto pItem = getItemInfo();
	build = m_hMCFile->getHeader()->getBuild();
	branch = m_hMCFile->getHeader()->getBranch();

	if (getMcfBranch() != branch)
		throw gcException(ERR_BADID, "Branch from mcf is different to requested branch");

	this->m_uiMcfBuild = build;

	if (m_bUpdating && build <=  pItem->getInstalledBuild())
		throw gcException(ERR_NOUPDATE, "The installed version is the same as the newest version. No Update available.");

	if (!pItem->setInstalledMcf(branch, build))
		throw gcException(ERR_BADID, "Failed to set branch id.");

#ifdef WIN32
	getUserCore()->getGameExplorerManager()->addItem(getItemId());
#endif

	if (m_bUnAuthed)
		pItem->addSFlag(UserCore::Item::ItemInfoI::STATUS_UNAUTHED);
}

bool ValidateTask::checkExistingMcf(gcString savePath)
{
	McfHandle mcfTemp;
	mcfTemp->setFile(savePath.c_str());

	setCurrentMcf(&mcfTemp);
	mcfTemp->parseMCF();
	setCurrentMcf(nullptr);

	if (isStopped())
		return false;

	mcfTemp->getProgEvent() += delegate(this, &ValidateTask::onProgress);

	bool idMatch = mcfTemp->getHeader()->getDesuraId() == getItemId();
	bool branchMatch = mcfTemp->getHeader()->getBranch() == getMcfBranch();
	bool buildMatch = mcfTemp->getHeader()->getBuild() == getMcfBuild();

	if (!idMatch || !branchMatch || !buildMatch)
	{
		Warning(gcString("Mcf Header didnt match: ID:{0}, Branch:{1}, Build:{2}, Path: [{3}]\n", idMatch, branchMatch, buildMatch, savePath));
		throw gcException(ERR_BADHEADER, "Mcf header didnt match required header");
	}

	uint32 count = 0;
	m_hMCFile->getPatchStats(mcfTemp.handle(), nullptr, &count);

	// we might have a diff mcf here
	if (count != 0)
		throw gcException(ERR_INVALIDFILE, "The mcf file contains different files to what it should have");

	bool complete;
	bool verify;
	m_CurMcfIndex = 1;

	setCurrentMcf(&mcfTemp);
	verify = mcfTemp->verifyMCF();
	setCurrentMcf(nullptr);

	McfHandle curMcf;
	bool useCurMcf = false;

	if (m_bLeaveLocalFiles)
	{
		try
		{
			m_CurMcfIndex = 2;

			setCurrentMcf(&curMcf);
			curMcf->parseFolder(m_szInstallPath.c_str(), false, true);
			curMcf->hashFiles(mcfTemp.handle());
			setCurrentMcf(nullptr);

			useCurMcf = true;
		}
		catch (gcException)
		{
		}
	}

	if (useCurMcf)
		complete = mcfTemp->isComplete(curMcf.handle());
	else
		complete = mcfTemp->isComplete();

	//if this is a fully validated MCF then we dont have to download anything
	if (verify && complete)
		return true;

	if (!complete)
		mcfTemp->resetSavedFiles();

	m_hMCFile.setHandle(mcfTemp.releaseHandle());
	return false;
}

void ValidateTask::copyLocalMcfs(MCFBranch branch, MCFBuild build)
{
	if (isStopped())
		return;

	UserCore::MCFManagerI *mm = getUserCore()->getInternal()->getMCFManager();

	std::vector<McfPathData> vOldMcfs;
	mm->getAllMcfPaths(getItemId(), vOldMcfs);

	std::vector<McfHandle> vHandleList;

	for (size_t x=0; x<vOldMcfs.size(); x++)
	{
		if (!UTIL::FS::isValidFile(vOldMcfs[x].path))
			continue;

		//exclude this mcf from list
		if (vOldMcfs[x].branch == branch && vOldMcfs[x].build == build)
			continue;

		McfHandle tempMcf;
		tempMcf->setFile(vOldMcfs[x].path.c_str());

		uint32 count = 0;

		try
		{
			tempMcf->parseMCF();
			m_hMCFile->getPatchStats(tempMcf.handle(), nullptr, &count);
		}
		catch (gcException &)
		{
			continue;
		}

		//if there are some files the same (getPatchStats returns number of files that are diff)
		if (count != tempMcf->getFileCount())
		{
			m_vMcfFileCountList.push_back(count);
			vHandleList.push_back(tempMcf.releaseHandle());
		}
	}

	std::for_each(m_vMcfFileCountList.begin(), m_vMcfFileCountList.end(), [this](uint32 c){
		m_TotalFileCount += c;
	});

	for (size_t x=0; x<vHandleList.size(); x++)
	{
		m_CurMcfIndex = x;

		if (!vHandleList[x].handle())
			continue;

		McfHandle tempMcf(vHandleList[x]);

		try
		{
			m_hMCFile->copyMissingFiles(vHandleList[x].handle());
		}
		catch (gcException &)
		{
		}

		if (m_bUnAuthed == false && vOldMcfs[x].branch == branch && vOldMcfs[x].build < build)
			mm->delMcfPath(getItemInfo()->getId(), vOldMcfs[x].branch, vOldMcfs[x].build);
	}
}

void ValidateTask::copyLocalFiles()
{
	if (isStopped())
		return;

	//! Dont bother doing this for complex installs
	if (getItemInfo()->isComplex())
		return;

	UTIL::FS::Path tempPath(getUserCore()->getAppDataPath(), gcString("{0}_{1}.mcf", getItemId().toInt64(), time(nullptr)), false);
	tempPath += "temp";

	McfHandle curMcf;
	curMcf->setFile(tempPath.getFullPath().c_str());
	curMcf->getProgEvent() += delegate(this, &ValidateTask::onLocalFileProgress);

	try
	{
		m_CurMcfIndex = 1;

		setCurrentMcf(&curMcf);
		curMcf->parseFolder(m_szInstallPath.c_str(), false, true);
		curMcf->hashFiles(m_hMCFile.handle());
		setCurrentMcf(nullptr);
	}
	catch (gcException &e)
	{
		Debug(gcString("Failed to parse folder: {0}", e));
	}

	// see if the files are on the local system
	try
	{
		m_CurMcfIndex = 2;
		m_TotalFileCount = 0;
		m_hMCFile->getPatchStats(curMcf.handle(), nullptr, &m_TotalFileCount);
		
		if (m_TotalFileCount != m_hMCFile->getFileCount())
		{
			curMcf->markFiles(m_hMCFile.handle(), true, false, false, false);
			curMcf->saveMCF();

			m_CurMcfIndex = 3;
			m_hMCFile->copyMissingFiles(curMcf.handle());
		}
	}
	catch (gcException &e)
	{
		Debug(gcString("Failed to copy files from local mcf: {0}", e));
	}

	UTIL::FS::delFile(tempPath);
}


void ValidateTask::onComplete(gcString &savePath)
{
	if (m_bInError || isStopped())
		return;

	onCompleteStrEvent(savePath);

	if (m_bUpdating && m_LastInsBuild != 0)
		getItemHandle()->getInternal()->goToStageUninstallUpdate(savePath.c_str(), m_LastInsBuild);
	else
		getItemHandle()->getInternal()->goToStageDownload(savePath.c_str());
}



void ValidateTask::onPause()
{
	m_hMCFile->pause();
}

void ValidateTask::onUnpause()
{
	m_hMCFile->unpause();
}


void ValidateTask::onLocalFileProgress(MCFCore::Misc::ProgressInfo& p)
{
	if (m_CurMcfIndex == 2)
	{
		p.doneAmmount= p.doneAmmount*100/p.totalAmmount;
		p.totalAmmount = 100;
	}

	onProgress(p);
}

void ValidateTask::onProgress(MCFCore::Misc::ProgressInfo& p)
{
	uint32 base = 33;
	int32 multi = 0;
	uint32 divid = 3;

	if (!m_bLeaveLocalFiles && !m_bLocalMcf)
	{
		base = 0;
		divid = 1;
		multi = 0;
	}
	else if (m_bLocalMcf && !m_bLeaveLocalFiles)
	{
		base = 50;
		divid = 2;
		multi = 0;
	}
	else if (!m_bLocalMcf && m_bLeaveLocalFiles)
	{
		base = 50;
		divid = 2;
		multi = -1;
	}

	if (m_Action == ACTION::CHECK_LOCALMCFS)
	{
		uint32 baseDone = 0;

		for (size_t x=0; x<m_CurMcfIndex; x++)
		{
			baseDone += m_vMcfFileCountList.size();
		}

		if (m_CurMcfIndex >= m_vMcfFileCountList.size())
		{
			p.percent = base*(multi);
		}
		else
		{
			double curSlice = (double)m_vMcfFileCountList[m_CurMcfIndex]/(double)m_TotalFileCount;
			double curPer = ((double)baseDone*100.0)/(double)m_TotalFileCount + curSlice*p.percent;

			p.percent = (uint8)(base*(multi) + curPer);
		}
		p.flag = 1 + (1<<4);
	}
	else if (m_Action == ACTION::CHECK_EXISTINGMCF)
	{
		uint32 curPer = p.percent/2;

		if (m_CurMcfIndex == 2)
			curPer += 50;

		p.percent = base*(multi+1) + curPer/divid;
		p.flag = 2 + (m_CurMcfIndex<<4);
	}
	else if (m_Action == ACTION::CHECK_LOCALFILES)
	{
		uint32 curPer = p.percent/3;

		if (m_CurMcfIndex == 2)
			curPer += 33;
		else if (m_CurMcfIndex == 3)
			curPer += 66;

		p.percent = base*(multi+2) + curPer/divid;
		p.flag = 3 + (m_CurMcfIndex<<4);
	}
	else if (m_Action == ACTION::PRE_ALLOCATING)
	{
		p.flag = 4 + (m_CurMcfIndex<<4);
	}

	onMcfProgressEvent(p);
	getItemInfo()->getInternal()->setPercent(p.percent);
}

void ValidateTask::onError(gcException &e)
{
	Warning(gcString("Error in MCF validate: {0}\n", e));
	m_bInError=true;
	getItemHandle()->getInternal()->completeStage(true);

	auto pItem = getItemInfo();

	if (pItem)
	{
		if (m_bUpdating)
		{
			pItem->delSFlag(UserCore::Item::ItemInfoI::STATUS_UPDATING);
			pItem->addSFlag(UserCore::Item::ItemInfoI::STATUS_READY);
		}
		else
		{
			pItem->delSFlag(UserCore::Item::ItemInfoI::STATUS_DOWNLOADING);
		}
	}
}

void ValidateTask::onStop()
{
	UserCore::ItemTask::BaseItemTask::onStop();

	std::lock_guard<std::mutex> guard(m_McfLock);

	if (m_CurrentMcf)
		m_CurrentMcf->handle()->stop();
}

void ValidateTask::cancel()
{
	onStop();
	getItemHandle()->getInternal()->resetStage(true);
}

void ValidateTask::setCurrentMcf(McfHandle* pMcfHandle)
{
	std::lock_guard<std::mutex> guard(m_McfLock);
	m_CurrentMcf = pMcfHandle;
}