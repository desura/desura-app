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
#include "GatherInfoThread.h"
#include "managers/WildcardManager.h"

#include "ItemInfo.h"
#include "ItemHandle.h"

namespace UserCore
{
namespace Thread
{

GatherInfoThread::GatherInfoThread(DesuraId id, MCFBranch branch, MCFBuild build) : MCFThread( "GatherInfo Thread", id, branch, build )
{
}


void GatherInfoThread::doRun()
{
	WildcardManager wildc = WildcardManager();
	wildc.onNeedSpecialEvent += delegate(&onNeedWCEvent);

	uint32 prog = 0;
	onProgUpdateEvent(prog);
	getUserCore()->getItemManager()->retrieveItemInfo(getItemId(), 0, &wildc, MCFBranch::BranchFromInt(getMcfBranch()), MCFBuild::BuildFromInt(getMcfBuild()));

	if (isStopped())
		return;

	auto item = getItemInfo();

	if (!item)
		throw gcException(ERR_INVALIDDATA, "The item handle was null (gather info failed)");

	uint32 itemId = item->getId().getItem();
	onCompleteEvent(itemId);
}


}
}
