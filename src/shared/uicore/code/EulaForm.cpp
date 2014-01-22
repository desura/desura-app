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
#include "EulaForm.h"
#include "MainApp.h"


EULAForm::EULAForm(wxWindow* parent, UserCore::ItemManagerI* pItemManager) 
	: gcFrame(parent, wxID_ANY, wxT("End User License Agreement"), wxDefaultPosition, wxSize(600, 250), wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL)
	, m_pItemManager(pItemManager)
{
	if (!m_pItemManager)
		m_pItemManager = GetUserCore()->getItemManager();

	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &EULAForm::onButtonPressed, this);
	Bind(wxEVT_CLOSE_WINDOW, &EULAForm::onFormClose, this);

	m_ieBrowser = nullptr;

	m_labInfo = new wxStaticText( this, wxID_ANY, Managers::GetString(L"#EULA_LABEL"), wxDefaultPosition, wxDefaultSize, 0 );
	m_butAgree = new gcButton( this, wxID_ANY, Managers::GetString(L"#AGREE"), wxDefaultPosition, wxDefaultSize, 0 );
	nm_butCancel = new gcButton( this, wxID_ANY, Managers::GetString(L"#CANCEL"), wxDefaultPosition, wxDefaultSize, 0 );

	m_butAgree->Enable(false);

	m_BrowserSizer = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	bSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	bSizer2->Add( m_butAgree, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	bSizer2->Add( nm_butCancel, 0, wxALL, 5 );


	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 4, 1, 0, 0 );
	fgSizer2->AddGrowableCol( 0 );
	fgSizer2->AddGrowableRow( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	
	fgSizer2->Add( m_labInfo, 0, wxTOP|wxLEFT|wxRIGHT, 5 );
	fgSizer2->Add( m_BrowserSizer, 1, wxALL|wxEXPAND, 5 );
	fgSizer2->Add( bSizer2, 0, wxEXPAND, 5 );
	

	this->SetSizer( fgSizer2 );
	this->Layout();

	m_uiInternId = 0;
	centerOnParent();
}

EULAForm::~EULAForm()
{
}

void EULAForm::onPageLoad()
{
	m_butAgree->Enable(true);
}

void EULAForm::onFormClose( wxCloseEvent& event )
{
	g_pMainApp->closeForm(this->GetId());
}

bool EULAForm::setInfo(DesuraId id)
{
	m_uiInternId = id;
	UserCore::Item::ItemInfoI* item = m_pItemManager->findItemInfo( id );

	if (!item)
	{
		Warning("Cant find item for eula form.\n");
		Close();
		return false;
	}

	if (item->getIcon() && UTIL::FS::isValidFile(UTIL::FS::PathWithFile(item->getIcon())))
		setIcon(item->getIcon());

	gcWString title(Managers::GetString(L"#EULA_TITLE"), item->getName());
	gcString url = item->getEulaUrl();

	SetTitle(title);

	m_ieBrowser = new gcMiscWebControl(this, url.c_str(), "EulaFrom");
	m_ieBrowser->onPageLoadEvent += delegate(this, &EULAForm::onPageLoad);

	m_BrowserSizer->Add(m_ieBrowser, 1, wxEXPAND, 0);
	Layout();

	return true;
}

void EULAForm::onButtonPressed(wxCommandEvent& event)
{
	if (event.GetId() == m_butAgree->GetId())
	{
		UserCore::Item::ItemInfoI* item = m_pItemManager->findItemInfo( m_uiInternId );

		if (item)
		{
			item->acceptEula();
			g_pMainApp->handleInternalLink(getItemId(), ACTION_LAUNCH);
		}
	}

	Close();
}