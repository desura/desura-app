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


#ifndef DESURA_ITEMHANDLERI_H
#define DESURA_ITEMHANDLERI_H
#ifdef _WIN32
#pragma once
#endif

#include "usercore/ItemHelpersI.h"

class LanguageManagerI;

namespace UserCore
{
	namespace Item
	{
		class ItemHandleI;
		class ItemHandleInternalI;

		class ItemTaskGroupI : public gcRefBase
		{
		public:
			enum ACTION
			{
				A_VERIFY,
				A_UNINSTALL,
			};

			virtual ACTION getAction()=0;
			virtual void getItemList(std::vector<gcRefPtr<UserCore::Item::ItemHandleI>> &list)=0;
			virtual void cancelAll()=0;

			virtual uint32 getPos(gcRefPtr<UserCore::Item::ItemHandleI> item) = 0;
			virtual uint32 getCount()=0;
		};


		enum class ITEM_STAGE
		{
			STAGE_NONE,
			STAGE_CLOSE,
			STAGE_DOWNLOAD,
			STAGE_GATHERINFO,
			STAGE_INSTALL,
			STAGE_INSTALL_CHECK,
			STAGE_INSTALL_COMPLEX,
			STAGE_UNINSTALL,
			STAGE_UNINSTALL_BRANCH,
			STAGE_UNINSTALL_COMPLEX,
			STAGE_UNINSTALL_PATCH,
			STAGE_UNINSTALL_UPDATE,
			STAGE_VERIFY,
			STAGE_LAUNCH,
			STAGE_DOWNLOADTOOL,
			STAGE_INSTALLTOOL,
			STAGE_VALIDATE,
			STAGE_WAIT,
			STAGE_COMBO_DL_IN,
		};

		class ItemHandleI : public gcRefBase
		{
		public:
			virtual void setFactory(gcRefPtr<Helper::ItemHandleFactoryI> factory) = 0;

			virtual void addHelper(gcRefPtr<Helper::ItemHandleHelperI> helper) = 0;
			virtual void delHelper(gcRefPtr<Helper::ItemHandleHelperI> helper) = 0;

			virtual bool cleanComplexMods()=0;
			virtual bool verify(bool files, bool tools, bool hooks)=0;
			virtual bool update()=0;
			virtual bool install(gcRefPtr<Helper::ItemLaunchHelperI> helper, MCFBranch branch) = 0;
			virtual bool install(MCFBranch branch, MCFBuild build, bool test = false)=0;
			virtual bool installCheck()=0;
			virtual bool launch(gcRefPtr<Helper::ItemLaunchHelperI> helper, bool offline = false, bool ignoreUpdate = false) = 0;
			virtual bool switchBranch(MCFBranch branch)=0;
			virtual bool startUpCheck()=0;
			virtual bool uninstall(gcRefPtr<Helper::ItemUninstallHelperI> helper, bool complete, bool account) = 0;

			virtual void setPaused(bool paused = true)=0;
			virtual void setPauseOnError(bool pause = true)=0;
			virtual bool shouldPauseOnError()=0;
			virtual bool isStopped()=0;
			virtual bool isInStage()=0;

			//! Gets the current stage
			//!
			virtual ITEM_STAGE getStage()=0;

			//! Cancels the current stage
			//!
			virtual void cancelCurrentStage()=0;

			virtual const gcRefPtr<UserCore::Item::ItemInfoI>& getItemInfo() = 0;

			virtual Event<ITEM_STAGE>& getChangeStageEvent()=0;
			virtual Event<gcException>& getErrorEvent()=0;

			//! Gets the item status string
			//!
			//! @param pLangMng Language manager for translation
			//! @param buffer Buffer to save status into
			//! @param buffsize Max size of buffer
			//!
			virtual void getStatusStr(LanguageManagerI & pLangMng, char* buffer, uint32 buffsize)=0;
			virtual gcRefPtr<ItemTaskGroupI> getTaskGroup() = 0;

			//! If this item is in a task group it will force start it
			//!
			virtual void force()=0;

			virtual bool createDesktopShortcut()=0;
			virtual bool createMenuEntry()=0;

		#ifdef NIX
			//! Creates the launch scripts
			virtual void installLaunchScripts()=0;
		#endif

			virtual gcRefPtr<ItemHandleInternalI> getInternal() = 0;
		};
	}
}


#ifdef LINK_WITH_GMOCK
#include "managers/Managers.h"

namespace UserCore
{
	namespace Item
	{
		class ItemHandleMock : public ItemHandleI
		{
		public:
			MOCK_METHOD1(setFactory, void(gcRefPtr<Helper::ItemHandleFactoryI> factory));
			MOCK_METHOD1(addHelper, void(gcRefPtr<Helper::ItemHandleHelperI> helper));
			MOCK_METHOD1(delHelper, void(gcRefPtr<Helper::ItemHandleHelperI> helper));
			MOCK_METHOD0(cleanComplexMods, bool());
			MOCK_METHOD3(verify, bool(bool files, bool tools, bool hooks));
			MOCK_METHOD0(update, bool());
			MOCK_METHOD2(install, bool(gcRefPtr<Helper::ItemLaunchHelperI> helper, MCFBranch branch));
			MOCK_METHOD3(install, bool(MCFBranch branch, MCFBuild build, bool test));
			MOCK_METHOD0(installCheck, bool());
			MOCK_METHOD3(launch, bool(gcRefPtr<Helper::ItemLaunchHelperI> helper, bool offline, bool ignoreUpdate));
			MOCK_METHOD1(switchBranch, bool(MCFBranch branch));
			MOCK_METHOD0(startUpCheck, bool());
			MOCK_METHOD3(uninstall, bool(gcRefPtr<Helper::ItemUninstallHelperI> helper, bool complete, bool account));
			MOCK_METHOD1(setPaused, void(bool paused));
			MOCK_METHOD1(setPauseOnError, void(bool pause));
			MOCK_METHOD0(shouldPauseOnError, bool());
			MOCK_METHOD0(isStopped, bool());
			MOCK_METHOD0(isInStage, bool());
			MOCK_METHOD0(getStage, ITEM_STAGE());
			MOCK_METHOD0(cancelCurrentStage, void());
			MOCK_METHOD0(getItemInfo, const gcRefPtr<UserCore::Item::ItemInfoI>&());
			MOCK_METHOD0(getChangeStageEvent, Event<ITEM_STAGE>&());
			MOCK_METHOD0(getErrorEvent, Event<gcException>&());
			MOCK_METHOD3(getStatusStr, void(LanguageManagerI & pLangMng, char* buffer, uint32 buffsize));
			MOCK_METHOD0(getTaskGroup, gcRefPtr<ItemTaskGroupI>());
			MOCK_METHOD0(force, void());
			MOCK_METHOD0(createDesktopShortcut, bool());
			MOCK_METHOD0(createMenuEntry, bool());
		#ifdef NIX
			MOCK_METHOD0(installLaunchScripts, void());
		#endif
			MOCK_METHOD0(getInternal, gcRefPtr<ItemHandleInternalI>());
		};
	}
}

#endif

#endif //DESURA_ITEMHANDLERI_H
