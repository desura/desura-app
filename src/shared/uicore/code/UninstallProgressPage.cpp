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
#include "UninstallProgressPage.h"

namespace UI
{
namespace Forms
{
namespace ItemFormPage
{

UninstallProgressPage::UninstallProgressPage(wxWindow* parent, wxString label) : BaseInstallPage(parent)
{
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &UninstallProgressPage::onButtonClicked, this);

	m_labInfo = new gcStaticText( this, wxID_ANY, Managers::GetString((wchar_t*)label.wchar_str()), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_pbProgress = new gcSpinnerProgBar( this, wxID_ANY, wxDefaultPosition, wxSize( -1,22 ) );
	m_butClose = new gcButton( this, wxID_ANY, Managers::GetString(L"#CLOSE"), wxDefaultPosition, wxDefaultSize, 0 );
	m_butClose->Enable(false);


	wxBoxSizer* bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	bSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	bSizer2->Add( m_butClose, 0, wxALL, 5 );
	

	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 4, 1, 0, 0 );
	fgSizer3->AddGrowableCol(0);
	fgSizer3->AddGrowableRow(2);
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );


	fgSizer3->Add( m_labInfo, 0, wxTOP|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	fgSizer3->Add( m_pbProgress, 0, wxALL|wxEXPAND, 5 );
	fgSizer3->Add(0,0,0,0,0);
	fgSizer3->Add( bSizer2, 0, wxEXPAND, 5 );

	this->SetSizer( fgSizer3 );
	this->Layout();

	m_bRemoveAll = true;
	m_bRemoveAcc = true;

	this->setParentSize(-1, 120);

	gcFrame* par = dynamic_cast<gcFrame*>(GetParent());
	if (par)
		par->setProgressState(gcFrame::P_NORMAL);
}

UninstallProgressPage::~UninstallProgressPage()
{
}

void UninstallProgressPage::init()
{

}

void UninstallProgressPage::onButtonClicked( wxCommandEvent& event )
{
	GetParent()->Close();
}

void UninstallProgressPage::completeUninstall()
{
	gcFrame* par = dynamic_cast<gcFrame*>(GetParent());
	if (par)
		par->setProgressState(gcFrame::P_NONE);

	m_labInfo->SetLabel(Managers::GetString(L"#COMPLTETED"));
	m_pbProgress->setProgress(100);

	m_butClose->Enable(true);
}

void UninstallProgressPage::onComplete(uint32&)
{
	completeUninstall();
}

void UninstallProgressPage::onError(gcException& e)
{
	gcFrame* par = dynamic_cast<gcFrame*>(GetParent());
	if (par)
		par->setProgressState(gcFrame::P_ERROR);

	gcErrorBox(GetParent(), "#UNF_ERRTITLE", "#UNF_ERROR", e);
	completeUninstall();
}

void UninstallProgressPage::onMcfProgress(MCFCore::Misc::ProgressInfo& info)
{
	gcFrame* par = dynamic_cast<gcFrame*>(GetParent());
	if (par)
		par->setProgress(info.percent);

	m_pbProgress->setProgress(info.percent);
	Refresh();
}

}
}
}