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

#ifndef DESURA_MAINAPP_H
#define DESURA_MAINAPP_H

#include "MainAppI.h"


#include "wx/app.h"
#include "webcore/WebCoreI.h"
#include "usercore/UserCoreI.h"
#include "usercore/UploadInfoThreadI.h"
#include "LinkArgs.h"

class gcMessageDialog;

extern "C"
CEXPORT void* CreateMainApp();

extern bool isSafeUrl(const char* url);
extern WebCore::WebCoreI* GetWebCore();
extern UserCore::UserI* GetUserCore();

const char* GetAppVersion();
const char* GetUserCoreVersion();
const char* GetWebCoreVersion();

class ut
{
public:
	gcString path;
	DesuraId id;
	wxWindow *caller;
};


class LoginForm;
class wxOnAppUpdateEvent;
class wxTaskBarIcon;
class MainForm;

namespace UserCore
{
	namespace Thread
	{
		class UserThreadI;
	}

	namespace Misc
	{
		class NewsItem;
	}
}

namespace UnitTest
{
	class MainAppFixture;
}

class InternalLinkInfo
{
public:
	DesuraId id;
	uint8 action;
	LinkArgs args;
};

class InternalLink;
class TaskBarIcon;
class gcUnitTestForm;

class MainAppNoUI
{
public:
	friend class UnitTest::MainAppFixture;

	void onGiftUpdate(std::vector<UserCore::Misc::NewsItem*>& itemList);

	EventV onNotifyGiftUpdateEvent;

	std::mutex m_NewsLock;
	std::vector<std::shared_ptr<UserCore::Misc::NewsItem>> m_vGiftItems;
};

class MainApp : public MainAppI, public MainAppProviderI, protected MainAppNoUI
{
public:
	MainApp();
	virtual ~MainApp();

	void run();
	//wx Function override
	void Init(int argc, wxCmdLineArgsArray &argv);
	void showMainWindow(bool raise = false);

	void onTBIMenuSelect(wxCommandEvent& event);

	void onLoginAccepted(bool saveLoginInfo = false, bool autologin = false);

	void logIn(const char* user, const char* pass);
	void logOut(bool bShowLogin = true, bool autoLogin = false);

	void offlineMode();
	void goOffline();

	bool isOffline();
	bool isLoggedIn();

	void handleInternalLink(const char* link) override;
	void handleInternalLink(DesuraId id, uint8 action, const LinkArgs& args) override;

	void closeMainForm();
	void closeForm(int32 id);
#ifdef NIX
	void toggleCurrentForm();
#endif

	void processWildCards(WCSpecialInfo &info, wxWindow* parent);

	gcFrame* getMainWindow();
	wxWindow* getTopLevelWindow();

	virtual void disableQuietMode();
	virtual bool isQuietMode();

	virtual EventV* getLoginEvent();


	void newAccountLogin(const char* username, const char* cookie) override;
	void newAccountLoginError(const char* szErrorMessage) override;

	const char* getProvider() const override;

	void showUnitTest();

protected:
	void showLogin(bool skipAutoLogin = false);
	void showOffline();
	void showMainForm(bool raise = false, bool show = true);

	void createTaskBarIcon();
	void onClose(wxCloseEvent& event);
	
	void changeAccountState(DesuraId id);

	void loadUrl(const char* url, PAGE page);
	void showProfile(DesuraId id, const LinkArgs &args = LinkArgs());
	void showDevProfile(DesuraId id);
	void showDevPage(DesuraId id);
	void showNews();
	void showPlay();
	void showPage(PAGE page);

	void getSteamUser(WCSpecialInfo *info, wxWindow *parent);

	void onNewsUpdate(std::vector<UserCore::Misc::NewsItem*>& itemList);
	
	void onNeedCvar(UserCore::Misc::CVar_s& info);

	void onAppUpdateProg(uint32& prog);
	void onAppUpdate(UserCore::Misc::UpdateInfo& info);

	void onCookieUpdate();

	void onLoginAcceptedCB(std::pair<bool,bool> &loginInfo);

	void onPipeDisconnect();

	void onInternalStrLink(gcString &link);
	void onInternalLink(InternalLinkInfo& info);

	void onNotifyGiftUpdate();

	void showConsole();

	void setProvider(const char* szProvider) override;

	Event<std::pair<bool,bool>> onLoginAcceptedEvent;
	EventV onLoginEvent;

	Event<InternalLinkInfo> onInternalLinkEvent;
	Event<gcString> onInternalLinkStrEvent;


private:
	friend class Desura;

	gcMessageDialog *m_pOfflineDialog = nullptr;

	std::vector<std::shared_ptr<UserCore::Misc::NewsItem>> m_vNewsItems;

	UserCore::Thread::UserThreadI* m_pDumpThread = nullptr;

	bool m_bQuiteMode = false;
	bool m_bLoggedIn = false;
	APP_MODE m_iMode = APP_MODE::MODE_LOGOUT;

	gcString m_strServiceProvider;
	gcString m_szDesuraCache;

#ifdef WITH_GTEST
	gcUnitTestForm* m_UnitTestForm = nullptr;
#endif

	LoginForm* m_wxLoginForm = nullptr;
	MainForm* m_wxMainForm = nullptr;
	TaskBarIcon* m_wxTBIcon = nullptr;

	InternalLink *m_pInternalLink = nullptr;

	std::mutex m_UserLock;

	friend void cc_NewsTest_cc_func(std::vector<gcString> &argv);
	friend void cc_GiftTest_cc_func(std::vector<gcString> &argv);

#ifdef DEBUG
	friend class PopUpThread;
#endif

	friend void cc_test_news_cc_func(std::vector<gcString> &vArgList);
};


extern MainAppI* g_pMainApp;

#endif
