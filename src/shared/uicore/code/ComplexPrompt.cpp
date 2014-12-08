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
#include "ComplexPrompt.h"
#include "MainApp.h"

BEGIN_EVENT_TABLE( ComplexPrompt, gcDialog )
	EVT_BUTTON( wxID_ANY, ComplexPrompt::onButtonClick )
END_EVENT_TABLE()

ComplexPrompt::ComplexPrompt( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : gcDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 3, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableRow( 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_labInfo = new wxStaticText( this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
	m_labInfo->Wrap( 350 );
	fgSizer1->Add( m_labInfo, 0, wxALL, 5 );


	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );


	bSizer1->Add( 0, 0, 1, wxEXPAND, 5 );

	m_butMoreInfo = new gcButton( this, wxID_ANY, Managers::GetString(L"#MOREINFO"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_butMoreInfo, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );

	m_butOk = new gcButton( this, wxID_ANY, Managers::GetString(L"#CONTINUE"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_butOk, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );

	m_butCancel = new gcButton( this, wxID_ANY, Managers::GetString(L"#CANCEL"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_butCancel, 0, wxALL, 5 );

	fgSizer1->Add( bSizer1, 1, wxEXPAND, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();

	SetTitle(Managers::GetString(L"#IF_CONFLICTTITLE"));
	centerOnParent();
}

ComplexPrompt::~ComplexPrompt()
{
}

void ComplexPrompt::setInfo(DesuraId id)
{
	auto item = GetUserCore()->getItemManager()->findItemInfo(id);

	if (!item)
	{
		EndModal(wxID_CANCEL);
		return;
	}

	m_labInfo->SetLabel(gcWString(Managers::GetString(L"#IF_CONFLICT"), item->getName()));
	m_labInfo->Wrap( 350 );
}

void ComplexPrompt::onButtonClick(wxCommandEvent& event)
{
	if (event.GetId() == m_butMoreInfo->GetId())
	{
		gcLaunchDefaultBrowser(GetWebCore()->getUrl(WebCore::ComplexModTutorial));
	}
	else if (event.GetId() == m_butOk->GetId())
	{
		EndModal(wxID_OK);
	}
	else if (event.GetId() == m_butCancel->GetId())
	{
		EndModal(wxID_CANCEL);
	}
}
