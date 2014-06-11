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
#include "ItemHandleEvents.h"

#include "ComplexLaunchServiceTask.h"
#include "InstallServiceTask.h"
#include "DownloadTask.h"
#include "GatherInfoTask.h"
#include "VerifyServiceTask.h"
#include "InstallCheckTask.h"
#include "LaunchTask.h"
#include "DownloadToolItemTask.h"
#include "ValidateTask.h"
#include "InstallToolTask.h"

#include "UIBranchServiceTask.h"
#include "UIServiceTask.h"
#include "UIComplexModServiceTask.h"
#include "UIPatchServiceTask.h"
#include "UIUpdateServiceTask.h"

#include "managers/Managers.h"
#include "usercore/MCFThreadI.h"

#include "ItemThread.h"
#include "ItemTaskGroup.h"

#include "User.h"

namespace
{
	class BlankTask : public UserCore::ItemTask::BaseItemTask
	{
	public:
		BlankTask(gcRefPtr<UserCore::Item::ItemHandleI> handle, UserCore::Item::ITEM_STAGE type)
			: BaseItemTask(type, "", handle)
		{
		}

		virtual void doRun()
		{
		}
	};
}

using namespace UserCore::Item;


template <>
std::string TraceClassInfo(ItemInfo *pClass);

template <>
std::string TraceClassInfo(ItemHandle *pClass)
{
    return gcString("[{0}] stage: {1}, {2}", (uint64)pClass, (uint32)pClass->getStage(), TraceClassInfo(pClass->getItemInfoNorm().get()));
}


ItemHandle::ItemHandle(gcRefPtr<ItemInfo> &itemInfo, gcRefPtr<UserCore::UserI> user)
	: m_pItemInfo(itemInfo)
	, m_pItemInfoI(itemInfo)
	, m_pUserCore(user)
	, m_pEventHandler(gcRefPtr<ItemHandleEvents>::create(m_HelperLock, m_vHelperList))
{
}

ItemHandle::~ItemHandle()
{
}

void ItemHandle::cleanup()
{
	std::lock_guard<std::mutex> guard(m_GroupLock);

	if (m_pGroup)
		m_pGroup->removeItem(this);

	m_pGroup = nullptr;
}

void ItemHandle::setFactory(gcRefPtr<Helper::ItemHandleFactoryI> factory)
{
	m_pFactory = factory;
}

const gcRefPtr<UserCore::UserI>& ItemHandle::getUserCore()
{
	return m_pUserCore;
}

const gcRefPtr<UserCore::Item::ItemInfoI>& ItemHandle::getItemInfo()
{
	return m_pItemInfoI;
}

const gcRefPtr<UserCore::Item::ItemInfo>& ItemHandle::getItemInfoNorm()
{
	return m_pItemInfo;
}

bool ItemHandle::getLock(void* obj)
{
#ifdef DESURA_DIABLO2_HACK
	DesuraId d2Id(50, DesuraId::TYPE_GAME);
	DesuraId lodId(13824, DesuraId::TYPE_GAME);

	if (m_pItemInfo->getId() == lodId)
	{
		auto handle = gcRefPtr<UserCore::Item::ItemHandle>::dyn_cast(m_pUserCore->getItemManager()->findItemHandle(d2Id));

		if (handle)
			return handle->getLock(obj);
	}
#endif

	if (!obj)
		return false;

	if (m_bLock)
		return (m_pLockObject == obj);

	m_bLock = true;
	m_pLockObject = obj;

	return true;
}

bool ItemHandle::isLocked()
{
#ifdef DESURA_DIABLO2_HACK
	DesuraId d2Id(50, DesuraId::TYPE_GAME);
	DesuraId lodId(13824, DesuraId::TYPE_GAME);

	if (m_pItemInfo->getId() == lodId)
	{
		auto handle = gcRefPtr<UserCore::Item::ItemHandle>::dyn_cast(m_pUserCore->getItemManager()->findItemHandle(d2Id));

		if (handle)
			return handle->isLocked();
	}
#endif


	return m_bLock;
}

bool ItemHandle::hasLock(void* obj)
{
#ifdef DESURA_DIABLO2_HACK
	DesuraId d2Id(50, DesuraId::TYPE_GAME);
	DesuraId lodId(13824, DesuraId::TYPE_GAME);

	if (m_pItemInfo->getId() == lodId)
	{
		auto handle = gcRefPtr<UserCore::Item::ItemHandle>::dyn_cast(m_pUserCore->getItemManager()->findItemHandle(d2Id));

		if (handle)
			return handle->hasLock(obj);
	}
#endif


	if (m_bLock)
		return (m_pLockObject == obj);

	return false;
}

void ItemHandle::releaseLock(void* obj)
{
#ifdef DESURA_DIABLO2_HACK
	DesuraId d2Id(50, DesuraId::TYPE_GAME);
	DesuraId lodId(13824, DesuraId::TYPE_GAME);

	if (m_pItemInfo->getId() == lodId)
	{
		auto handle = gcRefPtr<UserCore::Item::ItemHandle>::dyn_cast(m_pUserCore->getItemManager()->findItemHandle(d2Id));

		if (handle)
		{
			handle->releaseLock(obj);
			return;
		}
	}
#endif

	if (m_pLockObject != obj)
		return;

	m_pLockObject = nullptr;
	m_bLock = false;
}



void ItemHandle::delHelper(gcRefPtr<Helper::ItemHandleHelperI> helper)
{
	if (!helper)
		return;

	std::lock_guard<std::recursive_mutex> guard(m_HelperLock);

	auto it = std::remove(begin(m_vHelperList), end(m_vHelperList), helper);

	if (it != end(m_vHelperList))
		m_vHelperList.erase(it, end(m_vHelperList));
}

Event<ITEM_STAGE>& ItemHandle::getChangeStageEvent()
{
	return onChangeStageEvent;
}

Event<gcException>& ItemHandle::getErrorEvent()
{
	return onErrorEvent;
}

bool ItemHandle::isInStage()
{
	return m_uiStage != ITEM_STAGE::STAGE_NONE && m_uiStage != ITEM_STAGE::STAGE_CLOSE && m_uiStage != ITEM_STAGE::STAGE_LAUNCH;
}

bool ItemHandle::isStopped()
{
	return m_bStopped;
}

void ItemHandle::setPauseOnError(bool pause)
{
	m_bPauseOnError = pause;
}

bool ItemHandle::shouldPauseOnError()
{
	return m_bPauseOnError;
}

void ItemHandle::setPausable(bool state)
{
	if (state)
		getItemInfo()->addSFlag(UserCore::Item::ItemInfoI::STATUS_PAUSABLE);
	else
		getItemInfo()->delSFlag(UserCore::Item::ItemInfoI::STATUS_PAUSABLE);
}

//void ItemHandle::stop(bool block)
//{
//	if (!block)
//	{
//		 m_bStopped = true;
//	}
//	else
//	{
//		stopThread();
//	}
//}

void ItemHandle::setPaused(bool state, bool forced)
{
	gcTrace("Paused {0}, Forced {1}", state, forced);

	bool isPausable = (getItemInfo()->getStatus()&UserCore::Item::ItemInfoI::STATUS_PAUSABLE)?true:false;
	bool hasPauseFlag = HasAnyFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_PAUSED);

	if ((forced || isPausable) && (hasPauseFlag != state))
	{
		bool verify = false;

		{
			std::lock_guard<std::recursive_mutex> guard(m_ThreadMutex);

			if (state)
			{
				if (m_pThread && m_pThread->hasTaskToRun())
				{
					getItemInfo()->addSFlag(UserCore::Item::ItemInfoI::STATUS_PAUSED);
					m_pThread->pause();
				}
			}
			else
			{
				getItemInfo()->delSFlag(UserCore::Item::ItemInfoI::STATUS_PAUSED);

				if (m_pThread && m_pThread->hasTaskToRun())
					m_pThread->unpause();
				else if (startUpCheck())
					verify = true;
			}
		}

		if (verify)
			verifyOveride();

		m_pEventHandler->onPause(state);
	}
}

void ItemHandle::setPaused(bool state)
{
	setPaused(state, false);
}

void ItemHandle::setStage(ITEM_STAGE stage)
{
	gcTrace("Stage {0}", (int32)stage);

	m_pEventHandler->reset();
	m_uiStage = stage;
	onChangeStageEvent(stage);

	m_pItemInfo->setPercent(0);

	UserCore::Item::ItemInfoI::ItemInfo_s info;
	info.id = m_pItemInfo->getId();
	info.changeFlags = UserCore::Item::ItemInfoI::CHANGED_STATUS;

	m_pItemInfo->getInfoChangeEvent()(info);
}

void ItemHandle::onTaskStart(ITEM_STAGE &stage)
{
	gcTrace("Stage {0}", (int32)stage);

	if (stage == ITEM_STAGE::STAGE_NONE)
	{
		m_uiStage = ITEM_STAGE::STAGE_NONE;
		return;
	}

	setStage(stage);
}

