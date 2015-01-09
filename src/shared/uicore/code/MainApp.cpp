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


#include "Common.h"
#include "MainApp.h"

#include <wx/wx.h>
#include "TaskBarIcon.h"
#include "wx/window.h"
#include "Managers.h"
#include "Log.h"
#include <branding/uicore_version.h>
#include "mcfcore/MCFMain.h"

#include "managers/CVar.h"

#include "LoginForm.h"
#include "usercore/NewsItem.h"
#include "wx/taskbar.h"
#include "MainForm.h"


#include <wx/uri.h>

#include "InternalLink.h"
#include "DesuraServiceError.h"
#include "gcJSBinding.h"

#ifdef WITH_GTEST
#include "gcUnitTestPage.h"
#endif

extern void DeleteCookies();
extern void SetCookies();

// These should NOT be globals...
extern CVar gc_savelogin;
extern CVar admin_developer;

gcString g_szUICoreVersion("{0}.{1}.{2}.{3}", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILDNO, VERSION_EXTEND);

void InitManagers();
void InitLocalManagers();
void DestroyManagers();
void SetCrashDumpSettings(const wchar_t* user, bool upload);

const char* GetUICoreVersion()
{
	return g_szUICoreVersion.c_str();
}

//this is the handle to webcore and related functions
static gcRefPtr<UserCore::UserI> g_pUserHandle;

gcRefPtr<WebCore::WebCoreI> GetWebCore()
{
	if (g_pUserHandle)
		return g_pUserHandle->getWebCore();

	return nullptr;
}

gcRefPtr<UserCore::UserI> GetUserCore()
{
	return g_pUserHandle;
}



extern const char* GetAppVersion();
extern CVar gc_lastusername;
extern CVar gc_uploaddumps;
extern CVar gc_cleanmcf;
extern CVar gc_corecount;
extern CVar gc_silentlaunch;

extern ConCommand cc_PrintVersion;
extern ConCommand cc_CheckCert;


class DeleteUserThread : public ::Thread::BaseThread
{
public:
	DeleteUserThread(gcRefPtr<UserCore::UserI> user) : ::Thread::BaseThread("Delete User Thread")
	{
		m_pUser = user;
	}

	void run() override
	{
		if (m_pUser)
			m_pUser->destroy();
	}

	gcRefPtr<UserCore::UserI> m_pUser;
};

DeleteUserThread* g_pDeleteThread = nullptr;

class AutoDelDeleteThread
{
public:
	~AutoDelDeleteThread()
	{
		safe_delete(g_pDeleteThread);
	}
};

AutoDelDeleteThread addt;

static const std::vector<wxString> g_szSafeList =
{
	".desura.com",
	"desura.com",
	".paypal.com",
	"paypal.com",
	".google.com",
	"google.com",
	".lindenlab.com",
	"lindenlab.com",
};

static bool isSafeUrl(const wxString &server, const wxString &safeUrl)
{
	if (safeUrl.size()==0)
		return false;

	if (safeUrl[0] == '.')
		return server.Contains(safeUrl);

	return server == safeUrl;
}

static bool isSafeUrl(const char* url, MainAppProviderI* pMainApp)
{
	if (!url)
		return false;

	if (gcString(url) == "about:blank")
		return true;

	wxURI wxurl(url);

	if (wxurl.GetScheme() == "file")
		return true;

	wxString server = wxurl.GetServer();

	size_t x=0;

	for (auto s : g_szSafeList)
	{
		if (isSafeUrl(server, s))
			return true;

		x++;
	}

	if (pMainApp && pMainApp->getProvider())
	{
		wxString cusProvider(pMainApp->getProvider());

		if (cusProvider.empty())
			cusProvider = "desura.com";

		if (isSafeUrl(server, cusProvider))
			return true;

		cusProvider = "." + cusProvider;
		return isSafeUrl(server, cusProvider);
	}

	return false;
}

bool isSafeUrl(const char* url)
{
	return isSafeUrl(url, dynamic_cast<MainAppProviderI*>(g_pMainApp));
}

extern CVar gc_autostart;
extern CVar gc_enable_api_debugging;

