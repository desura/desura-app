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

#ifndef DESURA_ITEMHANDLE_H
#define DESURA_ITEMHANDLE_H
#ifdef _WIN32
#pragma once
#endif

#include "usercore/ItemInfoI.h"
#include "usercore/ItemHandleI.h"
#include "ItemInfo.h"
#include "BaseItemTask.h"
#include "util_thread/BaseThread.h"
#include "usercore/ToolManagerI.h"

#include <memory>

class BaseHandler;

namespace UserCore
{

	class User;

	namespace Thread
	{
		class MCFThreadI;
		class UserServiceI;
	}

	namespace ItemTask
	{
		enum GI_FLAGS
		{
			GI_FLAG_NONE = 0,
			GI_FLAG_EXISTING = 1<<1,
			GI_FLAG_UPDATE = 1<<2,
			GI_FLAG_TEST = 1<<3,
			GI_FLAG_LAUNCH = 1<<4,
		};
	}

	namespace Item
	{

		class ItemThread;
		class ItemHandleEvents;
		class ItemTaskGroup;

		class ItemHandleInternalI
		{
		public:
			virtual void setPausable(bool state = true)=0;
			virtual void setPaused(bool state, bool force)=0;
			virtual void setPaused(bool state)=0;

			virtual void completeStage(bool close = false)=0;
			virtual void resetStage(bool close = false)=0;
			virtual void goToStageGatherInfo(MCFBranch branch, MCFBuild build, UserCore::ItemTask::GI_FLAGS flags)=0;
			virtual void goToStageDownload(MCFBranch branch, MCFBuild build, bool test = false)=0;
			virtual void goToStageDownload(const char* path)=0;
			virtual void goToStageInstallComplex(MCFBranch branch, MCFBuild build, bool launch=false)=0;
			virtual void goToStageInstall(const char* path, MCFBranch branch)=0;
			virtual void goToStageVerify(MCFBranch branch, MCFBuild build, bool files, bool tools, bool hooks)=0;
			virtual void goToStageInstallCheck()=0;
			virtual void goToStageUninstall(bool complete, bool account)=0;
			virtual void goToStageUninstallBranch(MCFBranch branch, MCFBuild build, bool test = false)=0;
			virtual void goToStageUninstallComplexBranch(MCFBranch branch, MCFBuild build, bool complexLaunch = false)=0;
			virtual void goToStageUninstallPatch(MCFBranch branch, MCFBuild build)=0;
			virtual void goToStageUninstallUpdate(const char* path, MCFBuild lastBuild)=0;
			virtual void goToStageLaunch()=0;

			virtual void goToStageDownloadTools(ToolTransactionId ttid, const char* downloadPath, MCFBranch branch, MCFBuild build)=0;
			virtual void goToStageDownloadTools(bool launch, ToolTransactionId ttid = -1)=0;
			virtual void goToStageInstallTools(bool launch)=0;
		};

#ifdef LINK_WITH_GMOCK
		class ItemHandleInternalMock : public ItemHandleInternalI
		{
		public:
			MOCK_METHOD1(setPausable, void(bool state));
			MOCK_METHOD2(setPaused, void(bool state, bool force));
			MOCK_METHOD1(setPaused, void(bool state));

