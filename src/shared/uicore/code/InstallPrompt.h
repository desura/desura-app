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

#ifndef DESURA_INSTALLPROMPT_H
#define DESURA_INSTALLPROMPT_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcControls.h"
#include "wx/wx.h"

enum
{
	C_VERIFY,
	C_INSTALL,
	C_REMOVE,
};

///////////////////////////////////////////////////////////////////////////////
/// Class InstallPrompt
///////////////////////////////////////////////////////////////////////////////
class InstallPrompt : public gcDialog 
{
public:
	InstallPrompt( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Installing [item]"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 350,179 ), long style = wxCAPTION|wxFRAME_FLOAT_ON_PARENT|wxFRAME_NO_TASKBAR|wxSYSTEM_MENU|wxTAB_TRAVERSAL );
	~InstallPrompt();

	void setInfo(DesuraId id, const char* path);

	uint32 getChoice();

protected:
	wxStaticText* m_labQuestion;
	
	gcButton* m_butExplorer;
	gcButton* m_butOk;
	gcButton* m_butCancel;
	
#ifdef WIN32
	gcStaticText *textA;
	gcStaticText *textB;
	gcStaticText *textC;

	gcRadioButton* m_rbInstallRemove;
	gcRadioButton* m_rbInstallLeave;
	gcRadioButton* m_rbInstallVerify;
#else
	wxCheckBox* m_rbInstallRemove;
	wxCheckBox* m_rbInstallLeave;
	wxCheckBox* m_rbInstallVerify;
#endif

	void onMouseDown( wxMouseEvent& event );
	void onButtonClick(wxCommandEvent& event);
	void onCheckBoxSelect(wxCommandEvent &event);

private:
	gcWString m_szPath;

	DECLARE_EVENT_TABLE();
};


#endif //DESURA_INSTALLPROMPT_H