wxWindow* GetMainWindow(wxWindow* p)
{
	if (!p || p == g_pMainApp)
		return g_pMainApp->getTopLevelWindow();

	return p;
}

#ifdef WIN32
ProxyControl::ProxyControl()
	: m_UserSettingsNeedRestoring( false )
{
}

ProxyControl::~ProxyControl()
{
	RestoreUserProxySettings();
}

/*
[HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Internet Settings\Connections] "DefaultConnectionSettings"=hex:
3c,00,00,00,1f,00,00,00,--05--,00,00,00,00,00,00, 00,00,00,00,00,00,00,00,00,01,00,00,00,1f,00,00,00,68,74,74,70,3a,2f,2f,31, 34,34,2e,31,33,31,2e,32,32,32,2e,31,36,37,2f,77,70,61,64,2e,64,61,74,90,0e, 1e,66,d3,88,c5,01,01,00,00,00,8d,a8,4e,9e,00,00,00,00,00,00,00,00

It's a bitfield on the 9th byte:

0x1: (Always 1)
0x2: Proxy enabled
0x4: "Use automatic configuration script" checked
0x8: "Automatically detect settings" checked
*/
void ProxyControl::ForceProxyOff()
{
	bool is64bit = UTIL::WIN::is64OS();

	unsigned char blob[ 1024 ];
	size_t blob_size = 0;

	if ( (blob_size = UTIL::WIN::getRegBinaryValue( "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Connections\\DefaultConnectionSettings", blob, 1023, is64bit )) > 0 )
	{
		if ( blob_size > 9 )
		{
			// Preserve initial values
			m_UserDefaultConnectionSettingsControlByte = blob[ 8 ];
			m_UserProxyEnabled = UTIL::WIN::getRegValueInt( "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ProxyEnable", is64bit );

			// Turn on Automatically detect settings and first bit
			blob[ 8 ] |= 0x09;

			// Turn off ProxyEnabled
			blob[ 8 ] &= 0xfd;

			UTIL::WIN::setRegBinaryValue( "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Connections\\DefaultConnectionSettings", (const char*) blob, blob_size, is64bit );

			UTIL::WIN::setRegValue( "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ProxyEnable", 0, is64bit );

			m_UserSettingsNeedRestoring = true;
		}
	}
}

void ProxyControl::RestoreUserProxySettings()
{
	if ( m_UserSettingsNeedRestoring )
	{
		bool is64bit = UTIL::WIN::is64OS();

		unsigned char blob[ 1024 ];
		size_t blob_size = 0;

		if ( (blob_size = UTIL::WIN::getRegBinaryValue( "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Connections\\DefaultConnectionSettings", blob, 1023, is64bit )) > 0 )
		{
			if ( blob_size > 9 )
			{
				// Restore initial values
				blob[ 8 ] = m_UserDefaultConnectionSettingsControlByte;

				UTIL::WIN::setRegBinaryValue( "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\Connections\\DefaultConnectionSettings", (const char*) blob, blob_size, is64bit );

				UTIL::WIN::setRegValue( "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ProxyEnable", m_UserProxyEnabled, is64bit );
			}
		}

		m_UserSettingsNeedRestoring = false;
	}
}

#endif


MainApp::MainApp()
{
	Bind(wxEVT_CLOSE_WINDOW, &MainApp::onClose, this);

	//need to override the value in core count if not set
	if (gc_corecount.getInt() == 0)
	{
		//need to change the value so it will trigger the cvar callback
		gc_corecount.setValue(1);
		gc_corecount.setValue(0);
	}

	onLoginAcceptedEvent += guiDelegate(this, &MainApp::onLoginAcceptedCB);
	onInternalLinkEvent += guiDelegate(this, &MainApp::onInternalLink);
	onInternalLinkStrEvent += guiDelegate(this, &MainApp::onInternalStrLink);
	onNotifyGiftUpdateEvent += guiDelegate(this, &MainApp::onNotifyGiftUpdate);

	onLoggedOutEvent += guiDelegate(this, &MainApp::onLoggedOut);
}

