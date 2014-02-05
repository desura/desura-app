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
	};
}

namespace Item
{

class ItemThread;
class ItemHandleEvents;
class ItemTaskGroup;

class ItemHandle : public ItemHandleI
{
public:
	ItemHandle(ItemInfo* itemInfo, UserCore::User* user);
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

	void setPausable(bool state = true);
	void setPaused(bool state, bool force);
	void setPaused(bool state);

	void completeStage(bool close = false);
	void resetStage(bool close = false);
	void goToStageGatherInfo(MCFBranch branch, MCFBuild build, UserCore::ItemTask::GI_FLAGS flags);
	void goToStageDownload(MCFBranch branch, MCFBuild build, bool test = false);
	void goToStageDownload(const char* path);
	void goToStageInstallComplex(MCFBranch branch, MCFBuild build, bool launch=false);
	void goToStageInstall(const char* path, MCFBranch branch);
	void goToStageVerify(MCFBranch branch, MCFBuild build, bool files, bool tools, bool hooks);
	void goToStageInstallCheck();
	void goToStageUninstall(bool complete, bool account);
	void goToStageUninstallBranch(MCFBranch branch, MCFBuild build, bool test = false);
	void goToStageUninstallComplexBranch(MCFBranch branch, MCFBuild build, bool complexLaunch = false);
	void goToStageUninstallPatch(MCFBranch branch, MCFBuild build);
	void goToStageUninstallUpdate(const char* path, MCFBuild lastBuild);
	void goToStageLaunch();

	void goToStageDownloadTools(uint32 ttid, const char* downloadPath, MCFBranch branch, MCFBuild build);
	void goToStageDownloadTools(bool launch);
	void goToStageInstallTools(bool launch);

	bool install(MCFBranch branch, MCFBuild build, UserCore::ItemTask::GI_FLAGS flags);

	uint64 getHash(){return getItemInfo()->getId().toInt64();}
	const char* getName(){return getItemInfo()->getName();}	

	UserCore::Item::ItemInfo* getItemInfoNorm();

	UserCore::User* getUserCore();

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

	bool launchForReal(Helper::ItemLaunchHelperI* helper, bool offline = false);

	void onTaskStart(ITEM_STAGE &stage);
	void onTaskComplete(ITEM_STAGE &stage);

	bool getComplexLock();
	void releaseComplexLock();

	bool preDownloadCheck(MCFBranch branch, bool test);
	bool checkPaused();

private:
	bool m_bPauseOnError;
	bool m_bStopped;

	bool m_bLock;
	void* m_pLockObject;

	std::vector<Helper::ItemHandleHelperI*> m_vHelperList;

	uint32 m_uiHelperId;
	ITEM_STAGE m_uiStage;

	std::recursive_mutex m_ThreadMutex;
	UserCore::Item::ItemThread *m_pThread;
	UserCore::Item::ItemInfo* m_pItemInfo;
	UserCore::User* m_pUserCore;

	Helper::ItemHandleFactoryI* m_pFactory;
	ItemHandleEvents* m_pEventHandler;
	ItemTaskGroup* m_pGroup;
};


}
}










#endif //DESURA_ITEMHANDLE_H
