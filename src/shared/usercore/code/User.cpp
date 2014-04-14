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

#include "Common.h"
#include "User.h"

#ifdef WIN32
#include <shlobj.h>
#endif

#include "UpdateThread.h"

#include <branding/usercore_version.h>

#include "UserIPCPipeClient.h"
#include "IPCServiceMain.h"

#include "util_thread/BaseThread.h"
#include "util_thread/ThreadPool.h"
#include "DownloadUpdateTask.h"
#include "UserTasks.h"
#include "UserThreadManager.h"
#include "UploadManager.h"

#ifdef WIN32
#include "GameExplorerManager.h"
#endif

#include "sqlite3x.hpp"

#include "BDManager.h"
#include "McfManager.h"

namespace UM = UserCore::Misc;
using namespace UserCore;


User::User()
	: m_bAltProvider(false)
{
	onLoginItemsLoadedEvent += delegate(this, &User::onLoginItemsLoaded);
}

User::~User() 
{
	while (m_bLocked)
	{
		m_WaitCond.wait(0, 500);
	}

	cleanUp();

	onNeedWildCardEvent -= delegate(this, &User::onNeedWildCardCB);

	safe_delete(m_pThreadPool);
	safe_delete(m_pWebCore);
	safe_delete(m_pMcfManager);
}

void User::onLoginItemsLoaded()
{
	m_bDelayLoading = false;
}

void User::lockDelete()
{
	m_bLocked = true;
}

void User::unlockDelete()
{
	m_bLocked = false;
	m_WaitCond.notify();
}

void User::init(const char* appDataPath)
{
	init(appDataPath, nullptr);
}

void User::init(const char* appDataPath, const char* szProviderUrl)
{
	m_bAltProvider = (szProviderUrl && szProviderUrl[0] && gcString("desura.com") != szProviderUrl);


	UTIL::FS::recMakeFolder(UTIL::FS::PathWithFile(appDataPath));

	m_szAppDataPath = appDataPath;

	m_pThreadPool = std::make_shared<::Thread::ThreadPool>(2);
	m_pThreadPool->blockTasks();

	m_pWebCore = std::shared_ptr<WebCore::WebCoreI>((WebCore::WebCoreI*)WebCore::FactoryBuilder(WEBCORE), [](WebCore::WebCoreI* pWebCore){
		pWebCore->destroy();
	});

	m_pWebCore->init(appDataPath, szProviderUrl);

	m_pBannerDownloadManager = std::make_shared<BDManager>(this);
	m_pCDKeyManager = std::make_shared<CDKeyManager>(this);

	m_szMcfCachePath = gcString(UTIL::OS::getMcfCachePath());

	m_pMcfManager = std::make_shared<MCFManager>(appDataPath, m_szMcfCachePath.c_str());
	m_pMcfManager->init();

	init();
}

const char* User::getAppDataPath()
{
	return m_szAppDataPath.c_str();
}

const char* User::getMcfCachePath()
{
	return m_szMcfCachePath.c_str();
}

void User::cleanUp()
{
	m_pThreadPool->purgeTasks();
	m_pThreadPool->blockTasks();
	m_pWebCore->logOut();

	//must delete this one first as upload threads are apart of thread manager
	safe_delete(m_pUploadManager);
	safe_delete(m_pUThread);
	safe_delete(m_pThreadManager);
#ifdef WIN32
	safe_delete(m_pGameExplorerManager);
#endif
	safe_delete(m_pCIPManager);
	safe_delete(m_pItemManager);
	safe_delete(m_pToolManager);
	
	safe_delete(m_pPipeClient);

	safe_delete(m_pCDKeyManager);
	safe_delete(m_pBannerDownloadManager);

	m_szUserName = gcString("");
	m_szUserNameId = gcString("");
	m_szAvatar = gcString("");
	m_bDelayLoading = false;
}