MainApp::~MainApp()
{
#ifdef WIN32
	// Do we need to restore Proxy?
	m_ProxyControl.RestoreUserProxySettings();
#endif


	if (m_pOfflineDialog)
		m_pOfflineDialog->EndModal(0);

#ifdef WITH_GTEST
	if (m_UnitTestForm)
	{
		m_UnitTestForm->canClose();
		m_UnitTestForm->Close();
	}
#endif

	safe_delete(m_pInternalLink);

	//delete user first so threads will not die when they try to access webcore
	//should be deleted on logout but just to make sure
	std::lock_guard<std::mutex> a(m_UserLock);

	if (m_wxTBIcon)
		m_wxTBIcon->deregEvents();

	safe_delete(g_pDeleteThread);
	g_pDeleteThread = new DeleteUserThread(g_pUserHandle);

#ifdef NIX
	g_pDeleteThread->start();
#else
	g_pDeleteThread->run();
	safe_delete(g_pDeleteThread);
#endif

	g_pUserHandle = nullptr;
	safe_delete(m_wxTBIcon);

	DestroyManagers();
	DestroyLogging();

#if WIN32
	// Temporary resolve
	PostQuitMessage(0);
#endif // WIN32
}

gcFrame* MainApp::getMainWindow()
{
	return m_wxMainForm;
}

wxWindow* MainApp::getTopLevelWindow()
{
	if (m_wxLoginForm)
		return m_wxLoginForm;

	return m_wxMainForm;
}

void MainApp::Init(int argc, wxCmdLineArgsArray &argv)
{
	gcTrace("");

	if (argc > 0)
	{
		for (int x=0; x<argc; x++)
		{
			wxString str = argv[x].MakeLower();

			if (str == "-dgl")
				m_bQuiteMode = true;

			if (str.StartsWith("desura://"))
				m_szDesuraCache = gcString(argv[x].ToStdString());
		}
	}

	//char *comAppPath = nullptr;
	//UTIL::OS::getAppDataPath(&comAppPath);
	//UTIL::FS::recMakeFolder(comAppPath);
	//safe_delete(comAppPath);

	InitLogging();
	cc_PrintVersion();
	cc_CheckCert();
	Msg("Logging Has started\n");
	Msg("\n\n");

	InitManagers();
	InitLocalManagers();

	std::string val = UTIL::OS::getConfigValue(REGRUN);
	gc_autostart.setValue( val.size() > 0 );

	//because logging gets init first we need to man reg it
	RegLogWithWindow();

	//because logging gets init before managers we need to reapply the color scheme.
	LoggingapplyTheme();
	loadFrame(wxDEFAULT_FRAME_STYLE);

#ifndef NIX
	if (!m_bQuiteMode)
		m_wxTBIcon = new TaskBarIcon(this);
#endif
	m_wxTBIcon = nullptr;

	std::string szAppid = UTIL::OS::getConfigValue(APPID);

	uint32 appid = -1;

	if (szAppid.size() > 0)
		appid = Safe::atoi(szAppid.c_str());

	if (appid == BUILDID_BETA || appid == BUILDID_INTERNAL)
		gc_uploaddumps.setValue(true);
}

void MainApp::run()
{
	showLogin();
}

void MainApp::disableQuietMode()
{
#ifndef NIX
	if (!m_wxTBIcon)
	{
		m_wxTBIcon = new TaskBarIcon(this);
		m_wxTBIcon->regEvents();
	}
#endif

	m_bQuiteMode = false;
}

bool MainApp::isQuietMode()
{
	return m_bQuiteMode;
}

