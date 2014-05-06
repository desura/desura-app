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
#include "UIBranchServiceTask.h"

#include "IPCServiceMain.h"
#include "IPCUninstallBranch.h"

namespace UserCore
{
namespace ItemTask
{


UIBranchServiceTask::UIBranchServiceTask(UserCore::Item::ItemHandle* handle, MCFBranch installBranch, MCFBuild installBuild, bool test) 
	: UIBaseServiceTask(UserCore::Item::ITEM_STAGE::STAGE_UNINSTALL_BRANCH, "UnInstallBranch", handle, installBranch, installBuild)
{
	m_pIPCIM = nullptr;
	m_bTestInstall = test;
}

UIBranchServiceTask::~UIBranchServiceTask()
{
	waitForFinish();

	if (m_pIPCIM)
		m_pIPCIM->destroy();

	m_pIPCIM = nullptr;
}

bool UIBranchServiceTask::initService()
{
	if (!UIBaseServiceTask::initService())
	{
		onComplete();
		return false;
	}

	if (!getItemInfo()->setInstalledMcf(getMcfBranch(), getMcfBuild()))
	{
		gcException eFailedBrchId(ERR_BADID, "Failed to set branch id.");
		onErrorEvent(eFailedBrchId);
		return false;
	}

	gcString oldBranchMcf = getBranchMcf(getItemInfo()->getId(), m_OldBranch, m_OldBuild);
	gcString newBranchMcf = getBranchMcf(getItemInfo()->getId(), getMcfBranch(), getMcfBuild());

	m_pIPCIM = getServiceMain()->newUninstallBranch();
			
	if (!m_pIPCIM)
	{
		gcException eFailCrtBrnch(ERR_NULLHANDLE, "Failed to create uninstall branch mcf service!\n");
		onErrorEvent(eFailCrtBrnch);
		return false;
	}

	m_pIPCIM->onCompleteEvent += delegate(this, &UIBranchServiceTask::onComplete);
	m_pIPCIM->onProgressEvent += delegate(&onMcfProgressEvent);
	m_pIPCIM->onErrorEvent += delegate((UIBaseServiceTask*)this, &UIBaseServiceTask::onServiceError);

	m_pIPCIM->start(oldBranchMcf.c_str(), newBranchMcf.c_str(), getItemInfo()->getPath(), getItemInfo()->getInstallScriptPath());

	return true;
}

void UIBranchServiceTask::onComplete()
{
	MCFCore::Misc::ProgressInfo prog;
	prog.percent = 100;

	onMcfProgressEvent(prog);

	getItemHandle()->getInternal()->goToStageDownload(getMcfBranch(), getMcfBuild(), m_bTestInstall);
	UIBaseServiceTask::onComplete();
}


}
}

