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
		class BaseItemTask
		{
		public:
			BaseItemTask(UserCore::Item::ITEM_STAGE type, const char* name, UserCore::Item::ItemHandleI* handle, MCFBranch branch = MCFBranch(), MCFBuild build = MCFBuild());
			virtual ~BaseItemTask();

			void setWebCore(WebCore::WebCoreI *wc);
			void setUserCore(UserCore::UserI *uc);

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

			UserCore::Item::ItemHandleI* getItemHandle();

		protected:
			virtual void doRun()=0;

			UserCore::Item::ItemInfoI* getItemInfo();
			UserCore::Item::ItemInfoI* getParentItemInfo();

			DesuraId getItemId();

			WebCore::WebCoreI* getWebCore();
			UserCore::UserI* getUserCore();

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

			UserCore::Item::ItemHandleI* m_pHandle = nullptr;
			WebCore::WebCoreI* m_pWebCore = nullptr;
			UserCore::UserI* m_pUserCore = nullptr;

			UserCore::Item::ITEM_STAGE m_uiType;
			gcString m_szName;
		};
	}
}


#endif //DESURA_BASEITEMTASK_H
