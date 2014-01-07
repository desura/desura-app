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
#include "InstallDLToolPage.h"

#include "MainApp.h"

namespace UI
{
namespace Forms
{
namespace ItemFormPage
{


InstallDLToolPage::InstallDLToolPage(wxWindow* parent) : BaseInstallPage(parent)
{
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &InstallDLToolPage::onButtonPressed, this);

	m_labLabel = new gcStaticText( this, wxID_ANY, Managers::GetString(L"#IF_DOWNLOADTOOL_LABEL"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pbProgress = new gcProgressBar( this, wxID_ANY, wxDefaultPosition, wxSize( -1,22 ) );
	m_butCancel = new gcButton( this, wxID_ANY, Managers::GetString(L"#CLOSE"), wxDefaultPosition, wxDefaultSize, 0 );


	wxBoxSizer* bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	bSizer4->Add( m_butCancel, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 4, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableRow( 2 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	fgSizer1->Add( m_labLabel, 0, wxEXPAND|wxTOP|wxLEFT, 5 );
	fgSizer1->Add( m_pbProgress, 0, wxALL|wxEXPAND, 5 );
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	fgSizer1->Add( bSizer4, 0, wxEXPAND, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();

	this->setParentSize(-1, 140);

	gcFrame* par = dynamic_cast<gcFrame*>(GetParent());
	if (par)
		par->setProgressState(gcFrame::P_NORMAL);
}

InstallDLToolPage::~InstallDLToolPage()
{
}

void InstallDLToolPage::init()
{
}

void InstallDLToolPage::onButtonPressed(wxCommandEvent& event)
{
	GetParent()->Close();
}


void InstallDLToolPage::onComplete(uint32& res)
{
	gcFrame* par = dynamic_cast<gcFrame*>(GetParent());
	if (par)
		par->setProgressState(gcFrame::P_NONE);

	m_pbProgress->setProgress(100);
}

void InstallDLToolPage::onError(gcException& e)
{
	gcFrame* par = dynamic_cast<gcFrame*>(GetParent());
	if (par)
		par->setProgressState(gcFrame::P_ERROR);

	gcErrorBox(GetParent(), "#IF_IERRTITLE", "#IF_IERROR", e);
}

void InstallDLToolPage::onMcfProgress(MCFCore::Misc::ProgressInfo& info)
{
	if (info.totalAmmount > 0)
	{
		std::string done = UTIL::MISC::niceSizeStr(info.doneAmmount, true);
		std::string total = UTIL::MISC::niceSizeStr(info.totalAmmount);

		m_pbProgress->setCaption(gcString(Managers::GetString("#PROGRESS_INFO"), done, total));
	}

	gcFrame* par = dynamic_cast<gcFrame*>(GetParent());
	if (par)
		par->setProgress(info.percent);

	m_pbProgress->setProgress(info.percent);
	Update();
}

}
}
}