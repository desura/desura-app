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
#include "LaunchPrompt.h"
#include "MainApp.h"

BEGIN_EVENT_TABLE( LaunchItemDialog, gcFrame )
	EVT_BUTTON( wxID_ANY, LaunchItemDialog::onButtonPressed )
	EVT_CLOSE( LaunchItemDialog::onFormClose )
END_EVENT_TABLE()



LaunchItemDialog::LaunchItemDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : gcFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	m_labItemName = new wxStaticText( this, wxID_ANY, wxT("The [item], [name],"), wxDefaultPosition, wxDefaultSize, 0 );
	m_butLaunch = new gcButton( this, wxID_ANY, Managers::GetString(L"#LAUNCH"), wxDefaultPosition, wxDefaultSize, 0 );
	m_butCancel = new gcButton( this, wxID_ANY, Managers::GetString(L"#CANCEL"), wxDefaultPosition, wxDefaultSize, 0 );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableRow( 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxBoxSizer* bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	bSizer3->Add( m_labItemName, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );



	wxBoxSizer* bSizer2 = new wxBoxSizer( wxHORIZONTAL );

	bSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	bSizer2->Add( m_butLaunch, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	bSizer2->Add( m_butCancel, 0, wxALL, 5 );

	fgSizer1->Add( bSizer3, 1, wxEXPAND, 5 );
	fgSizer1->Add( bSizer2, 1, wxEXPAND, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();

	centerOnParent();
}

LaunchItemDialog::~LaunchItemDialog()
{
}

void LaunchItemDialog::onFormClose( wxCloseEvent& event )
{
	g_pMainApp->closeForm(this->GetId());
}

void LaunchItemDialog::setInfo(gcRefPtr<UserCore::Item::ItemInfoI> item)
{
	if (!item)
	{
		Close();
		return;
	}

	if (item->getIcon() && UTIL::FS::isValidFile(UTIL::FS::PathWithFile(item->getIcon())))
		setIcon(item->getIcon());

	m_uiInternId = item->getId();

	m_labItemName->SetLabel(gcWString(Managers::GetString(L"#LI_LABEL"), m_uiInternId.getTypeString(), item->getName()));
	SetTitle(gcWString(L"{0} {1}..", Managers::GetString(L"#LAUNCH"), item->getName()));
}


void LaunchItemDialog::onButtonPressed(wxCommandEvent& event)
{
	if (event.GetId() == m_butLaunch->GetId())
	{
		g_pMainApp->handleInternalLink(getItemId(), ACTION_LAUNCH);
		Close();
	}
	else if (event.GetId() == m_butCancel->GetId())
	{
		Close();
	}
}
