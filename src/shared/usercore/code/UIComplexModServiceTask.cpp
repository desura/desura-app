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
#include "UIComplexModServiceTask.h"

#include "IPCServiceMain.h"
#include "IPCComplexLaunch.h"

#include "McfManager.h"

enum
{
	CA_NONE,
	CA_INSTALL,
	CA_UNINSTALL_BRANCH,
	CA_UNINSTALL,
	CA_UIPATCH,
	CA_LAUNCH,
};



namespace UserCore
{
namespace ItemTask
{


UIComplexModServiceTask::UIComplexModServiceTask(UserCore::Item::ItemHandle* handle, MCFBranch installBranch, MCFBuild installBuild) : UIBaseServiceTask(UserCore::Item::ITEM_STAGE::STAGE_UNINSTALL_COMPLEX, "UnInstallComplex", handle, installBranch, installBuild)
{
	m_pIPCCL = nullptr;

	m_bRemoveAll = false;
	m_bRemoveAcc = false;

	m_uiCompleteAction = CA_NONE;
	m_bEndStage = false;
}

UIComplexModServiceTask::~UIComplexModServiceTask()
{
	waitForFinish();

	if (m_pIPCCL)
		m_pIPCCL->destroy();

	m_pIPCCL = nullptr;
}


bool UIComplexModServiceTask::initService()
{
	m_OldBranch = getItemInfo()->getInstalledBranch();
	m_OldBuild = getItemInfo()->getInstalledBuild();

	auto parentInfo = getParentItemInfo();

	if (getItemInfo()->getInstalledModId().isOk())
		parentInfo = getItemInfo();

	if (!parentInfo)
	{
		onComplete();
		return false;
	}

	m_idLastInstalledMod = parentInfo->getInstalledModId();
	getUserCore()->getItemManager()->setInstalledMod(parentInfo->getId(), DesuraId());

	if (!m_idLastInstalledMod.isOk())
	{
		onComplete();
		return false;
	}

	UserCore::Item::ItemInfo* modInfo = dynamic_cast<UserCore::Item::ItemInfo*>(getUserCore()->getItemManager()->findItemInfo(m_idLastInstalledMod));

	if (!modInfo)
	{
		gcException eModNoExist(ERR_NULLHANDLE, "Installed mod doesnt exist in database!\n");
		onErrorEvent(eModNoExist);
		return false;
	}

	m_pIPCCL = getServiceMain()->newComplexLaunch();

	if (!m_pIPCCL)
	{
		gcException eFailedUninstall (ERR_NULLHANDLE, "Failed to create uninstall complex branch mcf service!\n");
		onErrorEvent(eFailedUninstall);
		return false;
	}

	UserCore::MCFManagerI *mm = getUserCore()->getInternal()->getMCFManager();

	gcString installPath = modInfo->getPath();
	gcString parPath = mm->getMcfBackup(parentInfo->getId(), m_idLastInstalledMod);
	gcString modMcfPath;

	if (m_idLastInstalledMod == getItemInfo()->getId())
		modMcfPath = getBranchMcf(modInfo->getId(), m_OldBranch, m_OldBuild);
	else
		modMcfPath = getBranchMcf(modInfo->getId(), modInfo->getInstalledBranch(), modInfo->getInstalledBuild());

	if (m_uiCompleteAction == CA_UNINSTALL)
		modInfo->resetInstalledMcf();

	m_pIPCCL->onCompleteEvent += delegate(this, &UIComplexModServiceTask::onComplete);
	m_pIPCCL->onProgressEvent += delegate(this, &UIComplexModServiceTask::onProgress);
	m_pIPCCL->onErrorEvent += delegate((UIBaseServiceTask*)this, &UIBaseServiceTask::onServiceError);
	m_pIPCCL->startRemove(modMcfPath.c_str(), parPath.c_str(), installPath.c_str(), getItemInfo()->getInstallScriptPath());	

	return true;
}

void UIComplexModServiceTask::onProgress(MCFCore::Misc::ProgressInfo& p)
{
	onMcfProgressEvent(p);
}

void UIComplexModServiceTask::onComplete()
{
	if (HasAllFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_INSTALLCOMPLEX))
	{
		UserCore::MCFManagerI *mm = getUserCore()->getInternal()->getMCFManager();
		mm->delMcfBackup(getItemInfo()->getParentId(), m_idLastInstalledMod);
	}

	if (HasAllFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_LINK))
	{
		if (m_uiCompleteAction == CA_LAUNCH)
			getItemHandle()->getInternal()->goToStageLaunch();
	}
	else
	{
		switch (m_uiCompleteAction)
		{
		case CA_INSTALL:
			getItemHandle()->getInternal()->goToStageInstallComplex(getMcfBranch(), getMcfBuild());
			break;

		case CA_UNINSTALL_BRANCH:
			getItemHandle()->getInternal()->goToStageUninstallBranch(getMcfBranch(), getMcfBuild());
			break;

		case CA_UNINSTALL:
			getItemInfo()->delSFlag(	UserCore::Item::ItemInfoI::STATUS_INSTALLED|
										UserCore::Item::ItemInfoI::STATUS_READY|
										UserCore::Item::ItemInfoI::STATUS_DOWNLOADING|
										UserCore::Item::ItemInfoI::STATUS_VERIFING|
										UserCore::Item::ItemInfoI::STATUS_INSTALLING|
										UserCore::Item::ItemInfoI::STATUS_UPDATING|
										UserCore::Item::ItemInfoI::STATUS_ONCOMPUTER|
										UserCore::Item::ItemInfoI::STATUS_PAUSED|
										UserCore::Item::ItemInfoI::STATUS_PAUSABLE);

			completeUninstall(m_bRemoveAll, m_bRemoveAcc);
			break;

		case CA_UIPATCH:
			getItemHandle()->getInternal()->goToStageUninstallPatch(getMcfBranch(), getMcfBuild());
			break;
		};
	}

	if (m_bEndStage)
		getItemHandle()->getInternal()->completeStage(true);

	UIBaseServiceTask::onComplete();
}

void UIComplexModServiceTask::setCAUIPatch()
{
	m_uiCompleteAction = CA_UIPATCH;
}

void UIComplexModServiceTask::setCAInstall()
{
	m_uiCompleteAction = CA_INSTALL;
}

void UIComplexModServiceTask::setCAUninstallBranch()
{
	m_uiCompleteAction = CA_UNINSTALL_BRANCH;
}

void UIComplexModServiceTask::setCAUninstall(bool removeAll, bool removeAccount)
{
	m_uiCompleteAction = CA_UNINSTALL;
	m_bRemoveAll = removeAll;
	m_bRemoveAcc = removeAccount;
}

void UIComplexModServiceTask::setEndStage()
{
	m_bEndStage = true;
}

void UIComplexModServiceTask::setCALaunch()
{
	m_uiCompleteAction = CA_LAUNCH;
}


}
}
