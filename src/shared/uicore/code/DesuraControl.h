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

#ifndef DESURA_DESURACONTROL_H
#define DESURA_DESURACONTROL_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcControls.h"
#include "MainAppI.h"

class BaseTabPage;
class MainMenuButton;
class MenuStrip;
class ButtonStrip;
class UsernameBox;
class gcStatusBar;
class gcMainCustomFrameImpl;
class MainFormLeftBorder;
class DesuraMenuFiller;
class FrameButtons;
class BaseToolBarControl;

typedef struct
{
	int32 id;
	std::shared_ptr<BaseTabPage> page;
	std::shared_ptr<BaseToolBarControl> header;
} tabInfo_s;

///////////////////////////////////////////////////////////////////////////////
/// Class tabPage
///////////////////////////////////////////////////////////////////////////////
class DesuraControl : public gcPanel 
{
public:
	DesuraControl(gcFrame* parent, bool offline, const char* szProvider);
	~DesuraControl();

	void addPage(std::shared_ptr<BaseTabPage> page, const char* tabName);

	void setActivePage(PAGE index, bool reset = false);
	void setActivePage_ID(int32 id);

	void showLeftBorder(bool state);
	void refreshSearch();

	void setBaseTabPage(PAGE pageId, std::shared_ptr<BaseTabPage> &page);
	PAGE getActivePage();

protected:
	wxBoxSizer* m_sizerHeader;
	wxFlexGridSizer* m_sizerContent;

	MainMenuButton *m_pMainMenuButton;
	MenuStrip *m_pMenuStrip;
	ButtonStrip *m_bButtonStrip;
	UsernameBox *m_pUsernameBox;
	DesuraMenuFiller *m_pFiller;
	FrameButtons* m_pFrameButtons;

	gcImageButton *m_pAvatar;

	void unloadSearch(std::shared_ptr<BaseTabPage> &page);
	void loadSearch(std::shared_ptr<BaseTabPage> &page);
	void updateStatusBar(uint32 index);

	void onButtonClicked( wxCommandEvent& event );
	void onMenuSelect( wxCommandEvent& event );
	void onExitPressed( wxMouseEvent& event );
	void onResize( wxSizeEvent& event );

	void onProgressUpdate(uint32& idInt);
	void onNewAvatar(gcString strImage);
	void onDesuraUpdate(uint32& prog);

	void onActiveToggle(bool &state);

private:
	std::vector<tabInfo_s> m_vTabInfo;
	uint32 m_iIndex;

	bool m_bDownloadingUpdate;
	bool m_bOffline;

	uint32 m_uiUpdateProgress;

	DECLARE_EVENT_TABLE();
};



#endif //DESURA_DESURACONTROL_H