void ItemHandle::onTaskComplete(ITEM_STAGE &stage)
{
	gcTrace("Stage {0}", (int32)stage);

	if (stage == ITEM_STAGE::STAGE_NONE)
	{
		releaseComplexLock();
		stopThread();
	}
}

void ItemHandle::resetStage(bool close)
{
	gcTrace("Close {0}", close);

	//if we are updating and error out goback to last known state
	if (getItemInfo()->getStatus() & UserCore::Item::ItemInfoI::STATUS_UPDATING)
	{
		getItemInfo()->delSFlag(UserCore::Item::ItemInfoI::STATUS_UPDATING|UserCore::Item::ItemInfoI::STATUS_UPDATEAVAL);
		getItemInfo()->addSFlag(UserCore::Item::ItemInfoI::STATUS_READY);
	}
	else
	{
		getItemInfo()->delSFlag(UserCore::Item::ItemInfoI::STATUS_INSTALLED|
						UserCore::Item::ItemInfoI::STATUS_DOWNLOADING|
						UserCore::Item::ItemInfoI::STATUS_READY|
						UserCore::Item::ItemInfoI::STATUS_VERIFING|
						UserCore::Item::ItemInfoI::STATUS_UPDATEAVAL);

		getItemInfoNorm()->resetInstalledMcf();
	}

	completeStage(close);
}

void ItemHandle::completeStage(bool close)
{
	gcTrace("Close {0}", close);

	if (close)
		registerTask(gcRefPtr<BlankTask>::create(this, ITEM_STAGE::STAGE_CLOSE));

	registerTask(gcRefPtr<BlankTask>::create(this, ITEM_STAGE::STAGE_NONE));
}


void ItemHandle::goToStageDownloadTools(ToolTransactionId ttid, const char* downloadPath, MCFBranch branch, MCFBuild build)
{
	registerTask(gcRefPtr<UserCore::ItemTask::DownloadToolTask>::create(this, ttid, downloadPath, branch, build));
}

void ItemHandle::goToStageDownloadTools(bool launch, ToolTransactionId ttid)
{
	registerTask(gcRefPtr<UserCore::ItemTask::DownloadToolTask>::create(this, launch, ttid));
}

void ItemHandle::goToStageInstallTools(bool launch)
{
	registerTask(gcRefPtr<UserCore::ItemTask::InstallToolTask>::create(this, launch));
}

void ItemHandle::goToStageGatherInfo(MCFBranch branch, MCFBuild build, UserCore::ItemTask::GI_FLAGS flags)
{
	gcRefPtr<Helper::GatherInfoHandlerHelperI> helper;
	
	if (m_pFactory)
		helper = m_pFactory->getGatherInfoHelper();

	registerTask(gcRefPtr<UserCore::ItemTask::GatherInfoTask>::create(this, branch, build, helper, flags));
}

void ItemHandle::goToStageDownload(MCFBranch branch, MCFBuild build, bool test)
{
	if (shouldPauseOnError())
	{
		setPauseOnError(false);
		completeStage(true);
		return;
	}

	if (!preDownloadCheck(branch, test))
	{
		completeStage(true);
		return;
	}

	registerTask(gcRefPtr<UserCore::ItemTask::ValidateTask>::create(this, branch, build));
}

void ItemHandle::goToStageDownload(const char* path)
{
	registerTask(gcRefPtr<UserCore::ItemTask::DownloadTask>::create(this, path));
}

void ItemHandle::goToStageVerify(MCFBranch branch, MCFBuild build, bool files, bool tools, bool hooks)
{
	registerTask(gcRefPtr<UserCore::ItemTask::VerifyServiceTask>::create(this, branch, build, files, tools, hooks));
}

void ItemHandle::goToStageInstallCheck()
{
	registerTask(gcRefPtr<UserCore::ItemTask::InstallCheckTask>::create(this));
}

void ItemHandle::goToStageLaunch()
{
	registerTask(gcRefPtr<UserCore::ItemTask::LaunchTask>::create(this));
}

void ItemHandle::goToStageUninstall(bool complete, bool account)
{
	setPaused(false, true);

	{
		std::lock_guard<std::recursive_mutex> guard(m_ThreadMutex);

		if (m_pThread)
			m_pThread->purge();
	}
	

	if (HasAllFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_INSTALLCOMPLEX))
	{
		auto uibst = gcRefPtr<UserCore::ItemTask::UIComplexModServiceTask>::create(this, getItemInfo()->getInstalledBranch(), getItemInfo()->getInstalledBuild());
		uibst->setCAUninstall(complete, account);
		uibst->setEndStage();

		registerTask(uibst);
	}
	else
	{
		registerTask(gcRefPtr<UserCore::ItemTask::UIServiceTask>::create(this, complete, account));
	}
}

void ItemHandle::goToStageUninstallBranch(MCFBranch branch, MCFBuild build, bool test)
{
	registerTask(gcRefPtr<UserCore::ItemTask::UIBranchServiceTask>::create(this, branch, build, test));
}


void ItemHandle::releaseComplexLock()
{
	gcTrace("");

	bool isComplex = HasAllFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_INSTALLCOMPLEX);
	gcRefPtr<UserCore::Item::ItemHandle> obj = this;

	if (isComplex)
	{
		auto parentHandle = gcRefPtr<UserCore::Item::ItemHandle>::dyn_cast(m_pUserCore->getItemManager()->findItemHandle(getItemInfo()->getParentId()));

		if (parentHandle)
			obj = parentHandle;
	}

	if (obj)
		obj->releaseLock(this);
}

bool ItemHandle::getComplexLock()
{
	gcTrace("");

	bool isParentComplex = getItemInfo()->getInstalledModId().isOk();
	bool isComplex = HasAllFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_INSTALLCOMPLEX);


	if (!isComplex && !isParentComplex)
		return true;


	gcRefPtr<UserCore::Item::ItemHandle> obj = this;
	auto parentHandle = gcRefPtr<UserCore::Item::ItemHandle>::dyn_cast(m_pUserCore->getItemManager()->findItemHandle(getItemInfo()->getParentId()));

	if (isComplex && parentHandle)
		obj = parentHandle;

	if (obj && obj->getLock(this))
		return true;

	gcException eItem(ERR_INVALID, "Failed to get lock. Another task is using this item. Please stop that task and try again.");
	gcException eGame(ERR_INVALID, "Failed to get lock. Another task is using the parent game-> Please stop that task and try again.");
	
	if (obj != parentHandle)
		onErrorEvent(eItem);
	else
		onErrorEvent(eGame);
	
	completeStage(true);
	return false;
}

void ItemHandle::goToStageUninstallUpdate(const char* path, MCFBuild lastBuild)
{
	registerTask(gcRefPtr<UserCore::ItemTask::UIUpdateServiceTask>::create(this, path, lastBuild));
}

void ItemHandle::goToStageUninstallPatch(MCFBranch branch, MCFBuild build)
{
	bool isParentComplex = getItemInfo()->getInstalledModId().isOk();
	bool isComplex = HasAllFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_INSTALLCOMPLEX);

	auto parentItem = m_pUserCore->getItemManager()->findItemInfo(getItemInfo()->getParentId());

	if (isParentComplex || isComplex)
	{
		if (!getComplexLock())
			return;

		if (isParentComplex)
			parentItem = getItemInfo();
		
		//need to uninstall complex mods otherwise the backup file will be wrong
		if (parentItem->getInstalledModId().isOk())
		{
			auto uibst = gcRefPtr<UserCore::ItemTask::UIComplexModServiceTask>::create(this, branch, build);

			if (isParentComplex)
				uibst->setCAUIPatch();
			else
				uibst->setCAInstall();

			registerTask(uibst);
			return;
		}
	}

	registerTask(gcRefPtr<UserCore::ItemTask::UIPatchServiceTask>::create(this, branch, build));
}

void ItemHandle::goToStageUninstallComplexBranch(MCFBranch branch, MCFBuild build, bool launch)
{
	if (!getComplexLock())
		return;

	auto uibst = gcRefPtr<UserCore::ItemTask::UIComplexModServiceTask>::create(this, branch, build);

	//if we are the game and need our child branch removed
	if (getItemInfo()->getInstalledModId().isOk())
	{
		if (launch && HasAllFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_LINK))
		{
			uibst->setCALaunch();
		}
		else
		{
			uibst->setCAUninstallBranch();
		}
	}
	else
	{
		uibst->setCAInstall();
	}

	registerTask(uibst);
}

void ItemHandle::goToStageInstallComplex(MCFBranch branch, MCFBuild build, bool launch)
{
	if (!getComplexLock())
	{
		getItemInfo()->addSFlag(UserCore::Item::ItemInfoI::STATUS_PAUSED|UserCore::Item::ItemInfoI::STATUS_INSTALLING);
		return;
	}

	if (getItemInfo()->isComplex())
	{
		gcRefPtr<Helper::InstallerHandleHelperI> helper;

		if (m_pFactory)
			helper = m_pFactory->getInstallHelper();

		auto clst = gcRefPtr<UserCore::ItemTask::ComplexLaunchServiceTask>::create(this, false, branch, build, helper);

		if (launch)
			clst->launch();
		else
			clst->completeStage();

		registerTask(clst);
	}
	else
	{
		gcAssert(false);
	}
}

