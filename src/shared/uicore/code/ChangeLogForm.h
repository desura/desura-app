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

#ifndef DESURA_CHANGELOGFORM_H
#define DESURA_CHANGELOGFORM_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcControls.h"
#include "gcMiscWebControl.h"
#include "usercore/ItemInfoI.h"

///////////////////////////////////////////////////////////////////////////////
/// Class ChangeLogForm
///////////////////////////////////////////////////////////////////////////////
class ChangeLogForm : public gcFrame 
{
public:
	ChangeLogForm( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("[Item Name]: Change Log"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 382,336 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
	~ChangeLogForm();
	
	DesuraId getItemId(){return m_uiItemId;}

	void setInfo(UserCore::Item::ItemInfoI* item);	//used for items
	void setInfo(uint32 version);	//used for app

	bool isAppChangeLog(){return m_bApp;}

protected:
	gcMiscWebControl* m_ieBrowser;
	gcButton* m_butClose;

	void onFormClose( wxCloseEvent& event );
	void onButClick( wxCommandEvent& event );

private:
	DesuraId m_uiItemId;
	bool m_bApp;
};

#endif //DESURA_CHANGELOGFORM_H