void MainApp::logIn(const char* user, const char* pass)
{
	gcTrace("User: {0}", user);

	std::lock_guard<std::mutex> a(m_UserLock);

	safe_delete(g_pUserHandle);

	gcString path = UTIL::OS::getAppDataPath();

#ifdef WIN32
	if ( UTIL::OS::isProxyOff() )
	{
		m_ProxyControl.ForceProxyOff();
	}
#endif

	g_pUserHandle = (UserCore::UserI*)UserCore::FactoryBuilderUC( USERCORE );
	g_pUserHandle->init(path.c_str(), m_strServiceProvider.c_str());


	try
	{
		//need to do this here as news items will be passed onlogin
		g_pUserHandle->getNewsUpdateEvent() += delegate(this, &MainApp::onNewsUpdate);
		g_pUserHandle->getGiftUpdateEvent() += delegate((MainAppNoUI*)this, &MainAppNoUI::onGiftUpdate);
		g_pUserHandle->getNeedCvarEvent() += delegate(this, &MainApp::onNeedCvar);

		g_pUserHandle->lockDelete();
		g_pUserHandle->logIn(user, pass);
		g_pUserHandle->unlockDelete();

#ifndef DEBUG
		if (gc_enable_api_debugging.getBool())
			g_pUserHandle->getWebCore()->enableDebugging();
#endif
	}
	catch (gcException)
	{
		g_pUserHandle->logOut();

		g_pUserHandle->getNewsUpdateEvent() -= delegate(this, &MainApp::onNewsUpdate);
		g_pUserHandle->getGiftUpdateEvent() -= delegate((MainAppNoUI*)this, &MainAppNoUI::onGiftUpdate);
		g_pUserHandle->getNeedCvarEvent() -= delegate(this, &MainApp::onNeedCvar);

		g_pUserHandle->unlockDelete();

		g_pUserHandle->logOut(false, false);
		safe_delete(g_pUserHandle);
		throw;
	}
}

void MainApp::logOut(bool bShowLogin, bool autoLogin, bool webLoggedOut)
{
	gcTrace("");

	m_pInternalLink->closeAll();
	safe_delete(m_pInternalLink);

	closeMainForm();
	wxTheApp->ProcessIdle();

	{
		std::lock_guard<std::mutex> a(m_UserLock);
		if (g_pUserHandle)
		{
			auto user = g_pUserHandle;

			g_pUserHandle = nullptr;
			DesuraJSBinding::gs_pItemManager = nullptr;

			user->getPipeDisconnectEvent() -= guiDelegate(this, &MainApp::onPipeDisconnect);
			user->logOut(!autoLogin);

			user->getAppUpdateProgEvent()				-= guiDelegate(this, &MainApp::onAppUpdateProg);
			user->getAppUpdateCompleteEvent()			-= guiDelegate(this, &MainApp::onAppUpdate);
			user->getWebCore()->getCookieUpdateEvent() -= guiDelegate(this, &MainApp::onCookieUpdate);
			user->getWebCore()->getLoggedOutEvent()		-= delegate(&onLoggedOutEvent);

			user->destroy();
		}
	}

	GetCVarManager()->saveAll();
	GetCVarManager()->cleanUserCvars();

	m_bLoggedIn = false;
	m_iMode = APP_MODE::MODE_UNINT;

#ifdef WIN32
	// Do we need to restore Proxy?
	m_ProxyControl.RestoreUserProxySettings();
#endif

	if (bShowLogin)
		showLogin(!autoLogin, webLoggedOut);

	HideLogForm();

	DeleteCookies();
	SetCrashDumpSettings(nullptr, true);
}

void MainApp::goOffline()
{
	gcTrace("");

	int res = wxID_NO;

	{
		gcMessageDialog msgBox(nullptr, Managers::GetString(L"#MF_OFFLINE"), Managers::GetString(L"#MF_OFFLINE_TITLE"), wxYES_NO | wxICON_QUESTION);
		AutoScopeMemberVar<gcMessageDialog> asv(m_pOfflineDialog, &msgBox);
		res = msgBox.ShowModal();
	}

	if (res == wxID_YES)
		offlineMode();
}

bool MainApp::isOffline()
{
	return (m_iMode == APP_MODE::MODE_OFFLINE);
}

bool MainApp::isLoggedIn()
{
	return (m_iMode == APP_MODE::MODE_ONLINE);
}