void User::init()
{
	m_pUploadManager = std::make_shared<UserCore::UploadManager>(this);
	m_pThreadManager = std::make_shared<UserCore::UserThreadManager>();
	m_pThreadManager->setUserCore(this);

	m_iUserId = 0;
	m_pUThread = nullptr;
	m_bAdmin = false;
	m_bDelayLoading = false;

	m_uiLastUpdateVer = 0;
	m_uiLastUpdateBuild = 0;

	m_pItemManager = std::make_shared<ItemManager>(this);
	m_pToolManager = std::make_shared<ToolManager>(this);

#ifdef WIN32
	m_pGameExplorerManager = std::make_shared<GameExplorerManager>(this);
#endif

	m_pCIPManager = std::make_shared<CIPManager>(this);
	m_pPipeClient = nullptr;

	m_bDownloadingUpdate = false;
	onNeedWildCardEvent += delegate(this, &User::onNeedWildCardCB);

	m_iThreads = 0;
	m_iUpdates = 0;
	m_iPms = 0;
	m_iCartItems = 0;
#ifdef WIN32
	m_WinHandle = nullptr;
#endif
}

//call this if a update needs to be downloaded
void User::appNeedUpdate(uint32 appver, uint32 appbuild, bool bForced)
{
	gcTrace("Ver {0}, Build {1}, Forced {2}", appver, appbuild, bForced);

	if (m_bDownloadingUpdate)
		return;

	if (!bForced && appver == m_uiLastUpdateVer && appbuild <= m_uiLastUpdateBuild)
		return;

	m_bDownloadingUpdate = true;

	std::string szAppid = UTIL::OS::getConfigValue(APPID);

	if (m_uiLastUpdateVer == 0)
	{
		if (szAppid.size() > 0)
			m_uiLastUpdateVer = Safe::atoi(szAppid.c_str());
		
		if (m_uiLastUpdateVer == 0)
			m_uiLastUpdateVer = 100;
	}

	if (m_uiLastUpdateBuild == 0)
	{
		std::string szAppBuild = UTIL::OS::getConfigValue(APPBUILD);

		if (szAppBuild.size() > 0)
			m_uiLastUpdateBuild = Safe::atoi(szAppBuild.c_str());

		if (m_uiLastUpdateBuild == 0) 
			m_uiLastUpdateBuild = 0;
	}

	uint32 curAppVer=0;

	if (szAppid.size() > 0)
		curAppVer = Safe::atoi(szAppid.c_str());

	//if we changed the appver dont keep downloading updates for the old one
	if (appver == 0 && m_uiLastUpdateVer != curAppVer)
	{
		m_bDownloadingUpdate = false;
		return;
	}

	if (appver != 0)
	{
		m_uiLastUpdateVer = appver;	
		m_uiLastUpdateBuild = appbuild;
	}

	UserCore::Task::DownloadUpdateTask *task = new UserCore::Task::DownloadUpdateTask(this, m_uiLastUpdateVer, m_uiLastUpdateBuild, bForced);

	task->onDownloadCompleteEvent += delegate(this, &User::onUpdateComplete);
	task->onDownloadStartEvent += delegate(this, &User::onUpdateStart);
	task->onDownloadProgressEvent += delegate(getAppUpdateProgEvent());

	m_pThreadPool->forceTask(task);
}

const char* User::getCVarValue(const char* cvarName)
{
	if (!cvarName)
		return nullptr;
		
	UserCore::Misc::CVar_s temp;
	temp.name = cvarName;
	temp.value = nullptr;

	onNeedCvarEvent(temp);
	return temp.value;
}

void User::onUpdateComplete(UserCore::Misc::update_s& info)
{
	gcTrace("");

	m_uiLastUpdateBuild = info.build;
	m_bDownloadingUpdate = false;
	
	if (info.alert)
	{
		UserCore::Misc::UpdateInfo uLast(m_uiLastUpdateVer, m_uiLastUpdateBuild);
		onAppUpdateCompleteEvent(uLast);
	}
}

void User::onUpdateStart(UserCore::Misc::update_s& info)
{
	gcTrace("");

	m_uiLastUpdateBuild = info.build;
	
	if (info.alert)
	{
		UserCore::Misc::UpdateInfo uLast(m_uiLastUpdateVer, m_uiLastUpdateBuild);
		onAppUpdateEvent(uLast);
	}
}

