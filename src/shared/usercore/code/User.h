/*
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
////////////// Copyright 2010 Desura Pty Ltd. All rights reserved.  /////////////
//
//   Project     : UserCore::Usercore
//   File        : UserCore::User.h
//   Description :
//      [Write the purpose of UserCore::User.h.]
//
//   Created On: 9/23/2008 5:00:03 PM
//   Created By: Mark Chandler <mailto:mark@moddb.com>
////////////////////////////////////////////////////////////////////////////

#ifndef DESURA_USER_H
#define DESURA_USER_H
#ifdef _WIN32
#pragma once
#endif

#include "Common.h"

#include "usercore/UserCoreI.h"

#include "ItemManager.h"
#include "ToolManager.h"
#include "CDKeyManager.h"
#include "CIPManager.h"

#include "BaseManager.h"
#include "webcore/WebCoreI.h"
#include "managers/WildcardManager.h"
#include "util_thread/BaseThread.h"
#include "util_thread/ThreadPool.h"

#include "Event.h"

#include "ItemInfo.h"
#include "usercore/NewsItem.h"
#include "UserThreadManager.h"
#include "UploadManager.h"
#include "UserTasks.h"
#include "UserIPCPipeClient.h"

#include "service_pipe/IPCServiceMain.h"
#include "GameExplorerManager.h"

CEXPORT const char* GetUserCoreVersion();

class UpdateThread;
class ThreadPool;


namespace XML
{
	class gcXMLElement;
}

namespace Thread
{
	class WaitCondition;
}

namespace UserCore
{
	class MCFManagerI;
	class ItemManager;
	class ToolManager;
	class GameExplorerManager;
	class BDManager;
	class MCFManager;


	namespace Thread
	{
		class UserThreadI;
	}

	namespace Misc
	{
		//! Update available struct
		typedef struct
		{
			bool alert;
			uint32 build;
		} update_s;
	}

	class UserInternalI
	{
	public:
	#ifdef WIN32
		virtual HWND getMainWindowHandle()=0;
	#endif

		//! Downloads an image for an item
		//!
		//! @param itemInfo Item for which the image belongs
		//! @param image Which image to download
		//!
		virtual void downloadImage(UserCore::Item::ItemInfo* itemInfo, uint8 image)=0;

		//! Removes or adds an item to a desura account
		//!
		//! @param item Item for which to act appon
		//! @param action Add or remove it
		//!
		virtual void changeAccount(DesuraId id, uint8 action)=0;


		virtual MCFManagerI* getMCFManager()=0;
	};

#ifdef LINK_WITH_GMOCK
	class UserInternalMock : public UserInternalI
	{
	public:
	#ifdef WIN32
		MOCK_METHOD0(getMainWindowHandle, HWND());
	#endif

		MOCK_METHOD2(downloadImage, void(UserCore::Item::ItemInfo*, uint8));
		MOCK_METHOD2(changeAccount, void(DesuraId, uint8));
		MOCK_METHOD0(getMCFManager, MCFManagerI*());
	};
#endif

	class User : public UserI, public UserInternalI
	{
	public:
		User();
		~User();

		UserInternalI* getInternal() override
		{
			return this;
		}

		void init(const char* appDataPath, const char* szProviderUrl) override;
		void init(const char* appDataPath) override;
		const char* getAppDataPath() override;
		const char* getMcfCachePath() override;

		void lockDelete() override;
		void unlockDelete() override;

		///////////////////////////////////////////////////////////////////////////////
		// Login
		///////////////////////////////////////////////////////////////////////////////

		void logIn(const char* user, const char* password) override;
		void logInTool(const char* user, const char* pass) override;
		void logOut(bool delAutoLogin = false, bool reInit = true) override;
		void saveLoginInfo() override;
		void logInCleanUp() override;

		///////////////////////////////////////////////////////////////////////////////
		// Misc
		///////////////////////////////////////////////////////////////////////////////

		bool isAdmin() override;
		bool isDelayLoading() override;

		void appNeedUpdate(uint32 appid = 0, uint32 appver = 0, bool bForced = false) override;
		void restartPipe() override;
		void forceUpdatePoll() override;
		void forceQATestingUpdate() override;
		void setQATesting(bool bEnable = true) override;

		///////////////////////////////////////////////////////////////////////////////
		// Getters
		///////////////////////////////////////////////////////////////////////////////

		uint32 getUserId() override;
		const char* getAvatar() override;
		const char* getProfileUrl() override;
		const char* getProfileEditUrl() override;
		const char* getUserNameId() override;
		const char* getUserName() override;

		uint32 getPmCount() override;
		uint32 getUpCount() override;
		uint32 getCartCount() override;
		uint32 getThreadCount() override;

		const char* getCVarValue(const char* cvarName) override;
		::Thread::ThreadPool* getThreadPool() override;
		IPC::ServiceMainI* getServiceMain() override;
		WebCore::WebCoreI* getWebCore() override;
		UserCore::UserThreadManagerI* getThreadManager() override;
		UserCore::UploadManagerI* getUploadManager() override;
		UserCore::ItemManagerI* getItemManager() override;
		UserCore::ToolManagerI* getToolManager() override;
		UserCore::GameExplorerManagerI* getGameExplorerManager() override;
		UserCore::CDKeyManagerI* getCDKeyManager() override;
		UserCore::CIPManagerI* getCIPManager() override;

		///////////////////////////////////////////////////////////////////////////////
		// Events
		///////////////////////////////////////////////////////////////////////////////

		Event<uint32>* getItemsAddedEvent() override;
		Event<UserCore::Misc::UpdateInfo>* getAppUpdateEvent() override;
		Event<UserCore::Misc::UpdateInfo>* getAppUpdateCompleteEvent() override;
		Event<uint32>* getAppUpdateProgEvent() override;
		Event<UserCore::Misc::CVar_s>* getNeedCvarEvent() override;
		EventC<gcString>* getNewAvatarEvent() override;
		Event<WCSpecialInfo>* getNeedWildCardEvent() override;
		Event<std::vector<UserCore::Misc::NewsItem*> >* getNewsUpdateEvent() override;
		Event<std::vector<UserCore::Misc::NewsItem*> >* getGiftUpdateEvent() override;
		Event<std::vector<UserCore::Item::ItemUpdateInfo*> >* getItemUpdateEvent() override;
		EventV* getUserUpdateEvent() override;
		EventV* getPipeDisconnectEvent() override;
		Event<std::tuple<gcOptional<bool>, gcOptional<bool>, gcOptional<bool>>>* getForcedUpdatePollEvent() override;
		EventV* getLoginItemsLoadedEvent() override;
		Event<std::pair<bool, char> >* getLowSpaceEvent() override;

		void setCounts(uint32 msgs, uint32 updates, uint32 threads, uint32 cart) override;
	#ifdef WIN32
		void setMainWindowHandle(HWND handle) override;
	#endif

		void updateUninstallInfo() override;
		void updateUninstallInfo(DesuraId id, uint64 installSize) override;
		void removeUninstallInfo(DesuraId id) override;
		void updateRegKey(const char* key, const char* value) override;
		void updateBinaryRegKey(const char* key, const char* value, size_t size) override;

		void runInstallScript(const char* file, const char* installPath, const char* function) override;

		void setAvatarUrl(const char* szAvatarUrl) override;

		bool isAltProvider();

		//! Parses news xml
		//!
		//! @param newsNode News xml
		//!
		void parseNews(const XML::gcXMLElement &newsNode);

		//! Parses gifts xml
		//!
		//! @param giftNode Gifts xml
		//!
		void parseGifts(const XML::gcXMLElement &giftNode);

		//! Set user avatar
		//!
		//! @param path Avatar path
		//!
		void setAvatarPath(const char* path);

	
		//! Downloads an image for an item
		//!
		//! @param itemInfo Item for which the image belongs
		//! @param image Which image to download
		//!
		void downloadImage(UserCore::Item::ItemInfo* itemInfo, uint8 image) override;

		//! Removes or adds an item to a desura account
		//!
		//! @param item Item for which to act appon
		//! @param action Add or remove it
		//!
		void changeAccount(DesuraId id, uint8 action) override;

		//! Checks for saved login info and returns it if found
		//!
		//! @param internId Item id
		//! @param pWildCard Wildcard manager
		//!
		static void getLoginInfo(char** userhash, char** passhash);

	#ifdef WIN32
		HWND getMainWindowHandle() override;
	#endif

		MCFManagerI* getMCFManager() override;

		//! Start the pipe to the desura service
		//!
		void initPipe();


		//! Should this platform be filtered out.
		//! 
		//! @param platform Platform node form xml
		//! @param type Type of item applying the filter for
		//! @return true to filter, false to inclue
		//!
		bool platformFilter(const XML::gcXMLElement &platform, PlatformType type);

		BDManager* getBDManager();

	

	protected:
		Event<uint32> onItemsAddedEvent;
		Event<UserCore::Misc::UpdateInfo> onAppUpdateEvent;
		Event<UserCore::Misc::UpdateInfo> onAppUpdateCompleteEvent;
		Event<uint32> onAppUpdateProgEvent;
		EventV onUserUpdateEvent;
		Event<UserCore::Misc::CVar_s> onNeedCvarEvent;
		EventC<gcString> onNewAvatarEvent;
		Event<WCSpecialInfo> onNeedWildCardEvent;
		Event<std::vector<UserCore::Misc::NewsItem*> > onNewsUpdateEvent;
		Event<std::vector<UserCore::Misc::NewsItem*> > onGiftUpdateEvent;
		Event<std::vector<UserCore::Item::ItemUpdateInfo*> > onItemUpdateEvent;
		EventV onPipeDisconnect;
		Event<std::tuple<gcOptional<bool>, gcOptional<bool>, gcOptional<bool>>> onForcePollEvent;
		EventV onLoginItemsLoadedEvent;
		Event<std::pair<bool, char> > onLowSpaceEvent;

		//! Update has finished downloading
		//! 
		//! @param info Update info
		//!
		void onUpdateComplete(UserCore::Misc::update_s& info);

		//! Update has started downloading
		//! 
		//! @param info Update info
		//!
		void onUpdateStart(UserCore::Misc::update_s& info);

		//! Resolve Wildcards
		//!
		//! @param info Wildcard Info
		//!
		void onNeedWildCardCB(WCSpecialInfo& info);

		void parseNewsAndGifts(const XML::gcXMLElement &xmlNode, const char* szChildName, Event<std::vector<UserCore::Misc::NewsItem*> > &onEvent);

		void testMcfCache();

	private:
		void doLogIn(const char* user, const char* pass, bool bTestOnly);

		void init();
		void cleanUp();
		void onLoginItemsLoaded();
	
		gcFixedString<255> m_szMcfCachePath;
		gcFixedString<255> m_szAppDataPath;

		gcFixedString<255> m_szUserName;
		gcFixedString<255> m_szUserNameId;

		gcFixedString<255> m_szAvatar;
		gcFixedString<255> m_szAvatarUrl;

		gcFixedString<255> m_szProfileUrl;
		gcFixedString<255> m_szProfileEditUrl;

		bool m_bDelayLoading;
		bool m_bAdmin;
		bool m_bDownloadingUpdate;

		uint32 m_iCartItems;
		uint32 m_iUserId;
		uint32 m_iUpdates;
		uint32 m_iPms;
		uint32 m_iThreads;

		uint32 m_iSelectedIndex;
		uint32 m_uiLastUpdateBuild;
		uint32 m_uiLastUpdateVer;

		UserIPCPipeClient* m_pPipeClient;

		::Thread::ThreadPool* m_pThreadPool;

		WebCore::WebCoreI* m_pWebCore;
		UserCore::UserThreadManager* m_pThreadManager;
		UserCore::UploadManager* m_pUploadManager;
		UserCore::Thread::UserThreadI* m_pUThread;
		UserCore::ItemManager* m_pItemManager;
		UserCore::ToolManager* m_pToolManager;
		UserCore::GameExplorerManager* m_pGameExplorerManager;

		UserCore::CDKeyManager* m_pCDKeyManager;
		UserCore::BDManager* m_pBannerDownloadManager;
		UserCore::CIPManager* m_pCIPManager;

		volatile bool m_bLocked;
		::Thread::WaitCondition *m_pWaitCond;

		UserCore::MCFManager* m_pMcfManager = nullptr;

	#ifdef WIN32
		HWND m_WinHandle;
	#endif

		bool m_bAltProvider;

		friend class ItemManager;
	};


	inline bool User::isAdmin()
	{
		return m_bAdmin;
	}

	inline bool User::isDelayLoading()
	{
		return m_bDelayLoading;
	}

	inline uint32 User::getUserId()
	{
		return m_iUserId;
	}


	inline const char* User::getAvatar()
	{
		return m_szAvatar.c_str();
	}

	inline const char* User::getProfileUrl()
	{
		return m_szProfileUrl.c_str();
	}

	inline const char* User::getProfileEditUrl()
	{
		return m_szProfileEditUrl.c_str();
	}

	inline const char* User::getUserNameId()
	{
		return m_szUserNameId.c_str();
	}

	inline const char* User::getUserName()
	{
		return m_szUserName.c_str();
	}

	inline uint32 User::getPmCount()
	{
		return m_iPms;
	}

	inline uint32 User::getUpCount()
	{
		return m_iUpdates;
	}

	inline uint32 User::getCartCount()
	{
		return m_iCartItems;
	}

	inline uint32 User::getThreadCount()
	{
		return m_iThreads;
	}

	inline ::Thread::ThreadPool* User::getThreadPool()
	{
		return m_pThreadPool;
	}

	inline IPC::ServiceMainI* User::getServiceMain()
	{
		if (!m_pPipeClient)
			return nullptr;

		return m_pPipeClient->getServiceMain();
	}

	inline WebCore::WebCoreI* User::getWebCore()
	{
		return m_pWebCore;
	}

	inline UserCore::UserThreadManagerI* User::getThreadManager()
	{
		return m_pThreadManager;
	}

	inline UserCore::UploadManagerI* User::getUploadManager()
	{
		return m_pUploadManager;
	}

	inline UserCore::ItemManagerI* User::getItemManager()
	{
		return m_pItemManager;
	}

	inline UserCore::ToolManagerI* User::getToolManager()
	{
		return m_pToolManager;
	}

	inline UserCore::GameExplorerManagerI* User::getGameExplorerManager()
	{
		return m_pGameExplorerManager;
	}

	inline UserCore::CDKeyManagerI* User::getCDKeyManager()
	{
		return m_pCDKeyManager;
	}

	inline UserCore::CIPManagerI* User::getCIPManager()
	{
		return m_pCIPManager;
	}

	inline Event<uint32>* User::getItemsAddedEvent()
	{
		return &onItemsAddedEvent;
	}

	inline Event<UserCore::Misc::UpdateInfo>* User::getAppUpdateEvent()
	{
		return &onAppUpdateEvent;
	}

	inline Event<UserCore::Misc::UpdateInfo>* User::getAppUpdateCompleteEvent()
	{
		return &onAppUpdateCompleteEvent;
	}

	inline Event<uint32>* User::getAppUpdateProgEvent()
	{
		return &onAppUpdateProgEvent;
	}

	inline Event<UserCore::Misc::CVar_s>* User::getNeedCvarEvent()
	{
		return &onNeedCvarEvent;
	}

	inline EventC<gcString>* User::getNewAvatarEvent()
	{
		return &onNewAvatarEvent;
	}

	inline Event<WCSpecialInfo>* User::getNeedWildCardEvent()
	{
		return &onNeedWildCardEvent;
	}

	inline Event<std::vector<UserCore::Misc::NewsItem*> >* User::getNewsUpdateEvent()
	{
		return &onNewsUpdateEvent;
	}

	inline Event<std::vector<UserCore::Misc::NewsItem*> >* User::getGiftUpdateEvent()
	{
		return &onGiftUpdateEvent;
	}

	inline Event<std::vector<UserCore::Item::ItemUpdateInfo*> >* User::getItemUpdateEvent()
	{
		return &onItemUpdateEvent;
	}

	inline EventV* User::getUserUpdateEvent()
	{
		return &onUserUpdateEvent;
	}

	inline EventV* User::getPipeDisconnectEvent()
	{
		return &onPipeDisconnect;
	}

	inline Event<std::tuple<gcOptional<bool>, gcOptional<bool>, gcOptional<bool>>>* User::getForcedUpdatePollEvent()
	{
		return &onForcePollEvent;
	}

	inline EventV* User::getLoginItemsLoadedEvent()
	{
		return &onLoginItemsLoadedEvent;
	}

	inline Event<std::pair<bool, char> >* User::getLowSpaceEvent()
	{
		return &onLowSpaceEvent;
	}

	//other

	#ifdef WIN32
	inline void User::setMainWindowHandle(HWND handle)
	{
		m_WinHandle = handle;
	}

	inline HWND User::getMainWindowHandle()
	{
		return m_WinHandle;
	}
	#endif

	inline BDManager* User::getBDManager()
	{
		return m_pBannerDownloadManager;
	}

	inline bool User::isAltProvider()
	{
		return m_bAltProvider;
	}

}

#endif //DESURA_UserCore::User_H