void MainApp::offlineMode()
{
	if (m_iMode == APP_MODE::MODE_OFFLINE)
		return;

	if (m_bLoggedIn)
		logOut(false);

	closeMainForm();

	{
		std::lock_guard<std::mutex> a(m_UserLock);

		gcString path = UTIL::OS::getAppDataPath();

		if (g_pUserHandle)
			g_pUserHandle->destroy();

		g_pUserHandle = (UserCore::UserI*)UserCore::FactoryBuilderUC(USERCORE);
		g_pUserHandle->init(path.c_str());

		try
		{
			g_pUserHandle->getNeedCvarEvent() += delegate(this, &MainApp::onNeedCvar);
			g_pUserHandle->getItemManager()->loadItems();
		}
		catch (gcException &)
		{
			g_pUserHandle->getNeedCvarEvent() -= delegate(this, &MainApp::onNeedCvar);
			g_pUserHandle->logOut();
			g_pUserHandle->destroy();
			throw;
		}
	}

	GetCVarManager()->loadUser(GetUserCore()->getUserId());
	m_iMode = APP_MODE::MODE_OFFLINE;

	showMainWindow();
	m_pInternalLink = new InternalLink(this);
}

void MainApp::showMainWindow(bool raise)
{
	if (!m_bLoggedIn && m_iMode != APP_MODE::MODE_OFFLINE)
	{
		showLogin();
	}
	else
	{
		showMainForm(raise);
	}
}


void MainApp::onClose(wxCloseEvent& event)
{
	gcTrace("");

	if (m_pOfflineDialog)
		m_pOfflineDialog->EndModal(0);

	if (m_wxLoginForm)
	{
		m_wxLoginForm->Show(false);
		m_wxLoginForm->Close(true);
	}

	if (m_wxMainForm)
	{
		m_wxMainForm->Show(false);
		m_wxMainForm->Close(true);
	}

	HideLogForm();

	if (m_pInternalLink)
		m_pInternalLink->closeAll();

	Destroy();
}



void MainApp::closeMainForm()
{
	if (!m_wxMainForm)
		return;

	wxFrame* temp = m_wxMainForm;
	m_wxMainForm = nullptr;

	temp->Show(false);
	temp->Close(true);
	temp->Destroy();
}

EventV* MainApp::getLoginEvent()
{
	return &onLoginEvent;
}

void MainApp::onLoginAccepted(bool saveLoginInfo, bool autologin)
{
	gcTrace("");

	std::pair<bool,bool> res(saveLoginInfo, autologin);
	onLoginAcceptedEvent(res);
}

void MainApp::onLoginAcceptedCB(std::pair<bool,bool> &loginInfo)
{
	bool saveLoginInfo = loginInfo.first;
	bool autologin = loginInfo.second;

	if ( m_wxLoginForm )
	{
		m_wxLoginForm->Show(false);
		m_wxLoginForm->Destroy();
		m_wxLoginForm = nullptr;
	}

	if (saveLoginInfo)
		GetUserCore()->saveLoginInfo();

	GetUserCore()->getAppUpdateProgEvent() += guiDelegate(this, &MainApp::onAppUpdateProg);
	GetUserCore()->getAppUpdateCompleteEvent() += guiDelegate(this, &MainApp::onAppUpdate);
	GetWebCore()->getCookieUpdateEvent() += guiDelegate(this, &MainApp::onCookieUpdate);
	GetWebCore()->getLoggedOutEvent() += delegate(&onLoggedOutEvent);
	GetUserCore()->getPipeDisconnectEvent() += guiDelegate(this, &MainApp::onPipeDisconnect);

	//trigger this so it sets cookies first time around
	onCookieUpdate();


	admin_developer.setValue(GetUserCore()->isAdmin());
	GetCVarManager()->loadUser(GetUserCore()->getUserId());

	gcWString userName(GetUserCore()->getUserName());
	SetCrashDumpSettings(userName.c_str(), gc_uploaddumps.getBool());

	m_bLoggedIn = true;
	m_iMode = APP_MODE::MODE_ONLINE;

	bool showMain = !(autologin && gc_silentlaunch.getBool());
	showMainForm(false, showMain);

	m_pInternalLink = new InternalLink(this);

	if (!m_bQuiteMode)
		GetUserCore()->getItemManager()->checkItems();

	if (m_wxTBIcon)
		m_wxTBIcon->regEvents();

	if (showMain && !m_bQuiteMode)
		showNews();

	if (m_szDesuraCache != "")
	{
		m_pInternalLink->handleInternalLink(m_szDesuraCache.c_str());
		m_szDesuraCache = "";
	}

	onLoginEvent();
}

