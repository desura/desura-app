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

#ifndef DESURA_ITEMTASKGROUP_H
#define DESURA_ITEMTASKGROUP_H
#ifdef _WIN32
#pragma once
#endif

#include "util_thread/BaseThread.h"
#include "usercore/ItemHandleI.h"
#include "usercore/ItemHelpersI.h"
#include "BaseItemTask.h"



namespace UserCore
{
	class ItemManager;

	namespace Item
	{
		class ItemHandle;

		class ItemTaskGroup : public UserCore::Item::ItemTaskGroupI, public UserCore::Item::Helper::ItemHandleHelperI
		{
		public:
			ItemTaskGroup(gcRefPtr<UserCore::ItemManager> manager, ACTION action, uint8 activeCount = 1);
			~ItemTaskGroup();

			bool addItem(gcRefPtr<UserCore::Item::ItemHandleI> item);
			bool addItem(gcRefPtr<UserCore::Item::ItemInfoI> item);

			bool removeItem(gcRefPtr<UserCore::Item::ItemHandleI> item);

			gcRefPtr<UserCore::Item::ItemHandleI> getActiveItem();

			void start();
			void pause();
			void unpause();
			void cancel();
			void finalize();

			virtual UserCore::Item::ItemTaskGroupI::ACTION getAction();
			virtual void getItemList(std::vector<gcRefPtr<UserCore::Item::ItemHandleI>> &list);
			virtual void cancelAll();

			void startAction(gcRefPtr<ItemHandle> item);
			gcRefPtr<UserCore::ItemTask::BaseItemTask> newTask(gcRefPtr<ItemHandle> handle);

			uint32 getPos(gcRefPtr<UserCore::Item::ItemHandleI> item) override;
			uint32 getCount() override;

			template <typename F>
			void sort(F f)
			{
				if (m_bStarted)
					return;

				std::lock_guard<std::recursive_mutex> guard(m_ListLock);
				std::sort(m_vWaitingList.begin(), m_vWaitingList.end(), f);
			}

			gc_IMPLEMENT_REFCOUNTING(ItemTaskGroup);

		protected:
			virtual void onComplete(uint32 status);
			virtual void onComplete(gcString& string);

			virtual void onMcfProgress(MCFCore::Misc::ProgressInfo& info);
			virtual void onProgressUpdate(uint32 progress);

			virtual void onError(gcException e);
			virtual void onNeedWildCard(WCSpecialInfo& info);

			virtual void onDownloadProvider(UserCore::Misc::GuiDownloadProvider &provider);
			virtual void onVerifyComplete(UserCore::Misc::VerifyComplete& info);

			virtual uint32 getId();
			virtual void setId(uint32 id);

			virtual void onPause(bool state);

			void finish();
			void nextItem();

			void updateEvents(gcRefPtr<UserCore::ItemTask::BaseItemTask> task);

			class GroupItemTask :  public UserCore::ItemTask::BaseItemTask
			{
			public:
				GroupItemTask(gcRefPtr<ItemHandleI> handle, gcRefPtr<ItemTaskGroup> group)
					: BaseItemTask(UserCore::Item::ITEM_STAGE::STAGE_WAIT, "TaskGroup", handle)
					, m_pGroup(group)
				{
				}

				virtual void doRun()
				{
					m_pGroup->updateEvents(this);
					m_WaitCon.wait();
				}

				virtual void onStop()
				{
					m_WaitCon.notify();
				}

				virtual void cancel()
				{
					m_WaitCon.notify();
				}

				::Thread::WaitCondition m_WaitCon;
				gcRefPtr<ItemTaskGroup> m_pGroup;
			};

		private:
			bool m_bStarted = false;
			bool m_bPaused = false;
			bool m_bFinal = false;

			uint8 m_iActiveCount;
			uint32 m_uiId = 0;

			uint32 m_uiLastActive = -1;
			uint32 m_uiActiveItem = -1;

			ACTION m_Action;

			std::recursive_mutex m_ListLock;
			std::vector<gcRefPtr<UserCore::Item::ItemHandleI>> m_vWaitingList;

			std::recursive_mutex m_TaskListLock;
			std::vector<gcRefPtr<UserCore::ItemTask::BaseItemTask>> m_vTaskList;

			gcRefPtr<UserCore::ItemManager> m_pItemManager;
		};
	}
}

#endif //DESURA_ITEMTASKGROUP_H
