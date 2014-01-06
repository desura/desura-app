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


#include "Common.h"
#include "UploadPrompt.h"
#include "managers/CVar.h"

CVar gc_uploadprompt("gc_uploadprompt", "1", CFLAG_USER);

BEGIN_EVENT_TABLE( gcUploadPrompt, gcDialog )
	EVT_BUTTON( wxID_ANY, gcUploadPrompt::onButClick )
END_EVENT_TABLE()

gcUploadPrompt::gcUploadPrompt( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : gcDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 3, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labInfo = new wxStaticText( this, wxID_ANY, Managers::GetString(L"#UDF_PROMPTINFO"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	m_labInfo->Wrap( 230 );
	fgSizer1->Add( m_labInfo, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_cbReminder = new gcCheckBox( this, wxID_ANY, Managers::GetString(L"#UDF_REMINDER"), wxDefaultPosition, wxDefaultSize, 0 );

	fgSizer1->Add( m_cbReminder, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	
	bSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_butClose = new gcButton( this, wxID_ANY, Managers::GetString(L"#CLOSE"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_butClose, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	bSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	fgSizer1->Add( bSizer1, 1, wxEXPAND, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();
}

gcUploadPrompt::~gcUploadPrompt()
{

}

void gcUploadPrompt::onButClick( wxCommandEvent& event )
{
	EndModal(0);
}

void gcUploadPrompt::EndModal( int code )
{
	gc_uploadprompt.setValue(!m_cbReminder->GetValue());
	gcDialog::EndModal(code);
}