#ifdef NIX
void MainApp::toggleCurrentForm()
{
	if (!m_wxMainForm && !m_wxLoginForm)
		return;

	if (m_wxMainForm)
	{
		if (m_wxMainForm->IsShown())
			m_wxMainForm->Show(false);
		else
			m_wxMainForm->forceRaise();
	}
	else if (m_wxLoginForm)
	{
		if (m_wxLoginForm->IsShown())
		{
			m_wxLoginForm->Show(false);
		}
		else
		{
			m_wxLoginForm->Show(true);
			m_wxLoginForm->Raise();
		}
	}
}
#endif

void MainApp::showMainForm(bool raise, bool show)
{
	if (m_wxLoginForm)
	{
		m_wxLoginForm->Show(false);
		m_wxLoginForm->Destroy();
		m_wxLoginForm = nullptr;
	}

	if (!m_wxMainForm)
		m_wxMainForm = new MainForm(this, m_iMode == APP_MODE::MODE_OFFLINE, m_strServiceProvider.c_str());

	if (m_wxMainForm->IsIconized())
	{
		m_wxMainForm->Iconize(false);
	}


	m_wxMainForm->setMode(m_iMode);

	if (!m_bQuiteMode)
		m_wxMainForm->Show(show);

	if (!m_bQuiteMode && raise)
		m_wxMainForm->forceRaise();
}

void MainApp::showLogin(bool skipAutoLogin, bool webLoggedOut)
{
#ifdef DEBUG
	showUnitTest();
#endif

	if (!m_wxLoginForm)
		m_wxLoginForm = new LoginForm(this);

	if (!skipAutoLogin && gc_savelogin.getBool())
		m_wxLoginForm->autoLogin();

	m_wxLoginForm->Show(!m_bQuiteMode);

	if (m_wxLoginForm->IsShown())
		m_wxLoginForm->Raise();

	if (webLoggedOut)
		m_wxLoginForm->showForcedLogoutPromt();
}

void MainApp::onAppUpdateProg(uint32& prog)
{
	if (prog == 0 || prog == 100)
	{
		setProgressState(P_NONE);
	}
	else
	{
		setProgressState(P_NORMAL);
		setProgress((uint8)prog);
	}
}

void MainApp::onAppUpdate(UserCore::Misc::UpdateInfo& info)
{
	gcTrace("");

	setProgressState(P_NONE);
	m_pInternalLink->showAppUpdate(info.branch);
}

void MainApp::onNewsUpdate(std::vector<gcRefPtr<UserCore::Misc::NewsItem>>& itemList)
{
	gcTrace("");

	std::lock_guard<std::mutex> guard(m_NewsLock);

	for (auto i : itemList)
		m_vNewsItems.push_back(i);
}

void MainApp::onNotifyGiftUpdate()
{
	m_wxTBIcon->showGiftPopup(m_vGiftItems);
}

void MainAppNoUI::onGiftUpdate(std::vector<gcRefPtr<UserCore::Misc::NewsItem>>& itemList)
{
	std::vector<gcRefPtr<UserCore::Misc::NewsItem>> oldList;
	std::vector<gcRefPtr<UserCore::Misc::NewsItem>> output;

	{
		std::lock_guard<std::mutex> guard(m_NewsLock);

		oldList = m_vGiftItems;
		m_vGiftItems.clear();

		for (auto i : itemList)
		{
			if (!i)
				continue;

			m_vGiftItems.push_back(i);
		}

		auto it = std::set_intersection(begin(m_vGiftItems), end(m_vGiftItems), begin(oldList), end(oldList), std::back_inserter(output),
			[](gcRefPtr<UserCore::Misc::NewsItem> a, gcRefPtr<UserCore::Misc::NewsItem> b){
			return a->id < b->id;
		});

		for (auto i : output)
			i->hasBeenShown = true;
	}

	if (m_vGiftItems.size() != output.size())
	{
		gcTrace("");
		onNotifyGiftUpdateEvent();
	}
}

void MainApp::onNeedCvar(UserCore::Misc::CVar_s& info)
{
	if (info.name)
	{
		auto c = GetCVarManager()->findCVar(info.name);

		if (c)
			info.value = c->getString();
	}
}

