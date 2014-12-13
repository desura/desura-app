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
#include "ItemTaskGroup.h"
#include "ItemHandle.h"
#include "ItemHandleEvents.h"
#include "ItemManager.h"

using namespace UserCore::Item;


ItemTaskGroup::ItemTaskGroup(gcRefPtr<UserCore::ItemManager> manager, ACTION action, uint8 activeCount)
	: m_iActiveCount(activeCount)
	, m_Action(action)
	, m_pItemManager(manager)
{
}

ItemTaskGroup::~ItemTaskGroup()
{

}

UserCore::Item::ItemTaskGroupI::ACTION ItemTaskGroup::getAction()
{
	return m_Action;
}

void ItemTaskGroup::getItemList(std::vector<gcRefPtr<UserCore::Item::ItemHandleI>> &list)
{
	std::lock_guard<std::recursive_mutex> guard(m_ListLock);

	for (size_t x=0; x<m_vWaitingList.size(); x++)
	{
		list.push_back(m_vWaitingList[x]);
	}
}

void ItemTaskGroup::cancelAll()
{
	{
		std::lock_guard<std::recursive_mutex> guard(m_TaskListLock);
		m_vTaskList.clear();
	}

	std::lock_guard<std::recursive_mutex> guard(m_ListLock);
	for (auto i : m_vWaitingList)
	{
		auto handle = gcRefPtr<ItemHandle>::dyn_cast(i);

		if (handle)
			handle->setTaskGroup(nullptr);
	}

	m_vWaitingList.clear();
}

bool ItemTaskGroup::addItem(gcRefPtr<UserCore::Item::ItemInfoI> item)
{
	return addItem(m_pItemManager->findItemHandle(item->getId()));
}

bool ItemTaskGroup::addItem(gcRefPtr<ItemHandleI> item)
{
	if (!item)
		return false;

	auto handle = gcRefPtr<ItemHandle>::dyn_cast(item);

	if (!handle)
		return false;

	if (!handle->setTaskGroup(this))
		return false;

	{
		std::lock_guard<std::recursive_mutex> guard(m_ListLock);

		for (size_t x = 0; x < m_vWaitingList.size(); x++)
		{
			if (m_vWaitingList[x] == handle)
				return true;
		}

		m_vWaitingList.push_back(handle);
	}

	if (m_bStarted && m_uiActiveItem == UINT_MAX)
		nextItem();

	uint32 p = m_vWaitingList.size();

	{
		std::lock_guard<std::recursive_mutex> guard(m_TaskListLock);
		for (size_t x = 0; x < m_vTaskList.size(); x++)
		{
			if (m_vTaskList[x]->getItemHandle() != getActiveItem())
				m_vTaskList[x]->onProgUpdateEvent(p);
		}
	}

	return true;
}

bool ItemTaskGroup::removeItem(gcRefPtr<ItemHandleI> item)
{
	if (!item)
		return false;

	auto handle = gcRefPtr<ItemHandle>::dyn_cast(item);

	if (!handle)
		return false;

	bool found = false;

	{
		std::lock_guard<std::recursive_mutex> guard(m_ListLock);
		for (size_t x = 0; x < m_vWaitingList.size(); x++)
		{
			if (m_vWaitingList[x] == handle)
			{
				if (m_uiActiveItem > x)
					--m_uiActiveItem;
				else if (m_uiActiveItem == x)
					m_uiActiveItem = -1;

				if (m_uiLastActive >= x)
					m_uiLastActive--;

				found = true;
				m_vWaitingList.erase(m_vWaitingList.begin() + x);
				break;
			}
		}
	}

	if (found)
	{
		handle->delHelper(this);
		handle->setTaskGroup(nullptr);
	}


	uint32 p=m_vWaitingList.size();

	std::lock_guard<std::recursive_mutex> guard(m_TaskListLock);
	for (size_t x=0; x<m_vTaskList.size(); x++)
	{
		if (m_vTaskList[x]->getItemHandle() != getActiveItem())
			m_vTaskList[x]->onProgUpdateEvent(p);
	}

	return true;
}

gcRefPtr<ItemHandleI> ItemTaskGroup::getActiveItem()
{
	if (m_uiActiveItem >= m_vWaitingList.size())
		return nullptr;

	return m_vWaitingList[m_uiActiveItem];
}

void ItemTaskGroup::nextItem()
{
	onProgressUpdate(100);

	gcRefPtr<ItemHandleI> item = getActiveItem();

	if (item)
		item->delHelper(this);

	if (m_uiActiveItem+1 >=  m_vWaitingList.size())
	{
		m_uiLastActive = m_uiActiveItem;
		m_uiActiveItem = UINT_MAX;

		if (m_bFinal)
			finish();
	}
	else
	{
		if (m_uiLastActive != UINT_MAX)
		{
			m_uiActiveItem = m_uiLastActive;
			m_uiLastActive = UINT_MAX;
		}

		m_uiActiveItem++;

		item = getActiveItem();
		item->addHelper(this);
		startAction(gcRefPtr<ItemHandle>::dyn_cast(item));
	}
}

