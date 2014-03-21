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
#include "InstallCheckTask.h"
#include "managers/WildcardManager.h"

namespace UserCore
{
namespace ItemTask
{

InstallCheckTask::InstallCheckTask(UserCore::Item::ItemHandle *handle) : BaseItemTask(UserCore::Item::ITEM_STAGE::STAGE_INSTALL_CHECK, "InstallCheck", handle)
{
}

void InstallCheckTask::doRun()
{
	WildcardManager wildc = WildcardManager();
	wildc.onNeedSpecialEvent += delegate(&onNeedWCEvent);
	wildc.onNeedSpecialEvent += delegate(getUserCore()->getNeedWildCardEvent());

	getUserCore()->getItemManager()->retrieveItemInfo(getItemId(), 0, &wildc);

	uint32 prog = 0;
	onCompleteEvent(prog);

	getItemHandle()->getInternal()->completeStage(false);
}

}
}