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
#include "UserTask.h"
#include "User.h"

namespace UserCore
{
namespace Task
{


UserTask::UserTask(UserCore::UserI *user, DesuraId id)
{
	m_pUserCore = user;

	if (user)
		m_pWebCore = user->getWebCore();
	else
		m_pWebCore = nullptr;

	m_iId = id;
	m_bStopped = false;
}

UserTask::~UserTask()
{
}

UserCore::Item::ItemInfo* UserTask::getItemInfo()
{
	if (!m_pUserCore || !m_iId.isOk())
		return nullptr;

	return dynamic_cast<UserCore::Item::ItemInfo*>(m_pUserCore->getItemManager()->findItemInfo(m_iId));
}

void UserTask::onStop()
{
	m_bStopped = true;
}

volatile bool UserTask::isStopped()
{
	return m_bStopped;
}

}
}