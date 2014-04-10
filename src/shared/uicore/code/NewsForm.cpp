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
#include "NewsForm.h"
#include "MainApp.h"

#include <wx/uri.h>


NewsForm::NewsForm(wxWindow* parent) 
	: gcFrame(parent, wxID_ANY, "[News Form]", wxDefaultPosition, wxSize( 670,600 ), wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL)
{
	Bind(wxEVT_CLOSE_WINDOW, &NewsForm::onFormClose, this);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &NewsForm::onButClick, this);

	this->SetSizeHints( wxSize( 400,300 ), wxDefaultSize );

	m_szTitle = Managers::GetString(L"#NF_TITLE");

	m_ieBrowser = new NewsBrowser(this, "about:blank", "NewsForm");
	m_butPrev = new gcButton(this, wxID_ANY, Managers::GetString(L"#NF_PREV"));
	m_butNext = new gcButton(this, wxID_ANY, Managers::GetString(L"#NF_NEXT"));
	m_butClose = new gcButton(this, wxID_ANY, Managers::GetString(L"#CLOSE"));

	
	wxBoxSizer* bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	bSizer4->Add( m_butClose, 0, wxALL, 5 );
	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	bSizer4->Add( m_butPrev, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	bSizer4->Add( m_butNext, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer2->AddGrowableCol( 0 );
	fgSizer2->AddGrowableRow( 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	fgSizer2->Add( m_ieBrowser, 1, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	fgSizer2->Add( bSizer4, 1, wxEXPAND, 5 );
	
	this->SetSizer( fgSizer2 );
	this->Layout();

	m_uiSelected = 0;
	centerOnParent();

	m_szLoadingUrl = gcString(GetGCThemeManager()->getWebPage("loading"));
}

NewsForm::~NewsForm()
{
}

void NewsForm::setAsGift()
{
	m_szTitle = Managers::GetString(L"#GF_TITLE");
	SetTitle(m_szTitle.c_str());
}

void NewsForm::loadNewsItems(const std::vector<std::shared_ptr<UserCore::Misc::NewsItem>> &itemList)
{
	for (auto i : itemList)
	{
		if (!i)
			continue;

		m_vItemList.push_back(*i);
	}

	loadSelection();
}

void NewsForm::loadNewsItems(const std::vector<UserCore::Misc::NewsItem> &itemList)
{
	for (auto i : itemList)
		m_vItemList.push_back(i);

	loadSelection();
}

void NewsForm::onFormClose( wxCloseEvent& event )
{
	g_pMainApp->closeForm(this->GetId());
}

void NewsForm::onButClick( wxCommandEvent& event )
{
	if (event.GetId() == m_butPrev->GetId())
	{
		if (m_uiSelected > 0)
			m_uiSelected--;

		loadSelection();
	}	
	else if (event.GetId() == m_butNext->GetId())
	{
		if (m_uiSelected < m_vItemList.size()-1)
			m_uiSelected++;

		loadSelection();
	}
	else if (event.GetId() == m_butClose->GetId())
	{
		Close();
	}
}

void NewsForm::loadSelection()
{
	if (m_uiSelected >=  m_vItemList.size())
		m_uiSelected = 0;

	gcWString url(L"{0}?url={1}", m_szLoadingUrl, UTIL::STRING::urlEncode(m_vItemList[m_uiSelected].szUrl));
	m_ieBrowser->loadUrl(url.c_str());
	
	Layout();

	if (m_vItemList.size() > 1)
		SetTitle(gcWString(L"{0} {1} ({2} of {3})", m_szTitle, m_vItemList[m_uiSelected].szTitle, m_uiSelected+1, m_vItemList.size()));
	else
		SetTitle(gcWString(L"{0} {1}", m_szTitle, m_vItemList[m_uiSelected].szTitle));

	if (m_vItemList.size() == 1)
	{
		m_butPrev->Enable(false);
		m_butNext->Enable(false);
	}
	else
	{
		if (m_uiSelected == 0)
		{
			m_butPrev->Enable(false);
			m_butNext->Enable(true);
		}
		else if (m_uiSelected == m_vItemList.size()-1)
		{
			m_butPrev->Enable(true);
			m_butNext->Enable(false);
		}
		else
		{
			m_butPrev->Enable(true);
			m_butNext->Enable(true);
		}
	}
}


