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
#include "UIUpdateServiceTask.h"

#include "IPCServiceMain.h"
#include "IPCUninstallBranch.h"

using namespace UserCore::ItemTask;


UIUpdateServiceTask::UIUpdateServiceTask(gcRefPtr<UserCore::Item::ItemHandleI> handle, const char* path, MCFBuild lastBuild)
	: UIBaseServiceTask(UserCore::Item::ITEM_STAGE::STAGE_UNINSTALL_UPDATE, "UnInstallUpdate", handle, MCFBranch(), lastBuild)
{
	m_pIPCIM = nullptr;
	m_szPath = path;
}

UIUpdateServiceTask::~UIUpdateServiceTask()
{
	waitForFinish();

	if (m_pIPCIM)
		m_pIPCIM->destroy();

	m_pIPCIM = nullptr;
}

bool UIUpdateServiceTask::initService()
{
	if (!UIBaseServiceTask::initService())
	{
		onComplete();
		return false;
	}

	gcString oldBranchMcf = getBranchMcf(getItemInfo()->getId(), getItemInfo()->getInstalledBranch(), getMcfBuild());

	m_pIPCIM = getServiceMain()->newUninstallBranch();

	if (!m_pIPCIM)
	{
		gcException eFailCrtBrnch(ERR_NULLHANDLE, "Failed to create uninstall update mcf service!\n");
		onErrorEvent(eFailCrtBrnch);
		return false;
	}

	m_pIPCIM->onCompleteEvent += delegate(this, &UIUpdateServiceTask::onComplete);
	m_pIPCIM->onProgressEvent += delegate(&onMcfProgressEvent);
	m_pIPCIM->onErrorEvent += delegate((UIBaseServiceTask*)this, &UIBaseServiceTask::onServiceError);

	m_pIPCIM->start(oldBranchMcf.c_str(), m_szPath.c_str(), getItemInfo()->getPath(), "");

	return true;
}

void UIUpdateServiceTask::onComplete()
{
	MCFCore::Misc::ProgressInfo prog;
	prog.percent = 100;

	onMcfProgressEvent(prog);

	getItemHandle()->getInternal()->goToStageDownload(m_szPath.c_str());
	UIBaseServiceTask::onComplete();
}

