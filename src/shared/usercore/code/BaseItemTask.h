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

#ifndef DESURA_BASEITEMTASK_H
#define DESURA_BASEITEMTASK_H
#ifdef _WIN32
#pragma once
#endif

#include "usercore/ItemHelpersI.h"
#include "usercore/ItemHandleI.h"
#include "mcfcore/MCFMain.h"
#include "mcfcore/MCFI.h"

namespace WebCore
{
	class WebCoreI;
}

namespace UserCore
{
	class UserI;

	namespace Item
	{
		class ItemHandle;
		class ItemInfo;
	}

	namespace ItemTask
	{
		class BaseItemTask : public gcRefBase
		{
		public:
			BaseItemTask(UserCore::Item::ITEM_STAGE type, const char* name, gcRefPtr<UserCore::Item::ItemHandleI> &handle, MCFBranch branch = MCFBranch(), MCFBuild build = MCFBuild());
			virtual ~BaseItemTask();

			void setWebCore(gcRefPtr<WebCore::WebCoreI> &wc);
			void setUserCore(gcRefPtr<UserCore::UserI> &uc);

			virtual void onStop();
			virtual void onPause();
			virtual void onUnpause();
			virtual void doTask();

			virtual void cancel();

			Event<uint32> onCompleteEvent;
			Event<uint32> onProgUpdateEvent;
			Event<gcException> onErrorEvent;
			Event<WCSpecialInfo> onNeedWCEvent;

			//download mcf
			Event<UserCore::Misc::GuiDownloadProvider> onNewProviderEvent;
			Event<UserCore::Misc::VerifyComplete> onVerifyCompleteEvent;
			Event<MCFCore::Misc::ProgressInfo> onMcfProgressEvent;
			Event<gcString> onCompleteStrEvent;

			const char* getTaskName();
			UserCore::Item::ITEM_STAGE getTaskType();

			gcRefPtr<UserCore::Item::ItemHandleI> getItemHandle();

		protected:
			virtual void doRun()=0;

			gcRefPtr<UserCore::Item::ItemInfoI> getItemInfo();
			gcRefPtr<UserCore::Item::ItemInfoI> getParentItemInfo();

			DesuraId getItemId();

			gcRefPtr<WebCore::WebCoreI> getWebCore();
			gcRefPtr<UserCore::UserI> getUserCore();

			MCFBuild getMcfBuild();
			MCFBranch getMcfBranch();

			bool isStopped();
			bool isPaused();

			McfHandle m_hMCFile;

			MCFBranch m_uiMcfBranch;
			MCFBuild m_uiMcfBuild;

		private:
			volatile bool m_bIsStopped = false;
			volatile bool m_bIsPaused = false;

			gcRefPtr<UserCore::Item::ItemHandleI> m_pHandle;
			gcRefPtr<WebCore::WebCoreI> m_pWebCore;
			gcRefPtr<UserCore::UserI> m_pUserCore;

			const UserCore::Item::ITEM_STAGE m_uiType;
			const gcString m_szName;

			gc_IMPLEMENT_REFCOUNTING(BaseItemTask)
		};
	}
}


#endif //DESURA_BASEITEMTASK_H
