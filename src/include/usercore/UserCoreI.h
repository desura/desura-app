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

#ifndef DESURA_USERI_H
#define DESURA_USERI_H
#ifdef _WIN32
#pragma once
#endif

#ifdef LINK_WITH_GMOCK
#include <gmock/gmock.h>
#endif

#include "usercore/NewsItem.h"
#include "usercore/ItemHandleI.h"
#include "usercore/ItemInfoI.h"
#include "usercore/ItemManagerI.h"
#include "util/gcOptional.h"

class WCSpecialInfo;

#define USERCORE					"USERCORE_INTERFACE_001"
#define USERCORE_VER				"USERCORE_VERSION"
#define USERCORE_GETLOGIN			"USERCORE_GETLOGIN"
#define USERCORE_GETITEMSTATUS		"USERCORE_GETITEMSTATUS"

typedef const char* (*UserCoreVersionFN)();
typedef void *UserCoreGetLoginFN(char**, char**);
typedef void (*GetStatusStrFn)(UserCore::Item::ItemHandleI* pItemHandle, UserCore::Item::ItemInfoI *pItemInfo, UserCore::Item::ITEM_STAGE nStage, UserCore::Item::ItemTaskGroupI* pTaskGroup, LanguageManagerI &pLangMng, char* buffer, uint32 buffsize);

namespace Thread
{
	class ThreadPool;
}

namespace WebCore
{
	class WebCoreI;
}

namespace IPC
{
	class ServiceMainI;
}

namespace UserCore
{


	extern "C"
	{
		CEXPORT void* FactoryBuilderUC(const char* name);
	}

	class UserInternalI;
	class UserThreadManagerI;
	class UploadManagerI;
	class ToolManagerI;
	class GameExplorerManagerI;
	class CDKeyManagerI;
	class CIPManagerI;

	namespace Misc
	{
		//! Get Cvar information stuct
		typedef struct
		{
			const char* name;
			const char* value;
		} CVar_s;

		class UpdateInfo
		{
		public:
			UpdateInfo()
			{
				branch = 0;
				build = 0;
			}

			UpdateInfo(uint32 branch, uint32 build)
			{
				this->branch = branch;
				this->build = build;
			}

			uint32 branch;
			uint32 build;
		};
	}


	enum class PlatformType
	{
		Item,
		Tool,
	};


	class UserI
	{
	public:
		virtual ~UserI(){};

		//! Set up default user
		//!
		//! @param path New profile path
		//!
		virtual void init(const char* appDataPath)=0;

		//! Gets the current app data path
		//!
		virtual const char* getAppDataPath()=0;

		//! Gets the cache path mcf's are saved to
		//!
		virtual const char* getMcfCachePath()=0;

		//! Locks the user and stops it being deleted till unlock is called.
		//!
		virtual void lockDelete()=0;

		//! Unlock the user allowing it to be deleted
		//!
		virtual void unlockDelete()=0;

		///////////////////////////////////////////////////////////////////////////////
		// Login
		///////////////////////////////////////////////////////////////////////////////

		//! Log in as a Desura User
		//!
		//! @param user Username
		//! @param password User password
		//!
		virtual void logIn(const char* user, const char* password)=0;

		//! Logout
		//!
		virtual void logOut(bool delAutoLogin = false, bool reInit = true)=0;

		//! Saves the username and password hash to a file
		//!
		virtual void saveLoginInfo()=0;

		//! Login for the cleanup utility so it doesnt need a valid desura login
		//!
		virtual void logInCleanUp()=0;

		///////////////////////////////////////////////////////////////////////////////
		// Misc
		///////////////////////////////////////////////////////////////////////////////

		//! Is user an admin
		//!
		//! @return True if logged in user is an admin
		//!
		virtual bool isAdmin()=0;

		//! Is the login items delay loading
		//!
		virtual bool isDelayLoading()=0;

		//! Call this if a update needs to be downloaded
		//!
		//! @param appid Application branch (100, 300, 500)
		//! @param appver Application build
		//!
		virtual void appNeedUpdate(uint32 appid = 0, uint32 appver = 0, bool bForced = false)=0;

		//! Used to restart connection to desura serivce
		//!
		virtual void restartPipe()=0;

		//! Forces the update poll to run
		//!
		virtual void forceUpdatePoll()=0;

		//! Forces the app to trigger an update for qa testing
		//!
		virtual void forceQATestingUpdate()=0;

		//! Enables QA Testing builds (admin only)
		//!
		virtual void setQATesting(bool bEnable = true)=0;

		///////////////////////////////////////////////////////////////////////////////
		// Getters
		///////////////////////////////////////////////////////////////////////////////

