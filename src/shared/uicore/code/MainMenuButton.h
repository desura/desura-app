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

#ifndef DESURA_MAINMENUBUTTON_H
#define DESURA_MAINMENUBUTTON_H
#ifdef _WIN32
#pragma once
#endif

#include <wx/control.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>

#include <wx/menu.h>
#include "BaseMenuButton.h"

#include "wx_controls/gcCustomMenu.h"

enum
{
	DESURA_wxOffline = 1000,
	DESURA_wxLogOut,
	DESURA_wxSettings,
	DESURA_wxBackUp,
	DESURA_wxNewMCF,
	DESURA_wxOpenMCF,
	DESURA_wxInstallMCF,
	DESURA_wxModWizard,
	DESURA_wxBandwidthMon,
	DESURA_wxSpeedSched,
	DESURA_wxAbout,
	DESURA_wxHelp,
	DESURA_wxLog,
	DESURA_wxExit,
	DESURA_wxChangeLog,
	DESURA_wxCustomInstallPath,
	DESURA_wxListKeys,
	DESURA_wxDownloadReport,
	DESURA_wxCart,
	DESURA_wxPurchase,
	DESURA_wxGifts,
	DESURA_wxActivateGame,
	DESURA_wxUnitTest,
};

class gcMainCustomFrameImpl;
class gcMenu;

class MainMenuButton : public gcImageButton
{
public:
	MainMenuButton(wxWindow* parent, bool offline);
	~MainMenuButton();

#ifdef WIN32
	virtual WXLRESULT MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
#endif
	void onActiveToggle(bool state);

protected:
	void createMenu(bool offline = false);
	void onMouseClick(wxCommandEvent& event);
	void onBlur(wxFocusEvent& event);

private:
	gcMenu* m_mainMenu;
	bool m_bIgnoreNextClick;
};


#endif //DESURA_MAINMENUBUTTON_H
