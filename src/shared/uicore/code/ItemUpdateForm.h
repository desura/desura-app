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

#ifndef DESURA_ITEMUPDATEFORM_H
#define DESURA_ITEMUPDATEFORM_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcControls.h"
#include "wx/wx.h"
#include "gcMiscWebControl.h"

class UpdateInfoForm : public gcFrame 
{
public:
	UpdateInfoForm( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Update [Item Name]"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 370,250 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
	~UpdateInfoForm();

	void setInfo(DesuraId id, bool launch = false, const LinkArgs &args = LinkArgs());
	DesuraId getItemId(){return m_uiInternId;}

protected:
	
	wxStaticText* m_labInfo;
	gcMiscWebControl* m_ieBrowser;
	
	gcCheckBox* m_cbReminder;
	
	gcButton* m_butUpdate;
	gcButton* m_butLaunch;
	gcButton* nm_butCancel;

	void onButtonPressed(wxCommandEvent& event);
	void onFormClose( wxCloseEvent& event );

private:
	DesuraId m_uiInternId;
	bool m_bLaunch;

	LinkArgs m_Args;

	DECLARE_EVENT_TABLE();
};

#endif //DESURA_ITEMUPDATEFORM_H