		//! Get user id
		//!
		//! @return User id
		//!
		virtual uint32 getUserId()=0;

		//! Get user avatar (as web url or local file path)
		//!
		//! @return User avatar
		//!
		virtual const char* getAvatar()=0;

		//! Get user profile url
		//!
		//! @return User profile url
		//!
		virtual const char* getProfileUrl()=0;

		//! Get user profile url in edit mode
		//!
		//! @return User edit url
		//!
		virtual const char* getProfileEditUrl()=0;

		//! Get user name id
		//!
		//! @return User name id
		//!
		virtual const char* getUserNameId()=0;
	
		//! Get user login name
		//!
		//! @return User login name
		//!
		virtual const char* getUserName()=0;

		//! Gets the personal message count
		//!
		//! @return PM count
		//!
		virtual uint32 getPmCount()=0;

		//! Gets the update count
		//!
		//! @return Update count
		//!
		virtual uint32 getUpCount()=0;

		//! Gets the number of items in a users cart
		//!
		//! @return Cart count
		//!
		virtual uint32 getCartCount()=0;

		//! Gets the number of new threads
		//!
		//! @return Thread count
		//!
		virtual uint32 getThreadCount()=0;

		//! Get a cvar value from ui core
		//!
		//! @param cvarName The name of the cvar to find the value of
		//! @return CVar value or null if not found
		//!
		virtual const char* getCVarValue(const char* cvarName)=0;

		//! Gets the thread pool for this user
		//!
		//! @return Threadpool
		//!
		virtual ::Thread::ThreadPool* getThreadPool()=0;

		//! Gets the service main for talking to Desura service
		//!
		//! @return ServiceMain
		//!
		virtual IPC::ServiceMainI* getServiceMain()=0;

		//! Gets the webcore handle
		//!
		//! @return WebCore
		//!
		virtual WebCore::WebCoreI* getWebCore()=0;

		//! Gets the Thread manager handle
		//!
		//! @return Thread manager
		//!
		virtual UserCore::UserThreadManagerI* getThreadManager()=0;

		//! Gets the Upload manager handle
		//!
		//! @return Upload manager
		//!
		virtual UserCore::UploadManagerI* getUploadManager()=0;

		//! Gets the Item manager handle
		//!
		//! @return Item manager
		//!
		virtual UserCore::ItemManagerI* getItemManager()=0;

		//! Gets the Tool manager handle
		//!
		//! @return Tool manager
		//!
		virtual UserCore::ToolManagerI* getToolManager()=0;

		//! Gets the Game Explorer manager handle
		//!
		//! @return Game Explorer manager
		//!
		virtual GameExplorerManagerI* getGameExplorerManager()=0;

		//! Gets the cd key manager
		//!
		//! @return CDKey Manager
		//!
		virtual UserCore::CDKeyManagerI* getCDKeyManager()=0;

		//! Gets the custom install path manager
		//!
		//! @return custom install path manager
		//!
		virtual UserCore::CIPManagerI* getCIPManager()=0;


		///////////////////////////////////////////////////////////////////////////////
		// Events
		///////////////////////////////////////////////////////////////////////////////

		//! Gets the item added event handler. Triggered when an item is added to the list
		//!
		//! @return Item added event
		//!
		virtual Event<uint32>* getItemsAddedEvent()=0;

		//! Gets the application update event handler. Triggered when an update is about to downloaded
		//!
		//! @return Application update event
		//!
		virtual Event<UserCore::Misc::UpdateInfo>* getAppUpdateEvent()=0;

		//! Gets the application update event handler. Triggered when an update is fully downloaded
		//!
		//! @return Application update complete event
		//!
		virtual Event<UserCore::Misc::UpdateInfo>* getAppUpdateCompleteEvent()=0;

		//! Gets the application update progress event handler. Triggered when progress is made on update download
		//! uint32 = progress
		//!
		//! @return Application update progress event
		//!
		virtual Event<uint32>* getAppUpdateProgEvent()=0;

		//! Gets the need cvar event handler. Triggered when needing resolving of a cvar in uicore
		//!
		//! @return Cvar event
		//!
		virtual Event<UserCore::Misc::CVar_s>* getNeedCvarEvent()=0;

		//! Gets the new avatar event handler. Triggered when user avatar is resolved
		//!
		//! @return Avatar event
		//!
		virtual EventC<gcString>* getNewAvatarEvent()=0;

		//! Gets the wildcard event handler. Triggered when needing a wildcard resolved by ui core
		//!
		//! @return Wildcard event
		//!
		virtual Event<WCSpecialInfo>* getNeedWildCardEvent()=0;

