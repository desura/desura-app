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
#include "DownloadToolItemTask.h"
#include "UserTasks.h"
#include "User.h"



using namespace UserCore::ItemTask;


DownloadToolTask::DownloadToolTask(UserCore::Item::ItemHandle* handle, uint32 ttid, const char* downloadPath, MCFBranch branch, MCFBuild build) 
	: BaseItemTask(UserCore::Item::ITEM_STAGE::STAGE_DOWNLOADTOOL, "DownloadTool", handle, branch, build)
	, m_szDownloadPath(downloadPath)
	, m_ToolTTID(ttid)
{
}

DownloadToolTask::DownloadToolTask(UserCore::Item::ItemHandle* handle, bool launch, uint32 ttid) 
	: BaseItemTask(UserCore::Item::ITEM_STAGE::STAGE_DOWNLOADTOOL, "DownloadTool", handle, MCFBranch(), MCFBuild())
	, m_ToolTTID(ttid)
	, m_bLaunch(launch)
{
}

DownloadToolTask::~DownloadToolTask()
{
}

void DownloadToolTask::doRun()
{
	uint32 per = 0;
	getItemInfo()->getInternal()->setPercent(per);

	if (m_ToolTTID == UINT_MAX)
		validateTools();

	std::vector<DesuraId> toolList;
	getItemInfo()->getCurrentBranch()->getToolList(toolList);

	UserCore::Misc::ToolTransaction* tt = new UserCore::Misc::ToolTransaction(std::move(toolList));

	tt->onCompleteEvent += delegate(this, &DownloadToolTask::onDLComplete);
	tt->onErrorEvent += delegate(this, &DownloadToolTask::onDLError);
	tt->onProgressEvent += delegate(this, &DownloadToolTask::onDLProgress);
	
	if (m_ToolTTID != UINT_MAX)
	{
		bool res = getUserCore()->getToolManager()->updateTransaction(m_ToolTTID, tt);

		if (!res) //must be complete
		{
			onComplete();
			return;
		}
	}
	else
	{
		m_ToolTTID = getUserCore()->getToolManager()->downloadTools(tt);
	}

	if (m_ToolTTID != UINT_MAX)
		m_WaitCond.wait();

	onComplete();
}

void DownloadToolTask::validateTools()
{
	std::vector<DesuraId> toolList;
	getItemInfo()->getCurrentBranch()->getToolList(toolList);

	if (toolList.size() == 0)
		return;

	auto pToolManager = getUserCore()->getToolManager();

	if (pToolManager->areAllToolsValid(toolList))
		return;

	pToolManager->reloadTools(getItemId());
	getItemInfo()->getCurrentBranch()->getToolList(toolList);

	if (!pToolManager->areAllToolsValid(toolList))
		throw gcException(ERR_INVALID, "Tool ids cannot be resolved into tools.");
}

void DownloadToolTask::onPause()
{
}

void DownloadToolTask::onUnpause()
{
}

void DownloadToolTask::onStop()
{
	UserCore::ItemTask::BaseItemTask::onStop();
	m_WaitCond.notify();
}

void DownloadToolTask::cancel()
{
	m_bCancelled = true;
	m_WaitCond.notify();
}

void DownloadToolTask::onDLProgress(UserCore::Misc::ToolProgress &p)
{
	MCFCore::Misc::ProgressInfo m;

	m.doneAmmount = p.done;
	m.totalAmmount = p.total;
	m.percent = p.percent;

	onMcfProgressEvent(m);
	getItemInfo()->getInternal()->setPercent(p.percent);
}

void DownloadToolTask::onDLError(gcException &e)
{
	//Dont worry about errors here. We will sort them out on launch
	Warning("Failed to download tool: {0}\n", e);
}

void DownloadToolTask::onDLComplete()
{
	m_WaitCond.notify();
}

void DownloadToolTask::onComplete()
{
	bool notComplete = isStopped() || m_bCancelled;

	getUserCore()->getToolManager()->removeTransaction(m_ToolTTID, notComplete);
	m_ToolTTID = UINT_MAX;

	std::vector<DesuraId> toolList;
	getItemInfo()->getCurrentBranch()->getToolList(toolList);

	if (!m_bCancelled && !getUserCore()->getToolManager()->areAllToolsDownloaded(toolList))
	{
		gcException e(ERR_INVALID, "Failed to download tools.");
		onErrorEvent(e);
		notComplete = true;
	}

	if (notComplete)
	{
		getItemHandle()->getInternal()->completeStage(true);
		return;
	}

	uint32 blank = 0;
	onCompleteEvent(blank);

	if (HasAllFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_READY | UserCore::Item::ItemInfoI::STATUS_INSTALLED))
	{
		getItemHandle()->getInternal()->goToStageInstallTools(m_bLaunch);
	}
	else
	{
		if (HasAllFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_INSTALLCOMPLEX))
			getItemHandle()->getInternal()->goToStageInstallComplex(getMcfBranch(), getMcfBuild());
		else
			getItemHandle()->getInternal()->goToStageInstall(m_szDownloadPath.c_str(), getMcfBranch());
	}
}