void ItemHandle::goToStageInstall(const char* path, MCFBranch branch)
{
	if (!getItemInfo()->isComplex())
	{
		gcRefPtr<Helper::InstallerHandleHelperI> helper;

		if (m_pFactory)
			helper = m_pFactory->getInstallHelper();

		registerTask(gcRefPtr<UserCore::ItemTask::InstallServiceTask>::create(this, path, branch, helper));
	}
	else
	{
		gcAssert(false);
	}
}

void ItemHandle::stopThread()
{
	gcTrace("");

	std::lock_guard<std::recursive_mutex> guard(m_ThreadMutex);

	if (!m_pThread)
		return;

	m_pThread->setThreadManager(nullptr);

	gcAssert(m_pThread.getRefCt() == 1);

	m_pUserCore->getThreadPool()->queueTask(gcRefPtr<UserCore::Task::DeleteThread>::create(m_pUserCore, m_pThread));
	gcAssert(!m_pThread); //delete thread should null this to avoid race condition with thread pool deleting before we null it here

	m_bStopped = true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ItemHandle::registerTask(gcRefPtr<UserCore::ItemTask::BaseItemTask> task)
{
	if (!task)
		return;

	gcTrace("Task {0}", task->getTaskName());

	m_pEventHandler->registerTask(task);
	std::lock_guard<std::recursive_mutex> guard(m_ThreadMutex);

	if (!m_pThread)
	{
		m_pThread = gcRefPtr<ItemThread>::create(this);

		m_pThread->setThreadManager(m_pUserCore->getThreadManager());
		m_pThread->setUserCore(m_pUserCore);
		m_pThread->setWebCore(m_pUserCore->getWebCore());

		m_pThread->onTaskStartEvent += delegate(this, &ItemHandle::onTaskStart);
		m_pThread->onTaskCompleteEvent += delegate(this, &ItemHandle::onTaskComplete);

		m_bStopped = false;
	}
	
	m_pThread->queueTask(task);
}

void ItemHandle::addHelper(gcRefPtr<Helper::ItemHandleHelperI> helper)
{
	if (!helper)
		return;

	std::lock_guard<std::recursive_mutex> guard(m_HelperLock);

	m_vHelperList.push_back(helper);
	helper->setId(m_uiHelperId);
	m_uiHelperId++;

	//forward missed messages on
	if (isInStage())
		m_pEventHandler->postAll(helper);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool ItemHandle::preDownloadCheck(MCFBranch branch, bool test)
{
	auto branchInfo = getItemInfo()->getBranchById(branch);
	auto parentInfo = m_pUserCore->getItemManager()->findItemInfo(getItemInfo()->getParentId());

	gcException eExist(ERR_INVALID, "Branch does not exist.");
	gcException eNotAvaliable(ERR_INVALID, "Branch is not on your account. Please purchase branch first.");
	gcException eRelease(ERR_INVALID, "Branch has no releases available for download.");

	if (!branchInfo)
		onErrorEvent(eExist);
	else if (!branchInfo->isAvaliable() && !test)
		onErrorEvent(eNotAvaliable);
	else if (!branchInfo->isDownloadable() && !test)
		onErrorEvent(eRelease);
	else if (parentInfo)
	{
		gcException eDemo(ERR_INVALID, gcString("Parent game is a demo. Please install {0} on the full version only.", parentInfo->getName()));
		gcException eParentNotReady(ERR_INVALID, gcString("Parent game is not installed. Please install {0} first.", parentInfo->getName()));

		bool isParentDemo = false;
		bool bParentNotReady = false;

		if (parentInfo && parentInfo->getCurrentBranch())
			isParentDemo = HasAllFlags(parentInfo->getCurrentBranch()->getFlags(), UserCore::Item::BranchInfoI::BF_DEMO | UserCore::Item::BranchInfoI::BF_TEST);

		if (parentInfo && !parentInfo->isLaunchable())
			bParentNotReady = true;

		if (isParentDemo)
			onErrorEvent(eDemo);
		else if (bParentNotReady)
			onErrorEvent(eParentNotReady);
		else
			return true;
	}
	else
		return true;

	return false;
}



bool ItemHandle::verify(bool files, bool tools, bool hooks)
{
	if (isInStage())
		return true;

	goToStageVerify(MCFBranch(), MCFBuild(), files, tools, hooks);
	return true;
}

void ItemHandle::verifyOveride()
{
	{
		std::lock_guard<std::recursive_mutex> guard(m_ThreadMutex);

		if (m_pThread)
			m_pThread->purge();
	}

	goToStageVerify(MCFBranch(), MCFBuild(), true, false, false);
}

void ItemHandle::uninstallOveride()
{

}

bool ItemHandle::update()
{
	if (isInStage())
		return true;

	auto branch = getItemInfo()->getCurrentBranch();

	uint32 status = getItemInfo()->getStatus();
	uint32 flags = UserCore::Item::ItemInfoI::STATUS_READY|UserCore::Item::ItemInfoI::STATUS_INSTALLED|UserCore::Item::ItemInfoI::STATUS_UPDATEAVAL;

	gcException eNoBranches(ERR_NOUPDATE, "This item has no installed branches.");
	gcException eNoUpdates(ERR_NOUPDATE, "There is no update available for this item.");

	if (!branch)
	{
		onErrorEvent(eNoBranches);
	}
	else if (HasAllFlags(status, flags))
	{
		MCFBranch mcfBranch = branch->getBranchId();
		MCFBuild mcfBuild = getItemInfo()->getNextUpdateBuild();

		return install(mcfBranch, mcfBuild, UserCore::ItemTask::GI_FLAG_UPDATE);
	}
	else
	{
		onErrorEvent(eNoUpdates);
	}

	return false;
}

bool ItemHandle::install(gcRefPtr<Helper::ItemLaunchHelperI> helper, MCFBranch branch)
{
	if (isInStage())
		return true;

	if (!getItemInfo()->isDownloadable())
	{
		if (HasAnyFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_LINK) && branch != 0)
		{
			goToStageGatherInfo(branch, MCFBuild(), getItemInfo()->isLaunchable()?UserCore::ItemTask::GI_FLAG_EXISTING:UserCore::ItemTask::GI_FLAG_NONE);
			return true;
		}

		return installCheck();
	}

	if (branch != 0 && getItemInfo()->isLaunchable() && branch == getItemInfo()->getInstalledBranch())
		return launch(helper);

	return install(branch, MCFBuild());
}

bool ItemHandle::install(MCFBranch branch, MCFBuild build, bool test)
{
	if (test)
		return install(branch, build, UserCore::ItemTask::GI_FLAG_TEST);

	return install(branch, build, UserCore::ItemTask::GI_FLAG_NONE);
}

bool ItemHandle::install(MCFBranch branch, MCFBuild build, UserCore::ItemTask::GI_FLAGS flags)
{
	if (isInStage())
		return true;

	return installPrivate(branch, build, flags);
}

bool ItemHandle::installPrivate(MCFBranch branch, MCFBuild build, UserCore::ItemTask::GI_FLAGS flags)
{
	if (checkPaused())
		return true;

	if (!HasAnyFlags(flags, UserCore::ItemTask::GI_FLAG_TEST) && HasAnyFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_UNAUTHED))
		flags = (UserCore::ItemTask::GI_FLAGS)(flags & UserCore::ItemTask::GI_FLAG_TEST);

	MCFBranch iBranch = getItemInfo()->getInstalledBranch();
	auto bInfo = getItemInfo()->getBranchById(branch);

	bool isParentComplex	= getItemInfo()->isParentToComplex();
	bool isComplex			= getItemInfo()->isComplex();
	bool installReady		= getItemInfo()->isLaunchable();
	bool needBranchSwitch	= (iBranch != 0 && iBranch != branch) || (isComplex && !isCurrentlyInstalledGameOrMod());
	bool isValidBranch		= bInfo && HasAnyFlags(bInfo->getFlags(), BranchInfoI::BF_DEMO|BranchInfoI::BF_FREE|BranchInfoI::BF_ONACCOUNT|BranchInfoI::BF_TEST);

	if (isValidBranch && installReady && needBranchSwitch)
	{
		if (!preDownloadCheck(branch, HasAnyFlags(flags, UserCore::ItemTask::GI_FLAG_TEST)))
			return false;

		if (isComplex || isParentComplex)
			goToStageUninstallComplexBranch(branch, build, HasAnyFlags(flags, UserCore::ItemTask::GI_FLAG_LAUNCH));
		else
			goToStageUninstallBranch(branch, build, HasAnyFlags(flags, UserCore::ItemTask::GI_FLAG_TEST));
	}
	else
	{
		goToStageGatherInfo(branch, build, flags);
	}

	return true;
}