		//! Gets the news event handler. Triggered when new news
		//!
		//! @return News event
		//!
		virtual Event<std::vector<UserCore::Misc::NewsItem*> >* getNewsUpdateEvent()=0;

		//! Gets the gift event handler. Triggered when new gifts
		//!
		//! @return Gift event
		//!
		virtual Event<std::vector<UserCore::Misc::NewsItem*> >* getGiftUpdateEvent()=0;

		//! Gets the item update event handler. Triggered when an item information has been updated
		//!
		//! @return Item update event
		//!
		virtual Event<std::vector<UserCore::Item::ItemUpdateInfo*> >* getItemUpdateEvent()=0;

		//! Gets the user update event handler. Triggered when an user information has been updated
		//!
		//! @return User update event
		//!
		virtual EventV* getUserUpdateEvent()=0;

		//! Gets the event for pipe disconnect. Triggered when desura service dies.
		//!
		//! @return Pipe disconnect event
		//!
		virtual EventV* getPipeDisconnectEvent()=0;

		//! Gets the event for when a forced update poll is triggered.
		//!
		//! @return Force update poll event, 
		//!		set first bool to force update poll to run
		//!		set second bool to enable qa builds (admins only)
		//!		set third bool to force an update (maybe to same version)
		//!
		virtual Event<std::tuple<gcOptional<bool>, gcOptional<bool>, gcOptional<bool>>>* getForcedUpdatePollEvent()=0;

		//! Gets the event for when the login items are loaded
		//!
		virtual EventV* getLoginItemsLoadedEvent()=0;

		//! Gets the event for when low hdd space for mcf saves is detected
		//! First Event paramater is true if the low space is on the system disk, or false on another disk
		//! Second Event paramater is the hdd leter
		//!
		virtual Event<std::pair<bool, char> >* getLowSpaceEvent()=0;

		//! Updates the user counts. Used by javascript api as a cheap update method
		//!
		//! @param msgs Msg count
		//! @param updates Update count
		//! @param threads Thread count
		//! @param cart Cart count
		//!
		virtual void setCounts(uint32 msgs, uint32 updates, uint32 threads, uint32 cart)=0;

		//! Sets the main window handle to use with ShellExecute
		//!
		//! @pram handle Main window handle
		//!
	#ifdef WIN32
		virtual void setMainWindowHandle(HWND handle)=0;
	#endif

		//! Do not use. Depreciated
		//!
		virtual void updateUninstallInfo()=0;

		//! Updates the uninstall registry for one item
		//!
		//! @param id Item id
		//! @param installSize Install size
		//!
		virtual void updateUninstallInfo(DesuraId id, uint64 installSize)=0;

		//! Removes the uninstall registry for one item
		//!
		//! @param id Item id
		//!
		virtual void removeUninstallInfo(DesuraId id)=0;

		//! Updates a registry key using desura install service
		//!
		//! @param key Key to update
		//! @param value Value to set it to
		//!
		virtual void updateRegKey(const char* key, const char* value)=0;
		virtual void updateBinaryRegKey(const char* key, const char* value, size_t size)=0;

		virtual void runInstallScript(const char* file, const char* installPath, const char* function)=0;

		//! Logs in with out starting all the extra stuff
		virtual void logInTool(const char* user, const char* pass)=0;
	
		virtual void init(const char* appDataPath, const char* szProviderUrl)=0;

		//! Set the avatar url
		virtual void setAvatarUrl(const char* szAvatarUrl)=0;

		//! Is this platform active
		//!
		virtual bool platformFilter(const XML::gcXMLElement &platform, PlatformType type)=0;

