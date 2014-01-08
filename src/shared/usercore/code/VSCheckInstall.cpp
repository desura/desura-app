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

	UserCore::Item::ItemInfoI* pItem = getItemInfo();
	UserCore::Item::ItemInfoI* pParent = getParentItemInfo();

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
