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
#include "InstallCheckTask.h"
#include "managers/WildcardManager.h"

using namespace UserCore::ItemTask;

InstallCheckTask::InstallCheckTask(gcRefPtr<UserCore::Item::ItemHandleI> handle)
	: BaseItemTask(UserCore::Item::ITEM_STAGE::STAGE_INSTALL_CHECK, "InstallCheck", handle)
{
}

void InstallCheckTask::doRun()
{
	auto wildc = gcRefPtr<WildcardManager>::create();
	wildc->onNeedSpecialEvent += delegate(&onNeedWCEvent);

	getUserCore()->getItemManager()->retrieveItemInfo(getItemId(), 0, wildc);

	uint32 prog = 0;
	onCompleteEvent(prog);

	getItemHandle()->getInternal()->completeStage(false);
}