bool ItemHandle::installCheck()
{
	if (isInStage())
		return true;

	if (!HasAllFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_INSTALLED))
	{
		goToStageInstallCheck();
		return true;
	}

	return false;
}

bool ItemHandle::cleanComplexMods()
{
	if (getItemInfo()->getInstalledModId().isOk())
	{
		auto mod = m_pUserCore->getItemManager()->findItemInfo(getItemInfo()->getInstalledModId());
			
		if (mod && HasAllFlags(mod->getStatus(), UserCore::Item::ItemInfoI::STATUS_INSTALLCOMPLEX))
		{
			if (!getComplexLock())
				return false;

			auto uibst = gcRefPtr<UserCore::ItemTask::UIComplexModServiceTask>::create(this, getItemInfo()->getInstalledBranch(), getItemInfo()->getInstalledBuild());
			uibst->setEndStage();
			registerTask(uibst);
			return true;				
		}
	}

	return false;
}


void ItemHandle::preLaunchCheck()
{
	if (!getItemInfo()->isLaunchable())
		throw gcException(ERR_LAUNCH, gcString("Failed to launch item {0}, failed status check {1}.", getItemInfo()->getName(), getItemInfo()->getStatus()));

	auto ei = getItemInfo()->getActiveExe();

	if (!ei)
		throw gcException(ERR_LAUNCH, gcString("Failed to launch item {0}, No executable info available.", getItemInfo()->getName()));
	
	UTIL::FS::Path path = UTIL::FS::PathWithFile(ei->getExe());

	if (!UTIL::FS::isValidFile(path))
		throw gcException(ERR_LAUNCH, gcString("Failed to launch item {0}, File is not valid exe [{1}].", getItemInfo()->getName(), ei->getExe()));

	m_pUserCore->getItemManager()->setRecent(getItemInfo()->getId());
}



bool ItemHandle::launchForReal(gcRefPtr<Helper::ItemLaunchHelperI> helper, bool offline)
{
	gcTrace("");

	auto bi = this->getItemInfo()->getCurrentBranch();

	if (bi && bi->isPreOrder())
	{
		helper->showPreOrderPrompt();
		return false;
	}

	if (bi && !offline)
	{
		std::vector<DesuraId> toolList;
		bi->getToolList(toolList);

		if (!m_pUserCore->getToolManager()->areAllToolsDownloaded(toolList))
		{
			goToStageDownloadTools(true);
			return true;
		}
		else if (!m_pUserCore->getToolManager()->areAllToolsInstalled(toolList))
		{
			goToStageInstallTools(true);
			return true;
		}
	}

	try
	{
		doLaunch(helper);

		//steam games get this set on activate
		if (getItemInfo()->getCurrentBranch() && !getItemInfo()->getCurrentBranch()->isSteamGame())
			getItemInfo()->addSFlag(UserCore::Item::ItemInfoI::STATUS_LAUNCHED);
	}
	catch (gcException &e)
	{
		if (helper)
			helper->launchError(e);
		else
			onErrorEvent(e);
	}

	return false;
}

bool ItemHandle::checkPaused()
{
	bool hasPauseFlag = HasAnyFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_PAUSED);

	if (hasPauseFlag)
	{
		setPaused(false);

		std::lock_guard<std::recursive_mutex> guard(m_ThreadMutex);

		//if we have a valid thread then we should be doing something
		if (m_pThread && m_pThread->hasTaskToRun())
			return true;
	}

	return false;
}

bool ItemHandle::launch(gcRefPtr<Helper::ItemLaunchHelperI> helper, bool offline, bool ignoreUpdate)
{
	if (isInStage())
		return true;

	if (checkPaused())
		return true;




	if (getItemInfo()->getParentId().isOk())
	{
		auto pParent = getUserCore()->getItemManager()->findItemInfo(getItemInfo()->getParentId());

		if (pParent)
		{
			if (!pParent->isLaunchable())
			{
				gcException e(ERR_LAUNCH, gcString("Parent game is not installed. Please install {0} first.", pParent->getName()));
				helper->launchError(e);
				return false;
			}
			else if (pParent->isFirstLaunch())
			{
				if (helper)
					helper->showParentNoRunPrompt(pParent->getId());

				return false;
			}
		}
	}

	bool res = false;

	if (HasAllFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_LINK))
	{
		if (getItemInfo()->getInstalledModId().isOk())
		{
			auto mod = m_pUserCore->getItemManager()->findItemInfo(getItemInfo()->getInstalledModId());

			if (!mod || !HasAllFlags(mod->getStatus(), UserCore::Item::ItemInfoI::STATUS_INSTALLCOMPLEX))
			{
				return launchForReal(helper);
			}
			else
			{
				goToStageUninstallComplexBranch(getItemInfo()->getInstalledBranch(), getItemInfo()->getInstalledBuild(), true);
				return true;
			}
		}
		else if (getItemInfo()->isLaunchable())
		{
			res = launchForReal(helper);
		}
		else
		{
			install(helper, MCFBranch());
		}
	}
	else if (offline)
	{
		if (getItemInfo()->isLaunchable())
			res = launchForReal(helper, offline);
	}
	else if (HasAllFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_DELETED))
	{
		if (HasAllFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_NONDOWNLOADABLE))
			installCheck();
		else
			install(helper, MCFBranch());

		res = true;
	}
	else
	{
		bool hasPreorder = false;

		for (size_t x = 0; x<getItemInfo()->getBranchCount(); x++)
		{
			if (getItemInfo()->getBranch(x)->isPreOrderAndNotPreload())
			{
				hasPreorder = true;
				break;
			}
		}

		if (!getItemInfo()->getCurrentBranch() && hasPreorder)
		{
			helper->showPreOrderPrompt();
			res = false;
		}

		if (HasAnyFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_DOWNLOADING | UserCore::Item::ItemInfoI::STATUS_VERIFING | UserCore::Item::ItemInfoI::STATUS_INSTALLING))
		{
			verify(true, false, false);
			res = true;
		}
		else if (!HasAnyFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_READY | UserCore::Item::ItemInfoI::STATUS_UPDATEAVAL))
		{
			if (HasAllFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_NONDOWNLOADABLE))
				installCheck();
			else
				install(helper, MCFBranch());

			res = true;
		}
		else if (HasAnyFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_DOWNLOADING | UserCore::Item::ItemInfoI::STATUS_INSTALLING))
		{
			if (HasAllFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_PAUSED))
				getItemInfo()->delSFlag(UserCore::Item::ItemInfoI::STATUS_PAUSED);

			verify(true, false, false);
			res = true;
		}
		else if (HasAnyFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_PRELOADED))
		{
			if (getItemInfo()->getCurrentBranch()->isPreOrderAndNotPreload())
			{
				helper->showPreOrderPrompt();
				res = false;
			}
			else
			{
				//preorder tag has dropped. Install
				install(MCFBranch(), MCFBuild(), UserCore::ItemTask::GI_FLAG_EXISTING);
				res = true;
			}
		}
		else if (getItemInfo()->isLaunchable())
		{
			bool isComplex = HasAllFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_INSTALLCOMPLEX);

			if (isComplex && !isCurrentlyInstalledGameOrMod())
			{
				UserCore::ItemTask::GI_FLAGS flags = (UserCore::ItemTask::GI_FLAGS)(UserCore::ItemTask::GI_FLAG_EXISTING | UserCore::ItemTask::GI_FLAG_LAUNCH);
				install(getItemInfo()->getInstalledBranch(), getItemInfo()->getInstalledBuild(), flags);
				return true;
			}

			if (!getItemInfo()->hasAcceptedEula())
			{
				if (helper)
					helper->showEULAPrompt();
				else
					Warning("No Helper for launch item: Show EULA\n");

				return false;
			}
			else if (!ignoreUpdate && HasAllFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_UPDATEAVAL))
			{
				if (helper)
					helper->showUpdatePrompt();
				else
					Warning("No Helper for launch item: Show update form\n");

				return false;
			}

			res = launchForReal(helper);
		}
		else
		{
			Warning("Failed to launch item. :(");
		}
	}

	return res;
}

bool ItemHandle::isCurrentlyInstalledGameOrMod()
{
	if (!getItemInfo()->isComplex())
		return true;

	if (getItemInfo()->getParentId().isOk())
	{
		auto par = m_pUserCore->getItemManager()->findItemInfo(getItemInfo()->getParentId());
		return (par && par->getInstalledModId() == getItemInfo()->getId());
	}

	return !getItemInfo()->getInstalledModId().isOk();
}

bool ItemHandle::switchBranch(MCFBranch branch)
{
	return install(branch, MCFBuild());
}

bool ItemHandle::startUpCheck()
{
	if (isInStage())
		return true;

	if (HasAnyFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_READY|UserCore::Item::ItemInfoI::STATUS_PAUSED))
		return false;

	return (HasAnyFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_DOWNLOADING|UserCore::Item::ItemInfoI::STATUS_VERIFING|UserCore::Item::ItemInfoI::STATUS_INSTALLING));
}

