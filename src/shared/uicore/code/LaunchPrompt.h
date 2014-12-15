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

#ifndef DESURA_LAUNCHPROMPT_H
#define DESURA_LAUNCHPROMPT_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcControls.h"
#include "Managers.h"
#include "usercore/ItemInfoI.h"

///////////////////////////////////////////////////////////////////////////////
/// Class LaunchItemDialog
///////////////////////////////////////////////////////////////////////////////
class LaunchItemDialog : public gcFrame
{
public:
	LaunchItemDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Launch Item"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 370,101 ), long style = wxCAPTION|wxCLOSE_BOX|wxFRAME_FLOAT_ON_PARENT|wxFRAME_TOOL_WINDOW|wxSYSTEM_MENU|wxTAB_TRAVERSAL );
	~LaunchItemDialog();

	void setInfo(gcRefPtr<UserCore::Item::ItemInfoI> item);

	DesuraId getItemId(){return m_uiInternId;}

protected:
	wxStaticText* m_labItemName;

	gcButton* m_butLaunch;
	gcButton* m_butCancel;

	void onButtonPressed(wxCommandEvent& event);
	void onFormClose( wxCloseEvent& event );
private:
	DesuraId m_uiInternId;

	DECLARE_EVENT_TABLE();
};


#endif //DESURA_LAUNCHPROMPT_H