void User::onNeedWildCardCB(WCSpecialInfo& info)
{
	if (info.handled)
		return;

#ifdef WIN32
	if (Safe::stricmp("PROGRAM_FILES", info.name.c_str()) == 0)
	{
		wchar_t path[MAX_PATH]  = {0};
		SHGetFolderPathW(nullptr, CSIDL_PROGRAM_FILES, nullptr, SHGFP_TYPE_CURRENT, path);
		info.result = path;
		info.handled = true;
	}
	else if (Safe::stricmp("DOCUMENTS", info.name.c_str()) == 0)
	{
		wchar_t path[MAX_PATH] = {0};
		SHGetFolderPathW(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, path);
		info.result = path;
		info.handled = true;
	}
	else if (Safe::stricmp("JAVA_PATH", info.name.c_str()) == 0)
	{
		std::string cur = UTIL::WIN::getRegValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\JavaSoft\\Java Runtime Environment\\CurrentVersion");

		if (cur.size() > 0)
		{
			info.result = UTIL::WIN::getRegValue(gcString("HKEY_LOCAL_MACHINE\\SOFTWARE\\JavaSoft\\Java Runtime Environment\\{0}\\JavaHome", cur));
			info.handled = true;
		}
	}
	else if (Safe::stricmp("APP_DATA", info.name.c_str())==0)
	{
		wchar_t path[MAX_PATH]  = {0};
		SHGetFolderPathW(nullptr, CSIDL_COMMON_APPDATA, nullptr, SHGFP_TYPE_CURRENT, path);
		info.result = path;
		info.handled = true;
	}
	else if (Safe::stricmp("USER_APP_DATA", info.name.c_str())==0)
	{
		wchar_t path[MAX_PATH]  = {0};
		SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, path);
		info.result = path;
		info.handled = true;
	}
#else
	if (Safe::stricmp("XDG_OPEN", info.name.c_str()) == 0)
	{
		info.result = gcString(UTIL::OS::getCurrentDir(L"xdg-open"));
		info.handled = true;
	}
#endif
	else if (Safe::stricmp("APPLICATION", info.name.c_str()) == 0)
	{
		info.result = UTIL::OS::getAppInstallPath();
		info.handled = true;
	}
}



void User::setAvatarPath(const char* path)
{
	m_szAvatar = path;
	onNewAvatarEvent(m_szAvatar);
}

void User::downloadImage(UserCore::Item::ItemInfo* itemInfo, uint8 image)
{
	m_pThreadPool->queueTask(new UserCore::Task::DownloadImgTask(this, itemInfo, image));
}

void User::changeAccount(DesuraId id, uint8 action)
{
	m_pThreadPool->queueTask(new UserCore::Task::ChangeAccountTask(this, id, action) );
}



void User::parseNews(const XML::gcXMLElement &newsNode)
{
	parseNewsAndGifts(newsNode, "item", onNewsUpdateEvent);
}


void User::parseGifts(const XML::gcXMLElement &giftNode)
{

	parseNewsAndGifts(giftNode, "gift", onGiftUpdateEvent);
}

void User::parseNewsAndGifts(const XML::gcXMLElement &xmlNode, const char* szChildName, Event<std::vector<UserCore::Misc::NewsItem*> > &onEvent)
{
	if (!xmlNode.IsValid())
		return;

	std::vector<UserCore::Misc::NewsItem*> itemList;

	xmlNode.for_each_child(szChildName, [&itemList](const XML::gcXMLElement &itemElem)
	{
		const std::string szId = itemElem.GetAtt("id");

		gcString szTitle;
		gcString szUrl;

		itemElem.GetChild("title", szTitle);
		itemElem.GetChild("url", szUrl);
			
		if (szId.empty() || szTitle.empty() || szUrl.empty())
			return;

		uint32 id = (uint32)Safe::atoi(szId.c_str());

		UserCore::Misc::NewsItem *temp = new UserCore::Misc::NewsItem(id, 0, szTitle.c_str(), szUrl.c_str());
		itemList.push_back(temp);
	});

	if (itemList.size() > 0)
		onEvent(itemList);

	safe_delete(itemList);
}