bool ItemHandle::uninstall(gcRefPtr<Helper::ItemUninstallHelperI> helper, bool complete, bool account)
{
	gcTrace("");

	if (m_uiStage == ITEM_STAGE::STAGE_UNINSTALL)
		return true;

	if (isInStage())
	{
		if (helper && !helper->stopStagePrompt())
			return false;
	}

	goToStageUninstall(complete, account);
	return true;
}

ITEM_STAGE ItemHandle::getStage()
{
	return m_uiStage;
}

void ItemHandle::cancelCurrentStage()
{
	gcTrace("");

	if (!isInStage())
		return;

	if (getStage() == UserCore::Item::ITEM_STAGE::STAGE_NONE || getStage() == UserCore::Item::ITEM_STAGE::STAGE_CLOSE)
		return;

	setPaused(false);

	std::lock_guard<std::recursive_mutex> guard(m_ThreadMutex);

	if (m_pThread)
		m_pThread->cancelCurrentTask();
}

void ItemHandle::getStatusStr(LanguageManagerI & pLangMng, char* buffer, uint32 buffsize)
{
	std::lock_guard<std::mutex> guard(m_GroupLock);
	getStatusStr_s(this, m_pItemInfo, m_uiStage, m_pGroup, pLangMng, buffer, buffsize);
}

void ItemHandle::getStatusStr_s(gcRefPtr<UserCore::Item::ItemHandleI> pItemHandle, gcRefPtr<UserCore::Item::ItemInfoI> pItemInfo, UserCore::Item::ITEM_STAGE nStage
	, gcRefPtr<UserCore::Item::ItemTaskGroupI> pTaskGroup, LanguageManagerI &pLangMng, char* buffer, uint32 buffsize)
{
	gcString temp;

	uint32 status = pItemInfo->getStatus();

	if (!pItemInfo->isDownloadable())
	{
		if (pItemInfo->isInstalled())
			temp = gcString(pLangMng.getString("#IS_INSTALLED"));
		else
			temp = gcString(pLangMng.getString("#IS_NOTINSTALLED"));
	}
	else
	{
		bool skip = false;
		const char* stateMsg;

		if (nStage == UserCore::Item::ITEM_STAGE::STAGE_DOWNLOADTOOL)
		{
			stateMsg = "#IS_DOWNLOADINGTOOL";
		}
		else if (nStage == UserCore::Item::ITEM_STAGE::STAGE_INSTALLTOOL)
		{
			stateMsg = "#IS_INSTALLINGTOOL";
		}
		else if (nStage == UserCore::Item::ITEM_STAGE::STAGE_LAUNCH)
		{
			skip = true;
			temp = pLangMng.getString("#IS_LAUNCHING");
		}
		else if (nStage == UserCore::Item::ITEM_STAGE::STAGE_VALIDATE)
		{
			stateMsg = pLangMng.getString("#IS_VALIDATE");
		}
		else if (nStage == UserCore::Item::ITEM_STAGE::STAGE_WAIT && pTaskGroup)
		{
			skip = true;
			temp = gcString(pLangMng.getString("#IS_WAIT"), pTaskGroup->getPos(pItemHandle), pTaskGroup->getCount());
		}
		else if (status & UserCore::Item::ItemInfoI::STATUS_VERIFING || nStage == UserCore::Item::ITEM_STAGE::STAGE_VERIFY)
		{
			stateMsg = "#IS_VERIFY";
		}
		else if (status & UserCore::Item::ItemInfoI::STATUS_UPDATING)
		{
			stateMsg = "#IS_UPDATING";
		}
		else if (status & UserCore::Item::ItemInfoI::STATUS_DOWNLOADING || nStage == UserCore::Item::ITEM_STAGE::STAGE_DOWNLOAD)
		{
			stateMsg = "#IS_DOWNLOADING";
		}
		else if (status & UserCore::Item::ItemInfoI::STATUS_INSTALLING || nStage == UserCore::Item::ITEM_STAGE::STAGE_INSTALL)
		{
			stateMsg = "#IS_INSTALLING";
		}
		else 
		{
			bool hasPreorder = false;

			for (size_t x=0; x<pItemInfo->getBranchCount(); x++)
			{
				if (pItemInfo->getBranch(x)->isPreOrder())
				{
					hasPreorder = true;
					break;
				}
			}

			if (HasAnyFlags(status, UserCore::Item::ItemInfoI::STATUS_READY|UserCore::Item::ItemInfoI::STATUS_PRELOADED))
			{
				stateMsg = "#IS_READY";

				auto bi = pItemInfo->getCurrentBranch();
				if (bi && bi->isPreOrder())
					stateMsg = "#IS_PRELOADED_STATUS";
			}
			else if (!pItemInfo->getCurrentBranch() && hasPreorder)
			{
				stateMsg = "#IS_PREORDER_STATUS";
			}
			else if (status & UserCore::Item::ItemInfoI::STATUS_DEVELOPER)
			{
				stateMsg = "#IS_DEVSTUB";
			}
			else
			{
				stateMsg = "#IS_NOTAVAIL";
			}

			skip = true;
			temp = pLangMng.getString(stateMsg);
		}

		if (!skip)
		{
			if (status & UserCore::Item::ItemInfoI::STATUS_PAUSED)
				stateMsg = "#IS_PAUSED";

			temp = gcString("{0:u}% - {1}", pItemInfo->getPercent(), pLangMng.getString(stateMsg));
		}
	}

	Safe::strcpy(buffer, buffsize, temp.c_str());
}


gcRefPtr<ItemHandleEvents> ItemHandle::getEventHandler()
{
	return m_pEventHandler;
}

bool ItemHandle::setTaskGroup(gcRefPtr<ItemTaskGroup> group, bool force)
{
	if (group && isInStage() && !force)
		return false;

	if (force)
	{
		setPaused(false, true);

		std::lock_guard<std::recursive_mutex> guard(m_ThreadMutex);

		if (m_pThread)
			m_pThread->purge();
	}

	std::lock_guard<std::mutex> guard(m_GroupLock);
	m_pGroup = group;

	if (group)
	{
		registerTask(group->newTask(this));
	}
	else
	{
		if (getStage() == UserCore::Item::ITEM_STAGE::STAGE_WAIT)
			cancelCurrentStage();
	}
	
	return true;
}

gcRefPtr<ItemTaskGroupI> ItemHandle::getTaskGroup()
{
	std::lock_guard<std::mutex> guard(m_GroupLock);
	return m_pGroup;
}

void ItemHandle::force()
{
	std::lock_guard<std::mutex> guard(m_GroupLock);
	if (!m_pGroup)
		return;

	m_pGroup->removeItem(this);
	m_pGroup->startAction(this);
}

#ifdef LINK_WITH_GMOCK

#include "sqlite3x.hpp"
#include "sql/ItemInfoSql.h"
#include "usercore/ItemManagerI.h"
#include "usercore/ItemHelpersI.h"
#include "usercore/UserCoreI.h"

#include "McfManager.h"

namespace UnitTest
{
	using namespace UserCore;
	using namespace UserCore::Item::Helper;
	using namespace testing;

	class ItemHandleMock : public ItemHandle
	{
	public:
		ItemHandleMock(gcRefPtr<ItemInfo> itemInfo, gcRefPtr<UserCore::UserI> user)
			: ItemHandle(itemInfo, user)
		{
			addRef();
		}

		MOCK_METHOD1(completeStage, void(bool close));
		MOCK_METHOD1(resetStage, void(bool close));
		MOCK_METHOD3(goToStageGatherInfo, void(MCFBranch branch, MCFBuild build, UserCore::ItemTask::GI_FLAGS flags));
		MOCK_METHOD3(goToStageDownload, void(MCFBranch branch, MCFBuild build, bool test));
		MOCK_METHOD1(goToStageDownload, void(const char* path));
		MOCK_METHOD3(goToStageInstallComplex, void(MCFBranch branch, MCFBuild build, bool launch));
		MOCK_METHOD2(goToStageInstall, void(const char* path, MCFBranch branch));
		MOCK_METHOD5(goToStageVerify, void(MCFBranch branch, MCFBuild build, bool files, bool tools, bool hooks));
		MOCK_METHOD0(goToStageInstallCheck, void());
		MOCK_METHOD2(goToStageUninstall, void(bool complete, bool account));
		MOCK_METHOD3(goToStageUninstallBranch, void(MCFBranch branch, MCFBuild build, bool test));
		MOCK_METHOD3(goToStageUninstallComplexBranch, void(MCFBranch branch, MCFBuild build, bool complexLaunch));
		MOCK_METHOD2(goToStageUninstallPatch, void(MCFBranch branch, MCFBuild build));
		MOCK_METHOD2(goToStageUninstallUpdate, void(const char* path, MCFBuild lastBuild));
		MOCK_METHOD0(goToStageLaunch, void());
		MOCK_METHOD4(goToStageDownloadTools, void(ToolTransactionId ttid, const char* downloadPath, MCFBranch branch, MCFBuild build));
		MOCK_METHOD2(goToStageDownloadTools, void(bool launch, ToolTransactionId ttid));
		MOCK_METHOD1(goToStageInstallTools, void(bool launch));

