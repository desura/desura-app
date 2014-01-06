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
#ifdef WIN32
	CreateMCFForm( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Creating MCF"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 370,120 ), long style = wxCAPTION|wxCLOSE_BOX|wxFRAME_FLOAT_ON_PARENT|wxSYSTEM_MENU );
#else // Linux needs more room as there is no custom border
	CreateMCFForm( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Creating MCF"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 415,120 ), long style = wxCAPTION|wxCLOSE_BOX|wxFRAME_FLOAT_ON_PARENT|wxSYSTEM_MENU );
#endif
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
	void setTitle(const char*);
	void updateInfo(uint32&);
	void onFormClose( wxCloseEvent& event );

	BasePage* m_pPage;	
	wxBoxSizer* m_bsSizer;

	void cleanUpPages();

private:
	bool m_bPromptClose;
	DesuraId m_uiInternId;

	DECLARE_EVENT_TABLE();
};



#endif //DESURA_CREATEFORM_H
