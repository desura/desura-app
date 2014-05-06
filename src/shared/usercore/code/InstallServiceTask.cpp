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
#include "InstallServiceTask.h"

#include "User.h"
#include "ItemInfo.h"
#include "ItemHandle.h"

#include "IPCServiceMain.h"
#include "IPCInstallMcf.h"
#include "usercore/ItemHelpersI.h"

#include "McfManager.h"

#ifdef NIX
#include "util/UtilLinux.h"
#endif

using namespace UserCore::ItemTask;


InstallServiceTask::InstallServiceTask(UserCore::Item::ItemHandle* handle, const char* path, MCFBranch branch, UserCore::Item::Helper::InstallerHandleHelperI* ihh) 
	: BaseItemServiceTask(UserCore::Item::ITEM_STAGE::STAGE_INSTALL, "Install", handle, branch)
{
	m_szPath = gcString(path);

	m_bHasError = false;
	m_bInstalling = false;
	m_bHashMissMatch = false;
	m_pIPCIM = nullptr;
	m_pIHH = ihh;
}

InstallServiceTask::~InstallServiceTask()
{
	gcTrace("");

	gcException eFailCreateHandle(ERR_NULLHANDLE, "Failed to create install mcf service!\n");

	if (hasStarted())
		waitForFinish();

	if (m_pIPCIM)
		m_pIPCIM->destroy();

	if (m_pIHH)
		m_pIHH->destroy();
}

bool InstallServiceTask::initService()
{
	gcTrace("");

	gcException eBadItem(ERR_BADITEM);

	auto pItem = getItemInfo();
	if (!pItem)
	{
		onErrorEvent(eBadItem);
		return false;
	}

	m_pIPCIM = getServiceMain()->newInstallMcf();
	if (!m_pIPCIM)
	{
		gcException eFailCreateHandle(ERR_NULLHANDLE, "Failed to create install mcf service!\n");
		onErrorEvent(eFailCreateHandle);
		return false;
	}

	if (!pItem->isUpdating())
	{
		pItem->getInternal()->setPercent(0);
		pItem->delSFlag(UserCore::Item::ItemInfoI::STATUS_DOWNLOADING|UserCore::Item::ItemInfoI::STATUS_READY);
		pItem->addSFlag(UserCore::Item::ItemInfoI::STATUS_INSTALLING);
	}

	const char* val = getUserCore()->getCVarValue("gc_corecount");
	bool removeFiles = (pItem->getOptions() & UserCore::Item::ItemInfoI::OPTION_REMOVEFILES)?true:false;

	gcString gc_writeable = getUserCore()->getCVarValue("gc_ignore_windows_permissions_against_marks_wishes");

	bool makeWritable = (gc_writeable == "true" || gc_writeable == "1");

	m_pIPCIM->onCompleteEvent += delegate(this, &InstallServiceTask::onComplete);
	m_pIPCIM->onProgressEvent += delegate(this, &InstallServiceTask::onProgUpdate);
	m_pIPCIM->onErrorEvent += delegate(this, &InstallServiceTask::onError);
	m_pIPCIM->onFinishEvent += delegate(this, &InstallServiceTask::onFinish);

	uint8 workers = 1;
		
	if (val)
		workers = atoi(val);

	if (workers == 0)
		workers = 1;

	m_pIPCIM->start(m_szPath.c_str(), pItem->getPath(), getItemInfo()->getInstallScriptPath(), workers, removeFiles, makeWritable);

	return true;
}

void InstallServiceTask::onComplete()
{
	gcTrace("");

	if (m_bHasError)
	{
		onFinish();
		return;
	}


#ifdef NIX
	getItemHandle()->installLaunchScripts();
#endif

	auto pItem = getItemInfo();

	if (pItem->isUpdating() && getMcfBuild() == pItem->getNextUpdateBuild())
		pItem->updated();

	pItem->delSFlag(UserCore::Item::ItemInfoI::STATUS_INSTALLING|UserCore::Item::ItemInfoI::STATUS_UPDATING|UserCore::Item::ItemInfoI::STATUS_DOWNLOADING);
	pItem->addSFlag(UserCore::Item::ItemInfoI::STATUS_INSTALLED|UserCore::Item::ItemInfoI::STATUS_READY);
	
	if (pItem->isUpdating())
		pItem->addSFlag(UserCore::Item::ItemInfoI::STATUS_NEEDCLEANUP);

	MCFCore::Misc::ProgressInfo temp;
	temp.percent = 100;

	onMcfProgressEvent(temp);

	UserCore::Item::ItemInfoI *item = getItemHandle()->getItemInfo();
	item->delSFlag(UserCore::Item::ItemInfoI::STATUS_PAUSABLE);

	bool verify = false;

	if (m_bHashMissMatch && m_pIHH)
		verify = m_pIHH->verifyAfterHashFail();

	uint32 com = m_bHashMissMatch?1:0;
	onCompleteEvent(com);

	if (verify)
		getItemHandle()->getInternal()->goToStageVerify(getItemInfo()->getInstalledBranch(), getItemInfo()->getInstalledBuild(), true, true, true);
	else
		getItemHandle()->getInternal()->completeStage(false);

	onFinish();

	gcTrace("End");
}


void InstallServiceTask::onPause()
{
	if (m_pIPCIM)
		m_pIPCIM->pause();
}


void InstallServiceTask::onUnpause()
{
	if (m_pIPCIM)
		m_pIPCIM->unpause();
}

void InstallServiceTask::onStop()
{
	if (m_pIPCIM)
		m_pIPCIM->stop();

	BaseItemServiceTask::onStop();
}

void InstallServiceTask::onProgUpdate(MCFCore::Misc::ProgressInfo& info)
{
	if (info.flag == 0)
	{
		if (getItemInfo()->isUpdating())
		{
			//for updating installing is the second 50%
			getItemInfo()->getInternal()->setPercent(50+info.percent/2);
		}
		else
		{
			getItemInfo()->getInternal()->setPercent(info.percent);
		}

		if (!(getItemHandle()->getItemInfo()->getStatus() & UserCore::Item::ItemInfoI::STATUS_INSTALLCOMPLEX))
			getItemHandle()->getItemInfo()->addSFlag(UserCore::Item::ItemInfoI::STATUS_PAUSABLE);
	}

	onMcfProgressEvent(info);
}

void InstallServiceTask::onError(gcException &e)
{
	if (e.getErrId() == ERR_HASHMISSMATCH)
	{
		m_bHashMissMatch = true;
		return;
	}

	m_bHasError = true;

	Warning("Error in MCF install: {0}\n", e);
	getItemHandle()->getInternal()->setPausable(false);

	if (!getItemHandle()->shouldPauseOnError())
	{
		getItemInfo()->delSFlag(UserCore::Item::ItemInfoI::STATUS_INSTALLING|UserCore::Item::ItemInfoI::STATUS_UPDATING|UserCore::Item::ItemInfoI::STATUS_DOWNLOADING);
		getItemHandle()->getInternal()->resetStage(true);
	}
	else
	{
		getItemHandle()->getInternal()->setPaused(true, true);
	}

	onErrorEvent(e);
}

void InstallServiceTask::onFinish()
{
	gcTrace("");
	BaseItemServiceTask::onFinish();
}
