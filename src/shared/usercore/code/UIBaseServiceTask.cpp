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
#include "UIBaseServiceTask.h"
#include "McfManager.h"

#include "mcfcore/MCFI.h"
#include "mcfcore/MCFHeaderI.h"
#include "mcfcore/MCFMain.h"
#include "MCFDownloadProviders.h"

using namespace UserCore::ItemTask;


UIBaseServiceTask::UIBaseServiceTask(UserCore::Item::ITEM_STAGE stage, const char* taskname, UserCore::Item::ItemHandle* handle, MCFBranch installBranch, MCFBuild installBuild) 
	: BaseItemServiceTask(stage, taskname, handle, installBranch, installBuild)
{
}

UIBaseServiceTask::~UIBaseServiceTask()
{
	gcTrace("");
}

bool UIBaseServiceTask::initService()
{
	gcTrace("");

	auto pItem = getItemInfo();

	if (!pItem)
		return false;

	pItem->delSFlag(	UserCore::Item::ItemInfoI::STATUS_INSTALLED|
						UserCore::Item::ItemInfoI::STATUS_READY|
						UserCore::Item::ItemInfoI::STATUS_DOWNLOADING|
						UserCore::Item::ItemInfoI::STATUS_VERIFING|
						UserCore::Item::ItemInfoI::STATUS_INSTALLING|
						UserCore::Item::ItemInfoI::STATUS_UPDATING|
						UserCore::Item::ItemInfoI::STATUS_ONCOMPUTER|
						UserCore::Item::ItemInfoI::STATUS_PAUSED|
						UserCore::Item::ItemInfoI::STATUS_PAUSABLE);

	if (!pItem->isDownloadable() || HasAllFlags(pItem->getStatus(), UserCore::Item::ItemInfoI::STATUS_LINK))
	{
		pItem->delSFlag(UserCore::Item::ItemInfoI::STATUS_LINK);
		return false;
	}

	m_OldBranch = getItemInfo()->getInstalledBranch();
	m_OldBuild = getItemInfo()->getInstalledBuild();

	gcString installPath = getItemInfo()->getPath();

	if (installPath == "")
		return false;

	return true;
}

void UIBaseServiceTask::onServiceError(gcException& e)
{
	onComplete();
}

gcString UIBaseServiceTask::getBranchMcf(DesuraId id, MCFBranch branch, MCFBuild build)
{
	UserCore::MCFManagerI *mm = getUserCore()->getInternal()->getMCFManager();
	gcString filePath = mm->getMcfPath(id, branch, build);

	if (filePath == "" || !checkPath(filePath.c_str(), branch, build))
	{
		if (filePath == "")
			filePath =  mm->newMcfPath(id, branch, build);

		try
		{
			McfHandle mcfHandle;
			mcfHandle->setHeader(id, branch, build);
			mcfHandle->setFile(filePath.c_str());

			auto dp = std::make_shared<MCFDownloadProviders>(getWebCore(), getUserCore()->getUserId());
			MCFDownloadProviders::forceLoad(mcfHandle, dp);

			mcfHandle->dlHeaderFromWeb();
			mcfHandle->saveBlankMcf();
		}
		catch (gcException &)
		{
		}

		if (!checkPath(filePath.c_str(), branch, build))
			filePath = "";
	}

	return filePath;
}


bool UIBaseServiceTask::checkPath(const char* path, MCFBranch branch, MCFBuild build)
{
	if (!UTIL::FS::isValidFile(UTIL::FS::PathWithFile(path)))
		return false;

	McfHandle mcfHandle;
	mcfHandle->setFile(path);

	try
	{
		mcfHandle->parseMCF();
	}
	catch (gcException &except)
	{
		Warning("Uninstall Branch: MCF Error: {0}\n", except);
		return false;
	}

	MCFCore::MCFHeaderI *mcfHead = mcfHandle->getHeader();
	return (mcfHead && (build == 0 || mcfHead->getBuild() == build) && mcfHead->getBranch() == branch);
}

void UIBaseServiceTask::onStop()
{
	BaseItemServiceTask::onStop();
}

void UIBaseServiceTask::completeUninstall(bool removeAll, bool removeAccount)
{
	gcTrace("");

#ifdef NIX
	removeScripts();
#endif
	
	MCFCore::Misc::ProgressInfo prog;
	prog.percent = 100;
	onMcfProgressEvent(prog);

	auto pItem = getItemInfo();
	pItem->getInternal()->resetInstalledMcf();

	getUserCore()->removeUninstallInfo(getItemId());

	if (removeAll)
	{
		UserCore::MCFManagerI *mm = getUserCore()->getInternal()->getMCFManager();
		mm->delAllMcfPath(getItemId());

		UTIL::FS::Path path(getUserCore()->getAppDataPath(), "", false);
		path += pItem->getId().getFolderPathExtension();

		UTIL::FS::delFolder(path);
	}

	if (removeAccount)
	{
		pItem->removeFromAccount();
		getUserCore()->getItemManager()->removeItem(pItem->getId());
	}
}

void UIBaseServiceTask::onComplete()
{
	gcTrace("");

	uint32 com = 0;
	onCompleteEvent(com);
	onFinish();
}


#ifdef NIX
void UIBaseServiceTask::removeScripts()
{
	UserCore::Item::ItemInfoI* item = getItemInfo();
	
	if (!item)
		return;
	
	const char* insPath = item->getPath();
	
	if (!insPath)
		return;
	
	UTIL::FS::Path path(insPath, "", false);
	std::vector<std::string> filter;
	filter.push_back("sh");
	
	std::vector<UTIL::FS::Path> out;
	UTIL::FS::getAllFiles(path, out, &filter);
	
	for (size_t x=0; x<out.size(); x++)
	{
		if (out[x].getFile().getFile().find("desura_launch") == 0)
			UTIL::FS::delFile(out[x]);
	}
	
	UTIL::FS::delEmptyFolders(path);
}
#endif