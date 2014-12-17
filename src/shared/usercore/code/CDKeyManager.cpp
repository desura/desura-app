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
#include "CDKeyManager.h"
#include "User.h"


using namespace UserCore;


CDKeyManager::CDKeyManager(gcRefPtr<UserCore::User> user)
	: m_pUser(user)
{
}

CDKeyManager::~CDKeyManager()
{
	std::lock_guard<std::mutex> guard(m_TaskListLock);

	for (auto t : m_vCDKeyTaskList)
	{
		t->onCompleteEvent -= delegate(this, &CDKeyManager::onCDKeyComplete);
		t->onErrorEvent -= delegate(this, &CDKeyManager::onCDKeyError);
	}
}

void CDKeyManager::getCDKeyForCurrentBranch(DesuraId id, gcRefPtr<UserCore::Misc::CDKeyCallBackI> callback)
{
	if (!callback)
		return;

	gcRefPtr<UserCore::Item::ItemInfoI> info;
	gcRefPtr<UserCore::Item::BranchInfo> binfo;

	try
	{
		info = m_pUser->getItemManager()->findItemInfo(id);

		if (!info)
			throw gcException(ERR_CDKEY, 110, "Item can not be found");

		binfo = gcRefPtr<UserCore::Item::BranchInfo>(dynamic_cast<UserCore::Item::BranchInfo*>(info->getCurrentBranch().get()));

		if (!binfo)
			throw gcException(ERR_CDKEY, 110, "Item has no currently installed branches");

		if (!hasCDKeyForCurrentBranch(id))
			throw gcException(ERR_CDKEY, 117, "No CDKey required for item");
	}
	catch (gcException &e)
	{
		callback->onCDKeyError(id, e);
		return;
	}


	std::vector<gcString> vCDKeys;
	binfo->getCDKey(vCDKeys);

	if (!vCDKeys.empty())
	{
		gcString key(vCDKeys[0]);
		callback->onCDKeyComplete(id, key);
		return;
	}

	std::lock_guard<std::mutex> guard(m_TaskListLock);

	auto task = gcRefPtr<UserCore::Task::CDKeyTask>::create(m_pUser, id);
	task->onCompleteEvent += delegate(this, &CDKeyManager::onCDKeyComplete);
	task->onErrorEvent += delegate(this, &CDKeyManager::onCDKeyError);

	m_mCDKeyCallbackList.push_back(std::pair<DesuraId, gcRefPtr<UserCore::Misc::CDKeyCallBackI>>(id, callback));
	m_pUser->getThreadPool()->forceTask(task);
}

void CDKeyManager::cancelRequest(DesuraId id, gcRefPtr<UserCore::Misc::CDKeyCallBackI> callback)
{
	std::lock_guard<std::mutex> guard(m_TaskListLock);

	CDKeyList::iterator it=m_mCDKeyCallbackList.begin();

	while (it != m_mCDKeyCallbackList.end())
	{
		if (it->second == callback && it->first == id)
		{
			CDKeyList::iterator temp = it;
			++it;

			m_mCDKeyCallbackList.erase(temp);
		}
		else
		{
			it++;
		}
	}
}

bool CDKeyManager::hasCDKeyForCurrentBranch(DesuraId id)
{
	auto info = m_pUser->getItemManager()->findItemInfo(id);

	if (!info)
		return false;

	auto binfo = info->getCurrentBranch();

	if (!binfo)
		return false;

	return binfo->hasCDKey();
}

void CDKeyManager::onCDKeyComplete(UserCore::Task::CDKeyEventInfo<gcString> &info)
{
	gcRefPtr<UserCore::Item::ItemInfoI> iinfo;
	gcRefPtr<UserCore::Item::BranchInfo> binfo;

	iinfo = m_pUser->getItemManager()->findItemInfo(info.id);

	if (iinfo)
	{
		binfo = gcRefPtr<UserCore::Item::BranchInfo>(dynamic_cast<UserCore::Item::BranchInfo*>(iinfo->getCurrentBranch().get()));

		if (binfo)
			binfo->setCDKey(info.t);
	}


	{
		std::lock_guard<std::mutex> guard(m_TaskListLock);

		std::vector<size_t> delList;

		for (size_t x = 0; x < m_mCDKeyCallbackList.size(); x++)
		{
			if (m_mCDKeyCallbackList[x].first == info.id)
			{
				if (m_mCDKeyCallbackList[x].second)
					m_mCDKeyCallbackList[x].second->onCDKeyComplete(info.id, info.t);

				delList.push_back(x);
			}
		}

		std::for_each(delList.rbegin(), delList.rend(), [this](size_t x){
			m_mCDKeyCallbackList.erase(m_mCDKeyCallbackList.begin() + x);
		});
	}

	removeTask(info.task);
}

void CDKeyManager::onCDKeyError(UserCore::Task::CDKeyEventInfo<gcException> &info)
{
	{
		std::lock_guard<std::mutex> guard(m_TaskListLock);

		std::vector<size_t> delList;

		for (size_t x = 0; x < m_mCDKeyCallbackList.size(); x++)
		{
			if (m_mCDKeyCallbackList[x].first == info.id)
			{
				if (m_mCDKeyCallbackList[x].second)
					m_mCDKeyCallbackList[x].second->onCDKeyError(info.id, info.t);

				delList.push_back(x);
			}
		}

		std::for_each(delList.rbegin(), delList.rend(), [this](size_t x){
			m_mCDKeyCallbackList.erase(m_mCDKeyCallbackList.begin() + x);
		});
	}

	removeTask(info.task);
}

void CDKeyManager::removeTask(gcRefPtr<UserCore::Task::CDKeyTask> &task)
{
	std::lock_guard<std::mutex> guard(m_TaskListLock);

	for (size_t x=0; x<m_vCDKeyTaskList.size(); x++)
	{
		if (m_vCDKeyTaskList[x] == task)
		{
			m_vCDKeyTaskList.erase(m_vCDKeyTaskList.begin()+x);
			break;
		}
	}
}
