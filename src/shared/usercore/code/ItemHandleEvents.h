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

#ifndef DESURA_ITEMHANDLEEVENTS_H
#define DESURA_ITEMHANDLEEVENTS_H
#ifdef _WIN32
#pragma once
#endif

#include "ItemHandle.h"
#include "usercore/ItemHelpersI.h"

namespace UserCore
{
	namespace Item
	{

		class EventItemI;

		class ItemHandleEvents : public gcRefBase
		{
		public:
			ItemHandleEvents(std::recursive_mutex &helperLock, std::vector<gcRefPtr<Helper::ItemHandleHelperI>> &vHelperList);
			~ItemHandleEvents();

			void registerTask(gcRefPtr<UserCore::ItemTask::BaseItemTask> &task);
			void deregisterTask(gcRefPtr<UserCore::ItemTask::BaseItemTask> &task);

			void postAll(gcRefPtr<Helper::ItemHandleHelperI> &helper);

			void reset();
			void onPause(bool state);

		protected:
			void onComplete(uint32& status);
			void onProgressUpdate(uint32& progress);
			void onError(gcException& e);
			void onNeedWildCard(WCSpecialInfo& info);

			void onMcfProgress(MCFCore::Misc::ProgressInfo& info);
			void onComplete(gcString& str);

			void onDownloadProvider(UserCore::Misc::GuiDownloadProvider& provider);
			void onVerifyComplete(UserCore::Misc::VerifyComplete& info);

		private:
			MCFCore::Misc::ProgressInfo m_LastProg;

			std::vector<gcRefPtr<EventItemI>> m_EventHistory;

			std::recursive_mutex &m_HelperLock;
			std::vector<gcRefPtr<Helper::ItemHandleHelperI>> &m_vHelperList;

			gc_IMPLEMENT_REFCOUNTING(ItemHandleEvents)
		};
	}
}

#endif //DESURA_ITEMHANDLEEVENTS_H