void ItemTaskGroup::startAction(gcRefPtr<UserCore::Item::ItemHandle> item)
{
	if (!item)
		return;

	switch (m_Action)
	{
	case A_VERIFY:
		item->verifyOveride();
		break;

	case A_UNINSTALL:
		item->uninstallOveride();
		break;
	};
}

void ItemTaskGroup::finalize()
{
	m_bFinal = true;

	if (!getActiveItem())
		finish();
}

void ItemTaskGroup::start()
{
	m_bStarted = true;

	if (!getActiveItem())
		nextItem();
}

void ItemTaskGroup::pause()
{
	m_bPaused = true;
}

void ItemTaskGroup::unpause()
{
	m_bPaused = false;
}

void ItemTaskGroup::cancel()
{
	m_bStarted = false;
}

void ItemTaskGroup::finish()
{
	cancelAll();
}

void ItemTaskGroup::onComplete(uint32 status)
{
	nextItem();
}

void ItemTaskGroup::onComplete(gcString& string)
{
	nextItem();
}

void ItemTaskGroup::onVerifyComplete(UserCore::Misc::VerifyComplete& info)
{
	nextItem();
}

void ItemTaskGroup::onMcfProgress(MCFCore::Misc::ProgressInfo& info)
{
	onProgressUpdate(info.percent);
}

void ItemTaskGroup::onProgressUpdate(uint32 progress)
{
	MCFCore::Misc::ProgressInfo i;

	i.percent = progress;

	if (getActiveItem())
		i.totalAmmount = getActiveItem()->getItemInfo()->getId().toInt64();

	std::lock_guard<std::recursive_mutex> guard(m_TaskListLock);
	for (size_t x=0; x<m_vTaskList.size(); x++)
	{
		if (m_vTaskList[x]->getItemHandle() != getActiveItem())
			m_vTaskList[x]->onMcfProgressEvent(i);
	}
}

void ItemTaskGroup::onError(gcException e)
{
	nextItem();
}

void ItemTaskGroup::onNeedWildCard(WCSpecialInfo& info)
{
}

void ItemTaskGroup::onDownloadProvider(UserCore::Misc::GuiDownloadProvider &provider)
{
}

uint32 ItemTaskGroup::getId()
{
	return m_uiId;
}

void ItemTaskGroup::setId(uint32 id)
{
	m_uiId = id;
}

void ItemTaskGroup::onPause(bool state)
{
}

gcRefPtr<UserCore::ItemTask::BaseItemTask> ItemTaskGroup::newTask(gcRefPtr<ItemHandle> handle)
{
	auto g = gcRefPtr<GroupItemTask>::create(handle, this);

	std::lock_guard<std::recursive_mutex> guard(m_TaskListLock);
	m_vTaskList.push_back(g);

	return g;
}

void ItemTaskGroup::updateEvents(gcRefPtr<UserCore::ItemTask::BaseItemTask> task)
{
	std::lock_guard<std::recursive_mutex> guard(m_TaskListLock);

	uint32 curItem = m_uiActiveItem;

	if (curItem == UINT_MAX && m_uiLastActive != UINT_MAX)
		curItem = m_uiLastActive + 1; //need to add one as it has to do the current item (x<curItem)

	if (curItem != UINT_MAX)
	{
		MCFCore::Misc::ProgressInfo i;
		i.percent = 100;

		for (size_t x = 0; x < curItem; x++)
		{
			i.totalAmmount = m_vWaitingList[x]->getItemInfo()->getId().toInt64();
			task->onMcfProgressEvent(i);
		}
	}
}

uint32 ItemTaskGroup::getPos(gcRefPtr<ItemHandleI> item)
{
	uint32 res = 0;

	std::lock_guard<std::recursive_mutex> guard(m_ListLock);

	for (size_t x=0; x<m_vWaitingList.size(); x++)
	{
		if (m_vWaitingList[x] == item)
		{
			res = x+1;
			break;
		}
	}

	if (m_uiActiveItem != UINT_MAX)
		res -= (m_uiActiveItem+1);
	else if (m_uiLastActive != UINT_MAX)
		res -= m_uiLastActive;

	return res;
}

uint32 ItemTaskGroup::getCount()
{
	uint32 res = 0;

	std::lock_guard<std::recursive_mutex> guard(m_ListLock);
	res = m_vWaitingList.size();

	if (m_uiActiveItem != UINT_MAX)
		res -= (m_uiActiveItem+1);
	else if (m_uiLastActive != UINT_MAX)
		res -= m_uiLastActive;

	return res;
}