			MOCK_METHOD1(completeStage, void(bool close));
			MOCK_METHOD1(resetStage, void(bool close ));
			MOCK_METHOD3(goToStageGatherInfo, void(MCFBranch branch, MCFBuild build, UserCore::ItemTask::GI_FLAGS flags));
			MOCK_METHOD3(goToStageDownload, void(MCFBranch branch, MCFBuild build, bool test));
			MOCK_METHOD1(goToStageDownload, void(const char* path));
			MOCK_METHOD3(goToStageInstallComplex, void(MCFBranch branch, MCFBuild build, bool launch));
			MOCK_METHOD2(goToStageInstall, void(const char* path, MCFBranch branch));
			MOCK_METHOD5(goToStageVerify, void(MCFBranch branch, MCFBuild build, bool files, bool tools, bool hooks));
			MOCK_METHOD0(goToStageInstallCheck, void());
			MOCK_METHOD2(goToStageUninstall, void(bool complete, bool account));
			MOCK_METHOD3(goToStageUninstallBranch, void(MCFBranch branch, MCFBuild build, bool test));
			MOCK_METHOD3(goToStageUninstallComplexBranch, void(MCFBranch branch, MCFBuild build, bool complexLaunch));
			MOCK_METHOD2(goToStageUninstallPatch, void(MCFBranch branch, MCFBuild build));
			MOCK_METHOD2(goToStageUninstallUpdate, void(const char* path, MCFBuild lastBuild));
			MOCK_METHOD0(goToStageLaunch, void());

			MOCK_METHOD4(goToStageDownloadTools, void(ToolTransactionId ttid, const char* downloadPath, MCFBranch branch, MCFBuild build));
			MOCK_METHOD2(goToStageDownloadTools, void(bool launch, ToolTransactionId ttid));
			MOCK_METHOD1(goToStageInstallTools, void(bool launch));
		};
#endif

		class ItemHandle : public ItemHandleI, public ItemHandleInternalI
		{
		public:
			ItemHandle(ItemInfo* itemInfo, UserCore::UserI* user);
			ItemHandle(std::shared_ptr<UserCore::Item::ItemInfo> &itemInfo, UserCore::UserI* user);
			~ItemHandle();

			void setFactory(Helper::ItemHandleFactoryI* factory) override;

			void addHelper(Helper::ItemHandleHelperI* helper) override;
			void delHelper(Helper::ItemHandleHelperI* helper) override;

			bool cleanComplexMods() override;
			bool verify(bool files, bool tools, bool hooks) override;
			bool update() override;
			bool install(Helper::ItemLaunchHelperI* helper, MCFBranch branch) override;
			bool install(MCFBranch branch, MCFBuild build, bool test = false) override;
			bool installCheck() override;
			bool launch(Helper::ItemLaunchHelperI* helper, bool offline = false, bool ignoreUpdate = false) override;
			bool switchBranch(MCFBranch branch) override;
			bool startUpCheck() override;
			bool uninstall(Helper::ItemUninstallHelperI* helper, bool complete, bool account) override;
			bool isInStage() override;

			ITEM_STAGE getStage() override;
			void cancelCurrentStage() override;
			UserCore::Item::ItemInfoI* getItemInfo() override;

			//void stop(bool block = true) override;

			void setPauseOnError(bool pause = true) override;
			bool shouldPauseOnError() override;
			bool isStopped() override;

			Event<ITEM_STAGE>* getChangeStageEvent() override;
			Event<gcException>* getErrorEvent() override;

			void getStatusStr(LanguageManagerI & pLangMng, char* buffer, uint32 buffsize) override;
			static void getStatusStr_s(UserCore::Item::ItemHandleI* pItemHandle, UserCore::Item::ItemInfoI *pItemInfo, UserCore::Item::ITEM_STAGE nStage, UserCore::Item::ItemTaskGroupI* pTaskGroup, LanguageManagerI & pLangMng, char* buffer, uint32 buffsize);

			ItemTaskGroupI* getTaskGroup() override;
			void force() override;

			bool createDesktopShortcut() override;
			bool createMenuEntry() override;

		#ifdef NIX
			void installLaunchScripts() override;
		#endif

			ItemHandleInternalI* getInternal() override
			{
				return this;
			}

			void setPausable(bool state = true) override;
			void setPaused(bool state, bool force) override;
			void setPaused(bool state) override;

