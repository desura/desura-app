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
