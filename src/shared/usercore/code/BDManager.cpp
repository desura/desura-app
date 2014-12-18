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
#include "BDManager.h"
#include "User.h"

using namespace UserCore;


BDManager::BDManager(gcRefPtr<UserCore::User> user)
	: m_pUser(user)
{
}

BDManager::~BDManager()
{
	cleanup();
}

void BDManager::cleanup()
{
	{
		std::lock_guard<std::recursive_mutex> guard(m_BannerLock);
		m_mDownloadBannerTask.clear();
	}

	m_pUser.reset();
}

void BDManager::onBannerComplete(UserCore::Task::BannerCompleteInfo& bci)
{
	std::lock_guard<std::recursive_mutex> guard(m_BannerLock);

	auto it = std::find_if(begin(m_mDownloadBannerTask), end(m_mDownloadBannerTask), [&bci](std::pair<const gcRefPtr<UserCore::Task::DownloadBannerTask>, gcRefPtr<UserCore::Misc::BannerNotifierI>> p){
		return p.first == bci.task;
	});

	if (it == end(m_mDownloadBannerTask))
		return;

	if (bci.complete)
		it->second->onBannerComplete(bci.info);

	m_mDownloadBannerTask.erase(it);
}

void BDManager::downloadBanner(gcRefPtr<UserCore::Misc::BannerNotifierI> obj, const MCFCore::Misc::DownloadProvider& provider)
{
	std::lock_guard<std::recursive_mutex> guard(m_BannerLock);

	auto task = gcRefPtr<UserCore::Task::DownloadBannerTask>::create(m_pUser, provider);
	task->onDLCompleteEvent += delegate(this, &BDManager::onBannerComplete);

	m_mDownloadBannerTask[task] = obj;
	m_pUser->getThreadPool()->queueTask(task);
}

void BDManager::cancelDownloadBannerHooks(UserCore::Misc::BannerNotifierI *pObj)
{
	std::lock_guard<std::recursive_mutex> guard(m_BannerLock);

	auto it = std::find_if(begin(m_mDownloadBannerTask), end(m_mDownloadBannerTask), [pObj](std::pair<const gcRefPtr<UserCore::Task::DownloadBannerTask>, gcRefPtr<UserCore::Misc::BannerNotifierI>> p){
		return p.second.get() == pObj;
	});

	if (it != end(m_mDownloadBannerTask))
		m_mDownloadBannerTask.erase(it);
}
