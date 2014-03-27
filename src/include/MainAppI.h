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

#ifndef DESURA_MAIN_APP_I_H
#define DESURA_MAIN_APP_I_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcFrame.h"
#include "LinkArgs.h"

enum class APP_MODE
{
	MODE_UNINT = 0,		//not yet setup
	MODE_ONLINE,		//normal client
	MODE_ONLINE_MIN,	//small client
	MODE_OFFLINE,		//offline client
	MODE_LOGOUT,		//logged out (no client)
};

// CANT CHANGE THE ORDER/NUMBERING OF THIS AS ITS A PART OF THE JAVASCRIPT API
enum
{
	ACTION_INSTALL = 0,
	ACTION_TEST,
	ACTION_UNINSTALL,
	ACTION_VERIFY,
	ACTION_LAUNCH,
	ACTION_UPLOAD,
	ACTION_RESUPLOAD,
	ACTION_CREATE,
	ACTION_PROPERTIES, //NO LONGER USED
	ACTION_DEVPROFILE,
	ACTION_PROFILE,
	ACTION_UPDATELOG,
	ACTION_APPUPDATELOG,
	ACTION_ACCOUNTSTAT,
	ACTION_DEVPAGE,
	ACTION_UPDATE,
	ACTION_SHOWUPDATE,
	ACTION_INSCHECK,
	ACTION_INSTALLEDW,
	ACTION_PAUSE,
	ACTION_UNPAUSE,
	ACTION_PLAY,
	ACTION_SWITCHBRANCH,
	ACTION_PROMPT,
	ACTION_CLEANCOMPLEXMOD,
	ACTION_DISPCDKEY,
	ACTION_SHOWSETTINGS,

	ACTION_SHOWCONSOLE,

	ACTION_LAST,
};

// CANT CHANGE THE ORDER/NUMBERING OF THIS AS ITS A PART OF THE JAVASCRIPT API
enum PAGE
{
	ITEMS = 0,
	GAMES,
#ifndef UI_HIDE_MODS
	MODS,
#endif
	COMMUNITY,
	DEVELOPMENT,
	SUPPORT,
	END_PAGE,
	NONE = 0,
};

class WCSpecialInfo;
class MainForm;


class MainAppI : public gcFrame
{
public:
	virtual ~MainAppI(){;}

	virtual void showPage(PAGE page)=0;
	virtual void showNews()=0;
	virtual void showPlay()=0;
	virtual void showMainWindow(bool raise = false)=0;

	virtual void handleInternalLink(const char* link) = 0;
	virtual void handleInternalLink(DesuraId id, uint8 action, const LinkArgs& args = LinkArgs()) = 0;

	//this closes and destroys a sub form
	virtual void closeForm(int32 id)=0;

	virtual void logIn(const char* user, const char* pass)=0;
	virtual void logOut(bool bShowLogin = true, bool autoLogin = false)=0;
	virtual void onLoginAccepted(bool saveLoginInfo = false, bool autologin = false)=0;

	virtual bool isOffline()=0;
	virtual bool isLoggedIn()=0;

	virtual void processWildCards(WCSpecialInfo &info, wxWindow* parent = nullptr)=0;

	virtual void loadUrl(const char* url, PAGE page)=0;

	virtual gcFrame* getMainWindow()=0;

	virtual void disableQuietMode()=0;
	virtual bool isQuietMode()=0;

	virtual bool Destroy(){return wxFrame::Destroy();}

	virtual EventV* getLoginEvent()=0;
	
	virtual wxWindow* getTopLevelWindow()=0;

	virtual void newAccountLogin(const char* username, const char* cookie)=0;
	virtual void newAccountLoginError(const char* szErrorMessage) = 0;

	virtual void showUnitTest(){}
};

class MainAppProviderI
{
public:
	//Changes the server url provider. Set to null to reset
	virtual void setProvider(const char* szProvider) = 0;
	virtual const char* getProvider() const = 0;
};


#endif //DESURA_MAIN_APP_I_H