void MainApp::onCookieUpdate()
{
	SetCookies();
}

void MainApp::onLoggedOut()
{
	logOut(true, false, true);
}

void MainApp::onPipeDisconnect()
{
	if (!m_bLoggedIn)
		return;

	gcTrace("");
	DesuraServiceError dse(getMainWindow());
	dse.ShowModal();
}

void MainApp::newAccountLogin(const char* username, const char* cookie)
{
	gcTrace("User: {0}", username);

	if (!m_bLoggedIn && m_iMode != APP_MODE::MODE_OFFLINE && m_wxLoginForm)
		m_wxLoginForm->newAccountLogin(username, cookie);
}

void MainApp::showUnitTest()
{
#ifdef WITH_GTEST
	if (!m_UnitTestForm)
		m_UnitTestForm = new gcUnitTestForm(this);

	m_UnitTestForm->postShowEvent();
#endif
}

void MainApp::newAccountLoginError(const char* szErrorMessage)
{
	if (!m_bLoggedIn && m_iMode != APP_MODE::MODE_OFFLINE && m_wxLoginForm)
		m_wxLoginForm->newAccountLoginError(szErrorMessage);
}

void MainApp::setProvider(const char* szProvider)
{
	m_strServiceProvider = szProvider;
}

const char* MainApp::getProvider() const
{
	return m_strServiceProvider.c_str();
}

void MainApp::userLoggedOut()
{
	onLoggedOutEvent();
}


#ifdef WITH_GTEST

namespace UnitTest
{
	class StubMainAppProvider : public MainAppProviderI
	{
	public:
		//Changes the server url provider. Set to null to reset
		void setProvider(const char* szProvider)
		{
			m_szProvider = szProvider;
		}

		const char* getProvider() const
		{
			return m_szProvider;
		}

		const char* m_szProvider;
	};

	TEST(MainApp, SafeUrlNormal)
	{
		ASSERT_TRUE(isSafeUrl("http://desura.com", nullptr));
		ASSERT_TRUE(isSafeUrl("http://www.desura.com", nullptr));
		ASSERT_TRUE(isSafeUrl("http://api.desura.com", nullptr));

		ASSERT_TRUE(isSafeUrl("http://google.com", nullptr));
		ASSERT_TRUE(isSafeUrl("http://www.google.com", nullptr));
		ASSERT_TRUE(isSafeUrl("http://stuff.google.com", nullptr));

		ASSERT_TRUE(isSafeUrl("http://paypal.com", nullptr));
		ASSERT_TRUE(isSafeUrl("http://www.paypal.com", nullptr));
		ASSERT_TRUE(isSafeUrl("http://stuff.paypal.com", nullptr));

		ASSERT_TRUE(isSafeUrl("http://lindenlab.com", nullptr));
		ASSERT_TRUE(isSafeUrl("http://www.lindenlab.com", nullptr));
		ASSERT_TRUE(isSafeUrl("http://stuff.lindenlab.com", nullptr));

		ASSERT_FALSE(isSafeUrl("http://desura.blah.com", nullptr));
		ASSERT_FALSE(isSafeUrl("http://www.desura.blah.com", nullptr));
		ASSERT_FALSE(isSafeUrl("http://api.desura.blah.com", nullptr));
	}

	TEST(MainApp, SafeUrlStaging)
	{
		StubMainAppProvider a;
		a.setProvider("desura.blah.com");

		ASSERT_TRUE(isSafeUrl("http://desura.blah.com", &a));
		ASSERT_TRUE(isSafeUrl("http://www.desura.blah.com", &a));
		ASSERT_TRUE(isSafeUrl("http://api.desura.blah.com", &a));
	}

	class MainAppFixture : public ::testing::Test
	{
	public:
		MainAppFixture()
		{
			m_MainApp.onNotifyGiftUpdateEvent += delegate(this, &MainAppFixture::onGiftUpdateCallback);
		}

		void setExistingGift(gcRefPtr<UserCore::Misc::NewsItem> pItem)
		{
			m_MainApp.m_vGiftItems.push_back(pItem);
		}

