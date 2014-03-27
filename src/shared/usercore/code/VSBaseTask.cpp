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
#include "VSBaseTask.h"

#include "usercore/ItemHandleI.h"

#include "ItemInfo.h"
#include "ItemHandle.h"
#include "User.h"
#include "webcore/WebCoreI.h"

namespace UserCore
{
namespace ItemTask
{

VSBaseTask::VSBaseTask()
{
	m_bIsStopped = false;

	m_pHandle = nullptr;
	m_pWebCore = nullptr;
	m_pUserCore = nullptr;

	m_Result = RES_NONE;
}

void VSBaseTask::setWebCore(WebCore::WebCoreI *wc)
{
	m_pWebCore = wc;
}

void VSBaseTask::setUserCore(UserCore::UserI *uc)
{
	m_pUserCore = uc;
}

void VSBaseTask::setItemHandle(UserCore::Item::ItemHandleI* handle)
{
	m_pHandle = handle;
}

void VSBaseTask::setMcfBuild(MCFBuild build)
{
	m_uiMcfBuild = build;
}

void VSBaseTask::setMcfBranch(MCFBranch branch)
{
	m_uiMcfBranch = branch;
}


bool VSBaseTask::loadMcf(gcString mcfPath)
{
	//read file to make sure it is what it says it is
	m_hMcf->setFile(mcfPath.c_str());
		
	try
	{
		m_hMcf->parseMCF();
	}
	catch (gcException &except)
	{
		Warning("Verify - MCF Error: {0}\n", except);
		return false;
	}

	return true;
}

void VSBaseTask::setMcfHandle(MCFCore::MCFI* handle)
{
	m_hMcf = handle;
}


void VSBaseTask::setResult(RESULT res)
{
	if (m_Result != RES_STOPPED)
		m_Result = res;
}

VSBaseTask::RESULT VSBaseTask::getResult()
{
	return m_Result;
}

void VSBaseTask::stop()
{
	m_bIsStopped = true;
	m_Result = RES_STOPPED;

	onStop();
}



UserCore::Item::ItemHandleI* VSBaseTask::getItemHandle()
{
	return m_pHandle;
}

UserCore::Item::ItemInfoI* VSBaseTask::getItemInfo()
{
	return m_pHandle->getItemInfo();
}

UserCore::Item::ItemInfoI* VSBaseTask::getParentItemInfo()
{
	UserCore::Item::ItemInfoI* item = getItemInfo();

	if (!m_pUserCore || !item)
		return nullptr;

	return m_pUserCore->getItemManager()->findItemInfo(item->getParentId());
}

DesuraId VSBaseTask::getItemId()
{
	return getItemInfo()->getId();
}

WebCore::WebCoreI* VSBaseTask::getWebCore()
{
	return m_pWebCore;
}

UserCore::UserI* VSBaseTask::getUserCore()
{
	return m_pUserCore;
}

MCFBuild VSBaseTask::getMcfBuild()
{
	return m_uiMcfBuild;
}

MCFBranch VSBaseTask::getMcfBranch()
{
	return m_uiMcfBranch;
}

bool VSBaseTask::isStopped()
{
	return m_bIsStopped;
}

}
}
