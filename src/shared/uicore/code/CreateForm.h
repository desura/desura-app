/*
Desura is the leading indie game distribution platform
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef DESURA_CREATEFORM_H
#define DESURA_CREATEFORM_H
#ifdef _WIN32
#pragma once
#endif


#include "wx_controls/gcControls.h"
#include "BasePage.h"

#include "MainApp.h"


//wxCAPTION|wxCLOSE_BOX|wxFRAME_FLOAT_ON_PARENT|wxSYSTEM_MENU|wxTAB_TRAVERSAL

///////////////////////////////////////////////////////////////////////////////
/// Class create form
///////////////////////////////////////////////////////////////////////////////
class CreateMCFForm : public gcFrame 
{
public:
	CreateMCFForm(wxWindow* parent, UserCore::ItemManagerI* pItemManager = nullptr);
	~CreateMCFForm();

	void setInfo(DesuraId id);
	void run();

	void showInfo();
	void showProg(const char* path);
	void showOverView(const char* path);

	DesuraId getItemId(){return m_uiInternId;}

	Event<ut> onUploadTriggerEvent; 

	void cancelPrompt(){m_bPromptClose = false;}

protected:
	friend class LanguageTestDialog;

	void setTitle(const char* szItemName, const wchar_t* szFormat = L"#CREATE_MCF_TITILE");
	void updateInfo(uint32&);
	void onFormClose( wxCloseEvent& event );

	BasePage* m_pPage = nullptr;	
	wxBoxSizer* m_bsSizer;

	void cleanUpPages();

private:
	bool m_bPromptClose = true;
	DesuraId m_uiInternId;

	UserCore::ItemManagerI* m_pItemManager = nullptr;

	DECLARE_EVENT_TABLE();
};



#endif //DESURA_CREATEFORM_H