		MOCK_METHOD2(launchForReal, bool(gcRefPtr<Helper::ItemLaunchHelperI> helper, bool offline));
	};

	class ItemHandleLaunchMod : public ::testing::Test
	{
	public:
		ItemHandleLaunchMod()
			: gid("1", "games")
			, mid("2", "mods")
			, user(gcRefPtr<UserMock>::create())
			, itemManager(gcRefPtr<ItemManagerMock>::create())
			, game(gcRefPtr<ItemInfo>::create(user, gid, &fs))
			, mod(gcRefPtr<ItemInfo>::create(user, mid, gid, &fs))
		{

			ON_CALL(*user, getItemsAddedEvent()).WillByDefault(ReturnRef(itemAddedEvent));
			ON_CALL(*user, getItemManager()).WillByDefault(Return(itemManager));
			ON_CALL(*itemManager, findItemInfo(_)).WillByDefault(Invoke([&](DesuraId id) -> gcRefPtr<ItemInfoI>
			{
				if (id == game->getId())
					return game;

				if (id == mod->getId())
					return mod;

				return nullptr;
			}));

			ON_CALL(fs, isValidFile(_)).WillByDefault(Invoke([](const UTIL::FS::Path& path) -> bool
			{
				return path.getFile().getFile() == "Charlie.exe";
			}));
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

		Event<uint32> itemAddedEvent;
		UTIL::FS::UtilFSMock fs;
		gcRefPtr<ItemManagerMock> itemManager;
		gcRefPtr<UserMock> user;

		DesuraId gid;
		DesuraId mid;

		gcRefPtr<ItemInfo> game;
		gcRefPtr<ItemInfo> mod;
	};

	TEST_F(ItemHandleLaunchMod, LaunchModWithParentNotInstalled)
	{
		static const std::vector<std::string> vSqlCommands =
		{
			"INSERT INTO iteminfo VALUES(4294967328,0,0,0,0,'dev-02','Charlie','charlie','','','','','','','dev-02','',0,0);",
			"INSERT INTO iteminfo VALUES(8589934608,4294967328,0,0,0,'dev-02','Charlie Mod','charlie-mod','','','','','','','dev-02','',0,0);"
		};

		{
			sqlite3x::sqlite3_connection db(":memory:");
			setUpDb(db, vSqlCommands);

			game->loadDb(&db);
			mod->loadDb(&db);
		}

		auto modHandle = gcRefPtr<ItemHandleMock>::create(mod, user);
		auto ilh = gcRefPtr<ItemLaunchHelperMock>::create();
		EXPECT_CALL(*ilh, launchError(_)).Times(1);

		auto res = modHandle->launch(ilh);
		ASSERT_FALSE(res);
	}

	TEST_F(ItemHandleLaunchMod, LaunchModWithParentNotAcceptedEula)
	{
		static const std::vector<std::string> vSqlCommands =
		{
			"INSERT INTO exe VALUES(4294967328,100,'Play','C:\\Program Files (x86)\\charlie\\Charlie.exe','','',0);",
			"INSERT INTO installinfo VALUES(4294967328,100,'C:\\Program Files (x86)\\charlie','C:\\Program Files (x86)\\charlie\\Charlie.exe','',0,0,0);",
			"INSERT INTO installinfoex VALUES(4294967328,100,'C:\\Program Files (x86)\\charlie\\Charlie.exe');",
			"INSERT INTO iteminfo VALUES(4294967328,0,0,32798,0,'dev-02','Charlie','charlie','','','','','','','dev-02','',1,1);",
			"INSERT INTO branchinfo VALUES(1, 4294967328, 'test', 0, 'http://eula.com', 0, 0, '', '', 0, 1, 100);",
			"INSERT INTO iteminfo VALUES(8589934608,4294967328,0,0,0,'dev-02','Charlie Mod','charlie-mod','','','','','','','dev-02','',0,0);"
		};

		{
			sqlite3x::sqlite3_connection db(":memory:");
			setUpDb(db, vSqlCommands);

			game->loadDb(&db);
			mod->loadDb(&db);
		}

		ASSERT_TRUE(game->isInstalled());
		ASSERT_TRUE(game->isLaunchable());
		ASSERT_FALSE(game->hasAcceptedEula());

		auto modHandle = gcRefPtr<ItemHandleMock>::create(mod, user);
		auto ilh = gcRefPtr<ItemLaunchHelperMock>::create();
		EXPECT_CALL(*ilh, showParentNoRunPrompt(Eq(gid))).Times(1);

		auto res = modHandle->launch(ilh);
		ASSERT_FALSE(res);
	}

	TEST_F(ItemHandleLaunchMod, LaunchModWithParentInstalled_NotInstalled)
	{
		static const std::vector<std::string> vSqlCommands =
		{
			"INSERT INTO exe VALUES(4294967328,100,'Play','C:\\Program Files (x86)\\charlie\\Charlie.exe','','',0);",
			"INSERT INTO installinfo VALUES(4294967328,100,'C:\\Program Files (x86)\\charlie','C:\\Program Files (x86)\\charlie\\Charlie.exe','',0,0,0);",
			"INSERT INTO installinfoex VALUES(4294967328,100,'C:\\Program Files (x86)\\charlie\\Charlie.exe');",
			"INSERT INTO iteminfo VALUES(4294967328,0,0,16810014,0,'dev-02','Charlie','charlie','','','','','','','dev-02','',1,1);",
			"INSERT INTO branchinfo VALUES(1, 4294967328, 'test', 256, 'http://eula.com', 0, 0, '', '', 0, 1, 100);",
			"INSERT INTO iteminfo VALUES(8589934608,4294967328,0,0,0,'dev-02','Charlie Mod','charlie-mod','','','','','','','dev-02','',0,0);"
		};

		{
			sqlite3x::sqlite3_connection db(":memory:");
			setUpDb(db, vSqlCommands);

			game->loadDb(&db);
			mod->loadDb(&db);
		}

		ASSERT_FALSE(game->isFirstLaunch());
		ASSERT_TRUE(game->isInstalled());
		ASSERT_TRUE(game->isLaunchable());
		ASSERT_TRUE(game->hasAcceptedEula());

		std::function<void(ITEM_STAGE&)> stageChange = [](ITEM_STAGE&){
			//Should never change stage
			ASSERT_TRUE(false);
		};


		auto modHandle = gcRefPtr<ItemHandleMock>::create(mod, user);
		modHandle->getChangeStageEvent() += delegate(stageChange, 1);

		EXPECT_CALL(*modHandle, goToStageGatherInfo(_, _, _)).Times(1);

		auto ilh = gcRefPtr<ItemLaunchHelperMock>::create();
		auto res = modHandle->launch(ilh);

		ASSERT_TRUE(res);
	}

	TEST_F(ItemHandleLaunchMod, LaunchModWithParentInstalled_Installed)
	{
		static const std::vector<std::string> vSqlCommands =
		{
			"INSERT INTO exe VALUES(4294967328,100,'Play','C:\\Program Files (x86)\\charlie\\Charlie.exe','','',0);",
			"INSERT INTO installinfo VALUES(4294967328,100,'C:\\Program Files (x86)\\charlie','C:\\Program Files (x86)\\charlie\\Charlie.exe','',0,0,0);",
			"INSERT INTO installinfoex VALUES(4294967328,100,'C:\\Program Files (x86)\\charlie\\Charlie.exe');",
			"INSERT INTO branchinfo VALUES(1, 4294967328, 'test', 256, 'http://eula.com', 0, 0, '', '', 0, 1, 100);",
			"INSERT INTO iteminfo VALUES(4294967328,0,0,32798,0,'dev-02','Charlie','charlie','','','','','','','dev-02','',1,1);",
			
			"INSERT INTO exe VALUES(8589934608,100,'Play','C:\\Program Files (x86)\\charlie\\Charlie.exe','','',0);",
			"INSERT INTO installinfo VALUES(8589934608,100,'C:\\Program Files (x86)\\charlie','C:\\Program Files (x86)\\charlie\\Charlie.exe','',0,0,0);",
			"INSERT INTO installinfoex VALUES(8589934608,100,'C:\\Program Files (x86)\\charlie\\Charlie.exe');",
			"INSERT INTO branchinfo VALUES(2, 8589934608, 'test', 256, 'http://eula.com', 0, 0, '', '', 0, 2, 100);",
			"INSERT INTO iteminfo VALUES(8589934608,4294967328,0,32798,0,'dev-02','Charlie Mod','charlie-mod','','','','','','','dev-02','',2,2);"
		};

		{
			sqlite3x::sqlite3_connection db(":memory:");
			setUpDb(db, vSqlCommands);

			game->loadDb(&db);
			mod->loadDb(&db);
		}

		ASSERT_TRUE(game->isInstalled());
		ASSERT_TRUE(game->isLaunchable());
		ASSERT_TRUE(game->hasAcceptedEula());

		ASSERT_TRUE(mod->isInstalled());
		ASSERT_TRUE(mod->isLaunchable());
		ASSERT_TRUE(mod->hasAcceptedEula());

		auto modHandle = gcRefPtr<ItemHandleMock>::create(mod, user);
		EXPECT_CALL(*modHandle, launchForReal(_, _)).Times(1);

		auto ilh = gcRefPtr<ItemLaunchHelperMock>::create();
		modHandle->launch(ilh);
	}