			void completeStage(bool close = false) override;
			void resetStage(bool close = false) override;
			void goToStageGatherInfo(MCFBranch branch, MCFBuild build, UserCore::ItemTask::GI_FLAGS flags) override;
			void goToStageDownload(MCFBranch branch, MCFBuild build, bool test = false) override;
			void goToStageDownload(const char* path) override;
			void goToStageInstallComplex(MCFBranch branch, MCFBuild build, bool launch=false) override;
			void goToStageInstall(const char* path, MCFBranch branch) override;
			void goToStageVerify(MCFBranch branch, MCFBuild build, bool files, bool tools, bool hooks) override;
			void goToStageInstallCheck() override;
			void goToStageUninstall(bool complete, bool account) override;
			void goToStageUninstallBranch(MCFBranch branch, MCFBuild build, bool test = false) override;
			void goToStageUninstallComplexBranch(MCFBranch branch, MCFBuild build, bool complexLaunch = false) override;
			void goToStageUninstallPatch(MCFBranch branch, MCFBuild build) override;
			void goToStageUninstallUpdate(const char* path, MCFBuild lastBuild) override;
			void goToStageLaunch() override;

			void goToStageDownloadTools(ToolTransactionId ttid, const char* downloadPath, MCFBranch branch, MCFBuild build) override;
			void goToStageDownloadTools(bool launch, ToolTransactionId ttid = -1) override;
			void goToStageInstallTools(bool launch) override;
			
			bool install(MCFBranch branch, MCFBuild build, UserCore::ItemTask::GI_FLAGS flags);

			uint64 getHash(){return getItemInfo()->getId().toInt64();}
			const char* getName(){return getItemInfo()->getName();}	

			UserCore::Item::ItemInfo* getItemInfoNorm();

			UserCore::UserI* getUserCore();

			bool getLock(void* obj);
			bool isLocked();
			bool hasLock(void* obj);
			void releaseLock(void* obj);

			void verifyOveride();
			void uninstallOveride();

			ItemHandleEvents* getEventHandler();
			bool setTaskGroup(ItemTaskGroup* group, bool force = false);

			//used to get around the is in stage check
			bool installPrivate(MCFBranch branch, MCFBuild build, UserCore::ItemTask::GI_FLAGS flags);

			bool isCurrentlyInstalledGameOrMod();

		protected:
			Event<ITEM_STAGE> onChangeStageEvent;
			Event<gcException> onErrorEvent;

			void startGatherInfo();

			void setStage(ITEM_STAGE stage);
			void registerTask(UserCore::ItemTask::BaseItemTask* task);

			void stopThread();

			void preLaunchCheck();
	
			void doLaunch(Helper::ItemLaunchHelperI* helper);
		#ifdef NIX
			void doLaunch(bool useXdgOpen, const char* globalExe, const char* globalArgs);
		#endif

			virtual bool launchForReal(Helper::ItemLaunchHelperI* helper, bool offline = false);

			void onTaskStart(ITEM_STAGE &stage);
			void onTaskComplete(ITEM_STAGE &stage);

			bool getComplexLock();
			void releaseComplexLock();

			bool preDownloadCheck(MCFBranch branch, bool test);
			bool checkPaused();

		private:
			bool m_bPauseOnError = false;
			bool m_bStopped = false;

			bool m_bLock = false;
			void* m_pLockObject = nullptr;

			std::vector<Helper::ItemHandleHelperI*> m_vHelperList;

			uint32 m_uiHelperId = 0;
			ITEM_STAGE m_uiStage = ITEM_STAGE::STAGE_NONE;

			std::recursive_mutex m_ThreadMutex;
			UserCore::Item::ItemThread *m_pThread = nullptr;
			std::shared_ptr<UserCore::Item::ItemInfo> m_pItemInfo;
			UserCore::UserI* m_pUserCore = nullptr;

			Helper::ItemHandleFactoryI* m_pFactory = nullptr;
			ItemHandleEvents* m_pEventHandler = nullptr;
			ItemTaskGroup* m_pGroup = nullptr;
		};
	}
}










#endif //DESURA_ITEMHANDLE_H
