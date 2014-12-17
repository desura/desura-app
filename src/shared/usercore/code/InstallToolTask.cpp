/*
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)
Copyright (C) 2014 Bad Juju Games, Inc.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA.

Contact us at legal@badjuju.com.

*/

#include "Common.h"
#include "InstallToolTask.h"

#include "UserTasks.h"
#include "User.h"



using namespace UserCore::ItemTask;


InstallToolTask::InstallToolTask(gcRefPtr<UserCore::Item::ItemHandleI> handle, bool launch)
	: BaseItemTask(UserCore::Item::ITEM_STAGE::STAGE_INSTALLTOOL, "InstallTool", handle, MCFBranch(), MCFBuild())
{
	m_bInError = false;
	m_ToolTTID = UINT_MAX;
	m_bLaunch = launch;
}

InstallToolTask::~InstallToolTask()
{
}

void InstallToolTask::doRun()
{
	uint32 per = 0;
	getItemInfo()->getInternal()->setPercent(per);

	std::vector<DesuraId> toolList;

	if (!getItemInfo()->getCurrentBranch())
		throw gcException(ERR_INVALID, "Item doesnt have a valid current branch.");

	getItemInfo()->getCurrentBranch()->getToolList(toolList);

	if (!getUserCore()->getToolManager()->areAllToolsValid(toolList))
		throw gcException(ERR_INVALID, "Tool IDs cannot be resolved into tools.");

	if (!getUserCore()->getToolManager()->areAllToolsInstalled(toolList))
	{
		UserCore::Misc::ToolTransaction* tt = new UserCore::Misc::ToolTransaction(toolList);

		tt->onCompleteEvent += delegate(this, &InstallToolTask::onINComplete);
		tt->onErrorEvent += delegate(this, &InstallToolTask::onINError);
		tt->onProgressEvent += delegate(this, &InstallToolTask::onINProgress);
		tt->onStartInstallEvent += delegate(this, &InstallToolTask::onINStart);
		tt->onStartIPCEvent += delegate(this, &InstallToolTask::onIPCStart);

		m_ToolTTID = getUserCore()->getToolManager()->installTools(tt);
		gcException e(ERR_BADID, "Failed to install tools as transaction was cancelled.");

		if (m_ToolTTID == UINT_MAX)
		{
			if (toolList.size() != 0)
				onINError(e);
		}
		else
		{
			m_WaitCond.wait();
		}
	}

	onComplete();
}

void InstallToolTask::onPause()
{
}

void InstallToolTask::onUnpause()
{
}

void InstallToolTask::onStop()
{
	m_WaitCond.notify();
}

void InstallToolTask::onIPCStart()
{
	MCFCore::Misc::ProgressInfo m;
	m.flag = 2;
	onMcfProgressEvent(m);
}

void InstallToolTask::onINStart(DesuraId &id)
{
	MCFCore::Misc::ProgressInfo m;
	m.flag = 1;
	m.totalAmmount = id.toInt64();
	onMcfProgressEvent(m);
}

void InstallToolTask::onINProgress(UserCore::Misc::ToolProgress &p)
{
	MCFCore::Misc::ProgressInfo m;

	m.doneAmmount = p.done;
	m.totalAmmount = p.total;
	m.percent = p.percent;

	onMcfProgressEvent(m);
	getItemInfo()->getInternal()->setPercent(p.percent);
}

void InstallToolTask::onINError(gcException &e)
{
	m_bInError = true;

	onErrorEvent(e);
	m_WaitCond.notify();
}

void InstallToolTask::onINComplete()
{
	m_WaitCond.notify();
}

void InstallToolTask::onComplete()
{
	bool hasError = m_bInError || isStopped();

	if (m_ToolTTID != UINT_MAX)
		getUserCore()->getToolManager()->removeTransaction(m_ToolTTID, hasError);

	m_ToolTTID = UINT_MAX;

	if (hasError)
	{
		getItemHandle()->getInternal()->completeStage(true);
	}
	else
	{
		uint32 blank = 0;
		onCompleteEvent(blank);

		if (m_bLaunch)
			getItemHandle()->getInternal()->goToStageLaunch();
		else
			getItemHandle()->getInternal()->completeStage(false);
	}
}