	class ItemHandleComplexMods : public ::testing::Test
	{
	public:
		ItemHandleComplexMods()
			: gid("1", "games")
			, midA("2", "mods")
			, midB("3", "mods")
			, itemManager(gcRefPtr<ItemManagerMock>::create())
			, mcfManager(gcRefPtr<MCFManagerMock>::create())
			, user(gcRefPtr<UserMock>::create())
			, userInternalMock(gcRefPtr<UserInternalMock>::create())
			, game(gcRefPtr<ItemInfo>::create(user, gid, &fs))
			, modA(gcRefPtr<ItemInfo>::create(user, midA, gid, &fs))
			, modB(gcRefPtr<ItemInfo>::create(user, midB, gid, &fs))
		{

			ON_CALL(*user, getItemsAddedEvent()).WillByDefault(ReturnRef(itemAddedEvent));
			ON_CALL(*user, getItemManager()).WillByDefault(Return(itemManager));
			ON_CALL(*user, getInternal()).WillByDefault(Return(userInternalMock));

			ON_CALL(*userInternalMock, getMCFManager()).WillByDefault(Return(mcfManager));

			ON_CALL(*itemManager, findItemInfo(_)).WillByDefault(Invoke([&](DesuraId id) -> gcRefPtr<ItemInfoI>
			{
				if (id == game->getId())
					return game.get();

				if (id == modA->getId())
					return modA.get();

				if (id == modB->getId())
					return modB.get();

				return nullptr;
			}));

			ON_CALL(fs, isValidFile(_)).WillByDefault(Invoke([](const UTIL::FS::Path& path) -> bool
			{
				return path.getFile().getFile() == "Charlie.exe" || path.getFile().getFile() == "existing.file";
			}));
		}

		~ItemHandleComplexMods()
		{
			int a = 1;
		}

		void setUpDb(sqlite3x::sqlite3_connection &db, const std::vector<std::string> &vSqlCommands)
		{
			createItemInfoDbTables(db);

			for (auto s : vSqlCommands)
			{
				sqlite3x::sqlite3_command cmd(db, s.c_str());
				cmd.executenonquery();
			}

			game->loadDb(&db);
			modA->loadDb(&db);
			modB->loadDb(&db);
		}

		Event<uint32> itemAddedEvent;
		UTIL::FS::UtilFSMock fs;
		gcRefPtr<ItemManagerMock> itemManager;
		gcRefPtr<MCFManagerMock> mcfManager;
		gcRefPtr<UserMock> user;
		gcRefPtr<UserInternalMock> userInternalMock;

		DesuraId gid;
		DesuraId midA;
		DesuraId midB;

		gcRefPtr<UserCore::Item::ItemInfo> game;
		gcRefPtr<UserCore::Item::ItemInfo> modA;
		gcRefPtr<UserCore::Item::ItemInfo> modB;
	};

	TEST_F(ItemHandleComplexMods, LaunchParent_ModNotInstalled)
	{
		ON_CALL(*mcfManager, getMcfPath(_, _)).WillByDefault(Return(gcString()));
		ON_CALL(*mcfManager, getMcfPath(Eq(game), _)).WillByDefault(Return(gcString("existing.file")));

		static const std::vector<std::string> vSqlCommands =
		{
			"INSERT INTO exe VALUES(4294967328,100,'Play','C:\\ComplexGame\\Charlie.exe','','',0);",
			"INSERT INTO installinfo VALUES(4294967328,100,'C:\\ComplexGame','C:\\ComplexGame\\Charlie.exe','',0,0,0);",
			"INSERT INTO installinfoex VALUES(4294967328,100,'C:\\ComplexGame\\Charlie.exe');",
			"INSERT INTO branchinfo VALUES(1, 4294967328, 'test', 260, 'http://eula.com', 0, 0, '', '', 0, 1, 100);",
			"INSERT INTO iteminfo VALUES(4294967328,0,0,1081374,0,'dev-02','Complex Game','complex-game','','','','','','','dev-02','',1,1);",

			"INSERT INTO iteminfo VALUES(8589934608,4294967328,0,1048580,0,'dev-02','Complex Mod A','complex-mod-a','','','','','','','dev-02','',2,2);"
		};

		{
			sqlite3x::sqlite3_connection db(":memory:");
			setUpDb(db, vSqlCommands);
		}

		ASSERT_TRUE(game->isInstalled());
		ASSERT_TRUE(game->isLaunchable());
		ASSERT_TRUE(game->isComplex());
		ASSERT_FALSE(game->getInstalledModId().isOk());

		ASSERT_FALSE(modA->isInstalled());
		ASSERT_FALSE(modA->isLaunchable());
		ASSERT_TRUE(modA->isComplex());

		auto gameHandle = gcRefPtr<ItemHandleMock>::create(game, user);
		EXPECT_CALL(*gameHandle, launchForReal(_, _)).Times(1);

		auto ilh = gcRefPtr<ItemLaunchHelperMock>::create();
		gameHandle->launch(ilh);
	}

	TEST_F(ItemHandleComplexMods, LaunchParent_ModInstalled)
	{
		ON_CALL(*mcfManager, getMcfPath(_, _)).WillByDefault(Return(gcString()));
		ON_CALL(*mcfManager, getMcfPath(Eq(modA), _)).WillByDefault(Return(gcString("existing.file")));
		ON_CALL(*mcfManager, getMcfPath(Eq(game), _)).WillByDefault(Return(gcString("existing.file")));

		static const std::vector<std::string> vSqlCommands =
		{
			"INSERT INTO exe VALUES(4294967328,100,'Play','C:\\ComplexGame\\Charlie.exe','','',0);",
			"INSERT INTO installinfo VALUES(4294967328,100,'C:\\ComplexGame','C:\\ComplexGame\\Charlie.exe','',8589934608,0,0);",
			"INSERT INTO installinfoex VALUES(4294967328,100,'C:\\ComplexGame\\Charlie.exe');",
			"INSERT INTO branchinfo VALUES(1, 4294967328, 'test', 260, 'http://eula.com', 0, 0, '', '', 0, 1, 100);",
			"INSERT INTO iteminfo VALUES(4294967328,0,0,1081374,0,'dev-02','Complex Game','complex-game','','','','','','','dev-02','',1,1);",

			"INSERT INTO exe VALUES(8589934608,100,'Play','C:\\ComplexGame\\Charlie.exe','','',0);",
			"INSERT INTO installinfo VALUES(8589934608,100,'C:\\ComplexGame','C:\\ComplexGame\\Charlie.exe','',0,0,0);",
			"INSERT INTO installinfoex VALUES(8589934608,100,'C:\\ComplexGame\\Charlie.exe');",
			"INSERT INTO branchinfo VALUES(2, 8589934608, 'test', 260, 'http://eula.com', 0, 0, '', '', 0, 2, 100);",
			"INSERT INTO iteminfo VALUES(8589934608,4294967328,0,1081374,0,'dev-02','Complex Mod A','complex-mod-a','','','','','','','dev-02','',2,2);"
		};

		{
			sqlite3x::sqlite3_connection db(":memory:");
			setUpDb(db, vSqlCommands);
		}

		ASSERT_TRUE(game->isInstalled());
		ASSERT_TRUE(game->isLaunchable());
		ASSERT_TRUE(game->isComplex());
		ASSERT_EQ(midA, game->getInstalledModId());

		ASSERT_TRUE(modA->isInstalled());
		ASSERT_TRUE(modA->isLaunchable());
		ASSERT_TRUE(modA->isComplex());


		auto gameHandle = gcRefPtr<ItemHandleMock>::create(game, user);
		EXPECT_CALL(*gameHandle, goToStageUninstallComplexBranch(_, _, _)).Times(1);

		auto ilh = gcRefPtr<ItemLaunchHelperMock>::create();
		gameHandle->launch(ilh);
	}

