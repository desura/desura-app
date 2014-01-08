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
#include "BDManager.h"
#include "User.h"

namespace UserCore
{

BDManager::BDManager(UserCore::User* user)
{
	m_pUser = user;
}

BDManager::~BDManager()
{
	m_BannerLock.lock();

	std::map<UserCore::Task::DownloadBannerTask*, UserCore::Misc::BannerNotifierI*>::iterator it;

	for (it=m_mDownloadBannerTask.begin(); it != m_mDownloadBannerTask.end(); it++)
	{
		it->first->onDLCompleteEvent -= delegate(this, &BDManager::onBannerComplete);
	}

	m_BannerLock.unlock();
}

void BDManager::onBannerComplete(UserCore::Task::BannerCompleteInfo& bci)
{
	m_BannerLock.lock();

	std::map<UserCore::Task::DownloadBannerTask*, UserCore::Misc::BannerNotifierI*>::iterator it;

	for (it=m_mDownloadBannerTask.begin(); it != m_mDownloadBannerTask.end(); it++)
	{
		if (it->first == bci.task)
		{
			if (bci.complete)
				it->second->onBannerComplete(bci.info);

			m_mDownloadBannerTask.erase(it);

			break;
		}
	}

	m_BannerLock.unlock();
}

void BDManager::downloadBanner(UserCore::Misc::BannerNotifierI* obj, const MCFCore::Misc::DownloadProvider& provider)
{
	m_BannerLock.lock();

	UserCore::Task::DownloadBannerTask *task = new UserCore::Task::DownloadBannerTask(m_pUser, provider);
	task->onDLCompleteEvent += delegate(this, &BDManager::onBannerComplete);

	m_mDownloadBannerTask[task] = obj;
	m_pUser->getThreadPool()->queueTask(task);

	m_BannerLock.unlock();
}

void BDManager::cancelDownloadBannerHooks(UserCore::Misc::BannerNotifierI* obj)
{
	m_BannerLock.lock();

	std::map<UserCore::Task::DownloadBannerTask*, UserCore::Misc::BannerNotifierI*>::iterator it=m_mDownloadBannerTask.begin();

	while (it != m_mDownloadBannerTask.end())
	{
		if (it->second == obj)
		{
			std::map<UserCore::Task::DownloadBannerTask*, UserCore::Misc::BannerNotifierI*>::iterator temp = it;
			it++;

			m_mDownloadBannerTask.erase(temp);
		}
		else
		{
			it++;
		}
	}

	m_BannerLock.unlock();
}


}