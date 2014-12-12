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
#include "VSCheckInstall.h"

#include "ItemHandle.h"
#include "ItemInfo.h"
#include "usercore/UserCoreI.h"
#include "webcore/WebCoreI.h"

namespace UserCore
{
namespace ItemTask
{


bool VSCheckInstall::doTask(bool completeMcf)
{
	if (HasAnyFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_INSTALLED))
	{
		if (checkInstallComplex(completeMcf))
		{
			return true;
		}
		else
		{
			bool res = checkInstall();

			if (isStopped())
				return false;

			if (res)
			{
				setResult(RES_COMPLETE);
			}
			else
			{
				setResult(RES_DOWNLOADMISSING);
				m_hMcf->saveMCFHeader();
			}
		}
	}
	else if (HasAnyFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_INSTALLING))
	{
		//continue installing
		if (!completeMcf)
			setResult(RES_DOWNLOAD);
		else
			setResult(RES_INSTALL);
	}
	else
	{
		return false;
	}

	return true;
}

void VSCheckInstall::onStop()
{
	m_hMcf->stop();
}

bool VSCheckInstall::checkInstall()
{
	try
	{
		return m_hMcf->verifyInstall(getItemInfo()->getPath(), true);
	}
	catch (gcException &except)
	{
		onErrorEvent(except);
		return false;
	}
}

bool VSCheckInstall::checkInstallComplex(bool completeMcf)
{
	if (HasAnyFlags(getItemInfo()->getStatus(), UserCore::Item::ItemInfoI::STATUS_INSTALLCOMPLEX) == false)
		return false;

	auto pItem = getItemInfo();
	auto pParent = getParentItemInfo();

	if (pParent)
	{
		if (!pParent->getInstalledModId().isOk() || pParent->getInstalledModId() != pItem->getId())
		{
			if (completeMcf)
				setResult(RES_COMPLETE);
			else
				setResult(RES_DOWNLOADMISSING);

			return true;
		}
	}
	else if (pItem->getInstalledModId().isOk())
	{
		setResult(RES_COMPLETE);
		return true;
	}

	return false;
}


}
}