	TEST_F(ItemHandleComplexMods, LaunchMod_ModNotInstalled)
	{
		ON_CALL(*mcfManager, getMcfPath(_, _)).WillByDefault(Return(gcString()));
		ON_CALL(*mcfManager, getMcfPath(Eq(game), _)).WillByDefault(Return(gcString("existing.file")));

		static const std::vector<std::string> vSqlCommands =
		{
			"INSERT INTO exe VALUES(4294967328,100,'Play','C:\\ComplexGame\\Charlie.exe','','',0);",
			"INSERT INTO installinfo VALUES(4294967328,100,'C:\\ComplexGame','C:\\ComplexGame\\Charlie.exe','',0,0,0);",
			"INSERT INTO installinfoex VALUES(4294967328,100,'C:\\ComplexGame\\Charlie.exe');",
			"INSERT INTO branchinfo VALUES(1, 4294967328, 'test', 260, 'http://eula.com', 0, 0, '', '', 0, 1, 100);",
			"INSERT INTO iteminfo VALUES(4294967328,0,0,1081374,0,'dev-02','Complex Game','complex-game','','','','','','','dev-02','',1,1);",

			"INSERT INTO exe VALUES(8589934608,100,'Play','C:\\ComplexGame\\Charlie.exe','','',0);",
			"INSERT INTO installinfo VALUES(8589934608,100,'C:\\ComplexGame','C:\\ComplexGame\\Charlie.exe','',0,0,0);",
			"INSERT INTO installinfoex VALUES(8589934608,100,'C:\\ComplexGame\\Charlie.exe');",
			"INSERT INTO branchinfo VALUES(2, 8589934608, 'test', 260, 'http://eula.com', 0, 0, '', '', 0, 2, 100);",
			"INSERT INTO iteminfo VALUES(8589934608,4294967328,0,1048580,0,'dev-02','Complex Mod A','complex-mod-a','','','','','','','dev-02','',2,2);"
		};

		{
			sqlite3x::sqlite3_connection db(":memory:");
			setUpDb(db, vSqlCommands);
		}

		ASSERT_TRUE(game->isInstalled());
		ASSERT_TRUE(game->isLaunchable());
		ASSERT_TRUE(game->isComplex());
		ASSERT_FALSE(game->getInstalledModId().isOk());

		ASSERT_FALSE(modA->isInstalled());
		ASSERT_FALSE(modA->isLaunchable());
		ASSERT_TRUE(modA->isComplex());

		auto modHandle = gcRefPtr<ItemHandleMock>::create(modA, user);
		EXPECT_CALL(*modHandle, goToStageGatherInfo(_, _, _)).Times(1);

		auto ilh = gcRefPtr<ItemLaunchHelperMock>::create();
		modHandle->launch(ilh);
	}

	TEST_F(ItemHandleComplexMods, LaunchModA_ModAInstalled)
	{
		ON_CALL(*mcfManager, getMcfPath(_, _)).WillByDefault(Return(gcString()));
		ON_CALL(*mcfManager, getMcfPath(Eq(modA), _)).WillByDefault(Return(gcString("existing.file")));
		ON_CALL(*mcfManager, getMcfPath(Eq(game), _)).WillByDefault(Return(gcString("existing.file")));

		static const std::vector<std::string> vSqlCommands =
		{
			"INSERT INTO exe VALUES(4294967328,100,'Play','C:\\ComplexGame\\Charlie.exe','','',0);",
			"INSERT INTO installinfo VALUES(4294967328,100,'C:\\ComplexGame','C:\\ComplexGame\\Charlie.exe','',8589934608,0,0);",
			"INSERT INTO installinfoex VALUES(4294967328,100,'C:\\ComplexGame\\Charlie.exe');",
			"INSERT INTO branchinfo VALUES(1, 4294967328, 'test', 260, 'http://eula.com', 0, 0, '', '', 0, 1, 100);",
			"INSERT INTO iteminfo VALUES(4294967328,0,0,1081374,0,'dev-02','Complex Game','complex-game','','','','','','','dev-02','',1,1);",

			"INSERT INTO exe VALUES(8589934608,100,'Play','C:\\ComplexGame\\Charlie.exe','','',0);",
			"INSERT INTO installinfo VALUES(8589934608,100,'C:\\ComplexGame','C:\\ComplexGame\\Charlie.exe','',0,0,0);",
			"INSERT INTO installinfoex VALUES(8589934608,100,'C:\\ComplexGame\\Charlie.exe');",
			"INSERT INTO branchinfo VALUES(2, 8589934608, 'test', 260, 'http://eula.com', 0, 0, '', '', 0, 2, 100);",
			"INSERT INTO iteminfo VALUES(8589934608,4294967328,0,1081374,0,'dev-02','Complex Mod A','complex-mod-a','','','','','','','dev-02','',2,2);"
		};

		{
			sqlite3x::sqlite3_connection db(":memory:");
			setUpDb(db, vSqlCommands);
		}

		ASSERT_TRUE(game->isInstalled());
		ASSERT_TRUE(game->isLaunchable());
		ASSERT_TRUE(game->isComplex());
		ASSERT_EQ(midA, game->getInstalledModId());

		ASSERT_TRUE(modA->isInstalled());
		ASSERT_TRUE(modA->isLaunchable());
		ASSERT_TRUE(modA->isComplex());

		auto modHandle = gcRefPtr<ItemHandleMock>::create(modA, user);
		EXPECT_CALL(*modHandle, launchForReal(_, _)).Times(1);

		auto ilh = gcRefPtr<ItemLaunchHelperMock>::create();
		modHandle->launch(ilh);
	}

	TEST_F(ItemHandleComplexMods, LaunchModA_ModBInstalled)
	{
		ON_CALL(*mcfManager, getMcfPath(_, _)).WillByDefault(Return(gcString()));
		ON_CALL(*mcfManager, getMcfPath(Eq(modA), _)).WillByDefault(Return(gcString("existing.file")));
		ON_CALL(*mcfManager, getMcfPath(Eq(modB), _)).WillByDefault(Return(gcString("existing.file")));
		ON_CALL(*mcfManager, getMcfPath(Eq(game), _)).WillByDefault(Return(gcString("existing.file")));

		static const std::vector<std::string> vSqlCommands =
		{
			"INSERT INTO exe VALUES(4294967328,100,'Play','C:\\ComplexGame\\Charlie.exe','','',0);",
			"INSERT INTO installinfo VALUES(4294967328,100,'C:\\ComplexGame','C:\\ComplexGame\\Charlie.exe','',12884901904,0,0);",
			"INSERT INTO installinfoex VALUES(4294967328,100,'C:\\ComplexGame\\Charlie.exe');",
			"INSERT INTO branchinfo VALUES(1, 4294967328, 'test', 260, 'http://eula.com', 0, 0, '', '', 0, 1, 100);",
			"INSERT INTO iteminfo VALUES(4294967328,0,0,1081374,0,'dev-02','Complex Game','complex-game','','','','','','','dev-02','',1,1);",

			"INSERT INTO exe VALUES(8589934608,100,'Play','C:\\ComplexGame\\Charlie.exe','','',0);",
			"INSERT INTO installinfo VALUES(8589934608,100,'C:\\ComplexGame','C:\\ComplexGame\\Charlie.exe','',0,0,0);",
			"INSERT INTO installinfoex VALUES(8589934608,100,'C:\\ComplexGame\\Charlie.exe');",
			"INSERT INTO branchinfo VALUES(2, 8589934608, 'test', 260, 'http://eula.com', 0, 0, '', '', 0, 2, 100);",
			"INSERT INTO iteminfo VALUES(8589934608,4294967328,0,1048580,0,'dev-02','Complex Mod A','complex-mod-a','','','','','','','dev-02','',2,2);"

			"INSERT INTO exe VALUES(12884901904,100,'Play','C:\\ComplexGame\\Charlie.exe','','',0);",
			"INSERT INTO installinfo VALUES(12884901904,100,'C:\\ComplexGame','C:\\ComplexGame\\Charlie.exe','',0,0,0);",
			"INSERT INTO installinfoex VALUES(12884901904,100,'C:\\ComplexGame\\Charlie.exe');",
			"INSERT INTO branchinfo VALUES(3, 12884901904, 'test', 260, 'http://eula.com', 0, 0, '', '', 0, 3, 100);",
			"INSERT INTO iteminfo VALUES(12884901904,4294967328,0,1081374,0,'dev-02','Complex Mod B','complex-mod-b','','','','','','','dev-02','',3,3);"
		};

		{
			sqlite3x::sqlite3_connection db(":memory:");
			setUpDb(db, vSqlCommands);
		}

		ASSERT_TRUE(game->isInstalled());
		ASSERT_TRUE(game->isLaunchable());
		ASSERT_TRUE(game->isComplex());
		ASSERT_EQ(midB, game->getInstalledModId());

		ASSERT_FALSE(modA->isInstalled());
		ASSERT_FALSE(modA->isLaunchable());
		ASSERT_TRUE(modA->isComplex());

		ASSERT_TRUE(modB->isInstalled());
		ASSERT_TRUE(modB->isLaunchable());
		ASSERT_TRUE(modB->isComplex());


		auto modHandle = gcRefPtr<ItemHandleMock>::create(modA, user);
		EXPECT_CALL(*modHandle, goToStageGatherInfo(_, _, _)).Times(1);

		auto ilh = gcRefPtr<ItemLaunchHelperMock>::create();
		modHandle->launch(ilh);
	}
}

#endif