void User::restartPipe()
{
	if (m_pPipeClient)
		m_pPipeClient->restart();
}

void User::forceUpdatePoll()
{
	auto t = std::make_tuple<gcOptional<bool>, gcOptional<bool>, gcOptional<bool>>(true, gcOptional<bool>(), gcOptional<bool>());
	onForcePollEvent(t);
}

void User::forceQATestingUpdate()
{
	auto t = std::make_tuple<gcOptional<bool>, gcOptional<bool>, gcOptional<bool>>(gcOptional<bool>(), gcOptional<bool>(), true);
	onForcePollEvent(t);
}

void User::setQATesting(bool bEnable)
{
	auto t = std::make_tuple<gcOptional<bool>, gcOptional<bool>, gcOptional<bool>>(gcOptional<bool>(), bEnable, gcOptional<bool>());
	onForcePollEvent(t);
}

void User::setCounts(uint32 msgs, uint32 updates, uint32 threads, uint32 cart)
{
	bool needsUpdate = false;

	if (cart != UINT_MAX && m_iCartItems != cart)
	{
		m_iCartItems = cart;
		needsUpdate = true;
	}

	if (msgs != UINT_MAX && m_iPms != msgs)
	{
		m_iPms = msgs;
		needsUpdate = true;
	}

	if (updates != UINT_MAX && m_iUpdates != updates)
	{
		m_iUpdates = updates;
		needsUpdate = true;
	}

	if (threads != UINT_MAX && m_iThreads != threads)
	{
		m_iThreads = threads;
		needsUpdate = true;
	}

	if (needsUpdate)
		onUserUpdateEvent();
}

void User::updateUninstallInfo()
{
	gcAssert(false); //shouldnt be used no more
}

void User::updateUninstallInfo(DesuraId id, uint64 installSize)
{
#ifdef WIN32
	if (getServiceMain())
		getServiceMain()->setUninstallRegKey(id.toInt64(), installSize);
#endif
}

void User::removeUninstallInfo(DesuraId id)
{
#ifdef WIN32
	if (getServiceMain())
		getServiceMain()->removeUninstallRegKey(id.toInt64());
#endif
}

void User::updateRegKey(const char* key, const char* value)
{
	if (getServiceMain())
		getServiceMain()->updateRegKey(key, value);
}

void User::updateBinaryRegKey(const char* key, const char* value, size_t size)
{
	if (getServiceMain())
		getServiceMain()->updateBinaryRegKey(key, value, size);
}

void User::runInstallScript(const char* file, const char* installPath, const char* function)
{
	if (getServiceMain())
		getServiceMain()->runInstallScript(file, installPath, function);
}

bool User::platformFilter(const XML::gcXMLElement &platform, PlatformType type)
{
	if (!platform.IsValid())
		return true;

	uint32 id = 0;
	platform.GetAtt("id", id);

	if (id == 0)
		return true;

#ifdef WIN32
	return (id != 100);
#elif defined NIX
	if (type == PlatformType::Tool)
		return (id != 110 && id != 120);
#ifdef NIX64
	if (id == 120)
		return false;
#endif
	//linux will have windows and nix
	return (id != 110); //id != 100 && 
#else
	return true;
#endif
}

void User::testMcfCache()
{
	UTIL::FS::Path p(m_szMcfCachePath, "temp", false);
	UTIL::FS::FileHandle fh(p, UTIL::FS::FILE_WRITE);

	fh.write("1234", 4);
	fh.close();

	UTIL::FS::delFile(p);
}

void User::setAvatarUrl(const char* szAvatarUrl)
{
	if (UTIL::FS::isValidFile(szAvatarUrl))
	{
		setAvatarPath(szAvatarUrl);
	}
	else if (gcString(szAvatarUrl) != (std::string)m_szAvatarUrl)
	{
		m_szAvatarUrl = szAvatarUrl;
		m_pThreadPool->queueTask(new UserCore::Task::DownloadAvatarTask(this, szAvatarUrl, m_iUserId));
	}
}

MCFManagerI* User::getMCFManager()
{
	return m_pMcfManager.get();
}
