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
#include "shlobj.h"
#include "GameExplorerManager.h"
#include "UpdateUninstallTask_win.h"
#endif

#include "sqlite3x.hpp"

#include "util_thread/ThreadPool.h"

#include <branding/usercore_version.h>
#include "UpdateThread.h"

#include "UserIPCPipeClient.h"
#include "IPCServiceMain.h"

#include "UserTasks.h"
#include "DownloadUpdateTask.h"
#include "UserThreadManager.h"
#include "UploadManager.h"
#include "ItemManager.h"

namespace UM = UserCore::Misc;

#define CREATE_ACCOLADES "create table accolades(id INTEGER PRIMARY KEY AUTOINCREMENT, internalid INTEGER, desuraid INTEGER, shortname TEXT, fullname TEXT, description TEXT, lastvalue INTEGER DEFAULT 0, newvalue INTEGER DEFAULT 0, maxvalue INTEGER DEFAULT 1, groupid INTEGER);"
#define COUNT_ACCOLADES "select count(*) from sqlite_master where name='accolades';"

#define CREATE_ACCOLADEGROUPS "create table accoladegroups(id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT);"
#define COUNT_ACCOLADEGROUPS "select count(*) from sqlite_master where name='accoladegroups';"


#define CREATE_STATS "create table Stats(id INTEGER PRIMARY KEY AUTOINCREMENT, internalid INTEGER, desuraid INTEGER, name TEXT, type TEXT, value TEXT, dirty INTEGER DEFAULT 0, groupid INTEGER, updatetime DATE);"
#define COUNT_STATS "select count(*) from sqlite_master where name='Stats';"

#define CREATE_STATSGROUPS "create table StatsGroups(id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT);"
#define COUNT_STATSGROUPS "select count(*) from sqlite_master where name='StatsGroups';"

namespace UserCore
{

void User::logOut(bool delAutoLogin, bool reInit)
{
	cleanUp();

	if (reInit)
		init();

	if (delAutoLogin)
	{
		UTIL::FS::Path path(gcString(UTIL::OS::getLocalAppDataPath()), "userinfo.upi", false);
		UTIL::FS::delFile(path);
	}
}

void User::initPipe()
{
	if (m_pPipeClient)
		return;

	m_pPipeClient = std::make_shared<UserIPCPipeClient>(getUserName(), getAppDataPath(), true);
	m_pPipeClient->onDisconnectEvent += delegate(&onPipeDisconnect);

	size_t x=0;

	do
	{
		try
		{
			m_pPipeClient->start();
			break;
		}
		catch (gcException &)
		{
			if (x > 5)
			{
				logOut();
				throw;
			}
			else
			{
				gcSleep(100);
				x++;
			}
		}
	}
	while (true);
}

void User::logInCleanUp()
{
	m_pThreadPool->unBlockTasks();

	m_pItemManager->loadItems();
	m_pItemManager->enableSave();

	initPipe();
}

void User::logIn(const char* user, const char* pass)
{
	try
	{
		doLogIn(user, pass, false);
	}
	catch (...)
	{
		logOut();
		throw;
	}
}

void User::logInTool(const char* user, const char* pass)
{
	try
	{
		doLogIn(user, pass, true);
	}
	catch (...)
	{
		logOut();
		throw;
	}
}

void User::doLogIn(const char* user, const char* pass, bool bTestOnly)
{
	m_pThreadPool->unBlockTasks();

	if (!m_pWebCore)
		throw gcException(ERR_NULLWEBCORE);

	XML::gcXMLDocument doc;
	m_pWebCore->logIn(user, pass, doc);

	auto uNode = doc.GetRoot("memberlogin");

	if (!uNode.IsValid())
		throw gcException(ERR_BADXML);

	uint32 version = 0;
	uNode.GetAtt("version", version);

	if (version == 0)
		version = 1;

	m_bDelayLoading = (version >= 3);

	auto memNode = uNode.FirstChildElement("member");
	
	if (memNode.IsValid())
	{
		m_iUserId = 0;
		memNode.GetAtt("siteareaid", m_iUserId);

		if ((int)m_iUserId <= 0)
		{
			logOut();
			throw gcException(ERR_BAD_PORU);
		}
	}

	memNode.GetChild("admin", m_bAdmin);
	memNode.GetChild("name", m_szUserName);
	memNode.GetChild("nameid", m_szUserNameId);
	memNode.GetChild("url", m_szProfileUrl);
	memNode.GetChild("urledit", m_szProfileEditUrl);

	if (bTestOnly)
		return;

	initPipe();

#ifdef WIN32
	auto fixFolderPermissions = [this](const char* strName, const char* szPath)
	{
		try
		{
			if (getServiceMain())
				getServiceMain()->fixFolderPermissions(szPath);
		}
		catch (gcException* e)
		{
			Warning("Failed to set {0} path to be writeable: {1}", strName, e);
		}
		catch (...)
		{
			Warning("Failed to set {0} path to be writeable: (Unknown error)", strName);
		}
	};

	gcString appDataPath = UTIL::OS::getAppDataPath();
	fixFolderPermissions("AppData", appDataPath.c_str());

	try
	{
		testMcfCache();
	}
	catch (...)
	{
		fixFolderPermissions("Mcf Cache", m_szMcfCachePath.c_str());

		try
		{
			testMcfCache();
		}
		catch (...)
		{
		}
	}
#endif

	gcString szAvatar;
	memNode.GetChild("avatar", szAvatar);

	m_szAvatarUrl = szAvatar;
	m_pThreadPool->queueTask(new UserCore::Task::DownloadAvatarTask(this, szAvatar.c_str(), m_iUserId) );


	auto msgNode = memNode.FirstChildElement("messages");
	if (msgNode.IsValid())
	{
		msgNode.GetChild("updates", m_iUpdates);
		msgNode.GetChild("privatemessages", m_iPms);
		msgNode.GetChild("cart", m_iCartItems);
		msgNode.GetChild("threadwatch", m_iThreads);
	}

	m_pToolManager->loadItems();
	m_pItemManager->loadItems();

	if (m_bDelayLoading)
	{
		//do nothing as the update thread will grab it
	}
	else if (version == 2)
	{
		m_pItemManager->parseLoginXml2(memNode.FirstChildElement("games"), memNode.FirstChildElement("platforms"));
	}
	else
	{
		m_pItemManager->parseLoginXml(memNode.FirstChildElement("games"), memNode.FirstChildElement("developer"));
	}

	auto newsNode = memNode.FirstChildElement("news");
	if (newsNode.IsValid())
		parseNews(newsNode);

	auto giftsNode = memNode.FirstChildElement("gifts");
	if (giftsNode.IsValid())
		parseGifts(giftsNode);

	m_pUThread = m_pThreadManager->newUpdateThread(&onForcePollEvent, m_bDelayLoading);
	m_pUThread->start();

#ifdef WIN32
	m_pGameExplorerManager->loadItems();
#endif

	if (getServiceMain())
		getServiceMain()->updateShortCuts();

	if (!m_bDelayLoading)
	{
		m_pItemManager->enableSave();
#ifdef WIN32
		getThreadPool()->queueTask(new UpdateUninstallTask(this));
#endif
	}

#ifdef WIN32
	gcString val("\"{0}\"", UTIL::OS::getCurrentDir(L"desura.exe"));
	val +=  " \"%1\" -urllink";

	updateRegKey("HKEY_CLASSES_ROOT\\Desura\\shell\\open\\command\\", val.c_str());
#endif
}


}