		void onGiftUpdate(std::vector<gcRefPtr<UserCore::Misc::NewsItem>> &itemList)
		{
			m_MainApp.onGiftUpdate(itemList);
		}

		void onGiftUpdateCallback()
		{
			m_bHitGiftUpdate = true;
		}

		bool m_bHitGiftUpdate = false;
		MainAppNoUI m_MainApp;
	};

	TEST_F(MainAppFixture, GiftUpdate_NoneExisting)
	{
		std::vector<gcRefPtr<UserCore::Misc::NewsItem>> itemList;
		itemList.push_back(gcRefPtr<UserCore::Misc::NewsItem>::create(1, 1, "", ""));

		onGiftUpdate(itemList);
		ASSERT_TRUE(m_bHitGiftUpdate);
	}

	TEST_F(MainAppFixture, GiftUpdate_OneExisting_Diff)
	{
		std::vector<gcRefPtr<UserCore::Misc::NewsItem>> itemList;
		itemList.push_back(gcRefPtr<UserCore::Misc::NewsItem>::create(2, 1, "", ""));

		setExistingGift(gcRefPtr<UserCore::Misc::NewsItem>::create(1, 1, "", ""));
		onGiftUpdate(itemList);
		ASSERT_TRUE(m_bHitGiftUpdate);
	}

	TEST_F(MainAppFixture, GiftUpdate_OneExisting_Same)
	{
		std::vector<gcRefPtr<UserCore::Misc::NewsItem>> itemList;
		itemList.push_back(gcRefPtr<UserCore::Misc::NewsItem>::create(1, 1, "", ""));

		setExistingGift(gcRefPtr<UserCore::Misc::NewsItem>::create(1, 1, "", ""));
		onGiftUpdate(itemList);
		ASSERT_FALSE(m_bHitGiftUpdate);
	}

	TEST_F(MainAppFixture, GiftUpdate_ManyExisting_Same)
	{
		std::vector<gcRefPtr<UserCore::Misc::NewsItem>> itemList;
		itemList.push_back(gcRefPtr<UserCore::Misc::NewsItem>::create(1, 1, "", ""));
		itemList.push_back(gcRefPtr<UserCore::Misc::NewsItem>::create(2, 1, "", ""));

		setExistingGift(gcRefPtr<UserCore::Misc::NewsItem>::create(1, 1, "", ""));
		setExistingGift(gcRefPtr<UserCore::Misc::NewsItem>::create(2, 1, "", ""));

		onGiftUpdate(itemList);
		ASSERT_FALSE(m_bHitGiftUpdate);
	}

	TEST_F(MainAppFixture, GiftUpdate_ManyExisting_Diff_NoOverlap)
	{
		std::vector<gcRefPtr<UserCore::Misc::NewsItem>> itemList;
		itemList.push_back(gcRefPtr<UserCore::Misc::NewsItem>::create(1, 1, "", ""));
		itemList.push_back(gcRefPtr<UserCore::Misc::NewsItem>::create(2, 1, "", ""));

		setExistingGift(gcRefPtr<UserCore::Misc::NewsItem>::create(10, 1, "", ""));
		setExistingGift(gcRefPtr<UserCore::Misc::NewsItem>::create(11, 1, "", ""));

		onGiftUpdate(itemList);
		ASSERT_TRUE(m_bHitGiftUpdate);
	}

	TEST_F(MainAppFixture, GiftUpdate_ManyExisting_Diff_SomeOverlap)
	{
		std::vector<gcRefPtr<UserCore::Misc::NewsItem>> itemList;
		itemList.push_back(gcRefPtr<UserCore::Misc::NewsItem>::create(1, 1, "", ""));
		itemList.push_back(gcRefPtr<UserCore::Misc::NewsItem>::create(2, 1, "", ""));

		setExistingGift(gcRefPtr<UserCore::Misc::NewsItem>::create(1, 1, "", ""));
		setExistingGift(gcRefPtr<UserCore::Misc::NewsItem>::create(11, 1, "", ""));

		onGiftUpdate(itemList);
		ASSERT_TRUE(m_bHitGiftUpdate);
	}
}

#endif