		//Get internal user object (for user core)
		//!
		virtual UserInternalI* getInternal()=0;
	};


#ifdef LINK_WITH_GMOCK
	class UserMock : public UserI
	{
	public:
		MOCK_METHOD1(init, void(const char* appDataPath));
		MOCK_METHOD0(getAppDataPath, const char*());
		MOCK_METHOD0(getMcfCachePath, const char*());
		MOCK_METHOD0(lockDelete, void());
		MOCK_METHOD0(unlockDelete, void());
		MOCK_METHOD2(logIn, void(const char* user, const char* password));
		MOCK_METHOD2(logOut, void(bool delAutoLogin, bool reInit));
		MOCK_METHOD0(saveLoginInfo, void());
		MOCK_METHOD0(logInCleanUp, void());
		MOCK_METHOD0(isAdmin, bool());
		MOCK_METHOD0(isDelayLoading, bool());
		MOCK_METHOD3(appNeedUpdate, void(uint32 appid, uint32 appver, bool bForced));
		MOCK_METHOD0(restartPipe, void());
		MOCK_METHOD0(forceUpdatePoll, void());
		MOCK_METHOD0(forceQATestingUpdate, void());
		MOCK_METHOD1(setQATesting, void(bool bEnable));
		MOCK_METHOD0(getUserId, uint32());
		MOCK_METHOD0(getAvatar, const char*());
		MOCK_METHOD0(getProfileUrl, const char*());
		MOCK_METHOD0(getProfileEditUrl, const char*());
		MOCK_METHOD0(getUserNameId, const char*());
		MOCK_METHOD0(getUserName, const char*());
		MOCK_METHOD0(getPmCount, uint32());
		MOCK_METHOD0(getUpCount, uint32());
		MOCK_METHOD0(getCartCount, uint32());
		MOCK_METHOD0(getThreadCount, uint32());
		MOCK_METHOD1(getCVarValue, const char*(const char* cvarName));
		MOCK_METHOD0(getThreadPool, ::Thread::ThreadPool*());
		MOCK_METHOD0(getServiceMain, IPC::ServiceMainI*());
		MOCK_METHOD0(getWebCore, WebCore::WebCoreI*());
		MOCK_METHOD0(getThreadManager, UserCore::UserThreadManagerI*());
		MOCK_METHOD0(getUploadManager, UserCore::UploadManagerI*());
		MOCK_METHOD0(getItemManager, UserCore::ItemManagerI*());
		MOCK_METHOD0(getToolManager, UserCore::ToolManagerI*());
		MOCK_METHOD0(getGameExplorerManager, GameExplorerManagerI*());
		MOCK_METHOD0(getCDKeyManager, UserCore::CDKeyManagerI*());
		MOCK_METHOD0(getCIPManager, UserCore::CIPManagerI*());
		MOCK_METHOD0(getItemsAddedEvent, Event<uint32>*());
		MOCK_METHOD0(getAppUpdateEvent, Event<UserCore::Misc::UpdateInfo>*());
		MOCK_METHOD0(getAppUpdateCompleteEvent, Event<UserCore::Misc::UpdateInfo>*());
		MOCK_METHOD0(getAppUpdateProgEvent, Event<uint32>*());
		MOCK_METHOD0(getNeedCvarEvent, Event<UserCore::Misc::CVar_s>*());
		MOCK_METHOD0(getNewAvatarEvent, EventC<gcString>*());
		MOCK_METHOD0(getNeedWildCardEvent, Event<WCSpecialInfo>*());
		MOCK_METHOD0(getNewsUpdateEvent, Event<std::vector<UserCore::Misc::NewsItem*> >*());
		MOCK_METHOD0(getGiftUpdateEvent, Event<std::vector<UserCore::Misc::NewsItem*> >*());
		MOCK_METHOD0(getItemUpdateEvent, Event<std::vector<UserCore::Item::ItemUpdateInfo*> >*());
		MOCK_METHOD0(getUserUpdateEvent, EventV*());
		MOCK_METHOD0(getPipeDisconnectEvent, EventV*());
		MOCK_METHOD0(getForcedUpdatePollEvent, Event<std::tuple<gcOptional<bool>, gcOptional<bool>, gcOptional<bool>>>*());
		MOCK_METHOD0(getLoginItemsLoadedEvent, EventV*());
		MOCK_METHOD0(getLowSpaceEvent, Event<std::pair<bool, char> >*());
		MOCK_METHOD4(setCounts, void(uint32 msgs, uint32 updates, uint32 threads, uint32 cart));
	#ifdef WIN32
		MOCK_METHOD1(setMainWindowHandle, void(HWND handle));
	#endif
		MOCK_METHOD0(updateUninstallInfo, void());
		MOCK_METHOD2(updateUninstallInfo, void(DesuraId id, uint64 installSize));
		MOCK_METHOD1(removeUninstallInfo, void(DesuraId id));
		MOCK_METHOD2(updateRegKey, void(const char* key, const char* value));
		MOCK_METHOD3(updateBinaryRegKey, void(const char* key, const char* value, size_t size));
		MOCK_METHOD3(runInstallScript, void(const char* file, const char* installPath, const char* function));
		MOCK_METHOD2(logInTool, void(const char* user, const char* pass));
		MOCK_METHOD2(init, void(const char* appDataPath, const char* szProviderUrl));
		MOCK_METHOD1(setAvatarUrl, void(const char* szAvatarUrl));
		MOCK_METHOD2(platformFilter, bool(const XML::gcXMLElement &, PlatformType));
		MOCK_METHOD0(getInternal, UserInternalI*());
	};
#endif


}

#endif //DESURA_UserI_H
