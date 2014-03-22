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
#include "BaseItemTask.h"

#include "usercore/ItemHandleI.h"

#include "ItemInfo.h"
#include "ItemHandle.h"
#include "User.h"
#include "webcore/WebCoreI.h"


using namespace UserCore::ItemTask;


BaseItemTask::BaseItemTask(UserCore::Item::ITEM_STAGE type, const char* name, UserCore::Item::ItemHandleI* handle, MCFBranch branch, MCFBuild build)
	: m_pHandle(handle)
	, m_uiType(type)
	, m_uiMcfBranch(branch)
	, m_uiMcfBuild(build)
	, m_szName(name)
{
	assert(handle);
}

BaseItemTask::~BaseItemTask()
{
}

const char* BaseItemTask::getTaskName()
{
	return m_szName.c_str();
}

UserCore::Item::ITEM_STAGE BaseItemTask::getTaskType()
{
	return m_uiType;
}

void BaseItemTask::setWebCore(WebCore::WebCoreI *wc)
{
	m_pWebCore = wc;
}

void BaseItemTask::setUserCore(UserCore::UserI *uc)
{
	m_pUserCore = uc;
}

void BaseItemTask::doTask()
{
	if (!m_pWebCore || !m_pUserCore)
	{
		gcException e(ERR_BADCLASS);
		onErrorEvent(e);
		return;
	}

	try
	{
		doRun();
	}
	catch (gcException& e)
	{
		onErrorEvent(e);
	}
}

void BaseItemTask::onStop()
{
	m_bIsStopped = true;

	if (m_hMCFile.handle())
		m_hMCFile->stop();
}

void BaseItemTask::onPause()
{
	m_bIsPaused = true;
}

void BaseItemTask::onUnpause()
{
	m_bIsPaused = false;
}

void BaseItemTask::cancel()
{

}

UserCore::Item::ItemHandleI* BaseItemTask::getItemHandle()
{
	return m_pHandle;
}

UserCore::Item::ItemInfoI* BaseItemTask::getItemInfo()
{
	if (!m_pHandle)
		return nullptr;

	return m_pHandle->getItemInfo();
}

UserCore::Item::ItemInfoI* BaseItemTask::getParentItemInfo()
{
	UserCore::Item::ItemInfoI* item = getItemInfo();

	if (!m_pUserCore || !item)
		return nullptr;

	return m_pUserCore->getItemManager()->findItemInfo(item->getParentId());
}

DesuraId BaseItemTask::getItemId()
{
	return getItemInfo()->getId();
}

WebCore::WebCoreI* BaseItemTask::getWebCore()
{
	return m_pWebCore;
}

UserCore::UserI* BaseItemTask::getUserCore()
{
	return m_pUserCore;
}

MCFBuild BaseItemTask::getMcfBuild()
{
	return m_uiMcfBuild;
}

MCFBranch BaseItemTask::getMcfBranch()
{
	return m_uiMcfBranch;
}

bool BaseItemTask::isStopped()
{
	return m_bIsStopped;
}

bool BaseItemTask::isPaused()
{
	return m_bIsPaused;
}


