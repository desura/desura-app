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
#include "CDKProgress.h"
#include "CDKeyForm.h"

#include "Managers.h"
#include "MainApp.h"

#include "InternalLink.h"

CDKProgress::CDKProgress(wxWindow* parent, bool launch) : BasePage(parent)
{
	m_bLaunch = launch;

	m_labInfo = new wxStaticText( this, wxID_ANY, Managers::GetString("#CDK_PROGRESSINFO"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pbProgress = new gcSpinningBar(this, wxID_ANY, wxDefaultPosition, wxSize( -1,22 ));
	m_butClose = new gcButton( this, wxID_ANY, Managers::GetString("#CLOSE"), wxDefaultPosition, wxDefaultSize, 0 );

	wxBoxSizer* bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	bSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	bSizer1->Add( m_butClose, 0, wxALL, 5 );


	wxFlexGridSizer* fgSizer2 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer2->AddGrowableCol( 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	fgSizer2->Add( m_labInfo, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	fgSizer2->Add( m_pbProgress, 0, wxEXPAND|wxALL, 5 );
	

	wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableRow( 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	fgSizer1->Add( fgSizer2, 1, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	fgSizer1->Add( bSizer1, 1, wxEXPAND, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();


	setParentSize(-1, 140);
	onCompleteEvent += guiDelegate(this, &CDKProgress::onComplete);
	onErrorEvent += guiDelegate(this, &CDKProgress::onError);
}

CDKProgress::~CDKProgress()
{
	
}

void CDKProgress::run()
{
	GetUserCore()->getCDKeyManager()->getCDKeyForCurrentBranch(getItemId(), this);
	m_bOutstandingRequest = true;
}

void CDKProgress::dispose()
{
	if (m_bOutstandingRequest)
		GetUserCore()->getCDKeyManager()->cancelRequest(getItemId(), this);
}

void CDKProgress::onCDKeyComplete(DesuraId id, gcString &cdKey)
{
	m_bOutstandingRequest = false;
	onCompleteEvent(cdKey);
}

void CDKProgress::onCDKeyError(DesuraId id, gcException& e)
{
	m_bOutstandingRequest = false;
	onErrorEvent(e);
}

void CDKProgress::onComplete(gcString& cdKey)
{
	CDKeyForm* keyForm = dynamic_cast<CDKeyForm*>(GetParent());

	if (keyForm)
		keyForm->finish(cdKey.c_str());
}

void CDKProgress::onError(gcException& e)
{
	if (e.getErrId() == ERR_CDKEY && e.getSecErrId() == 117) //not required
	{
		if (m_bLaunch)
			g_pMainApp->handleInternalLink(getItemId(), ACTION_LAUNCH, FormatArgs("cdkey"));

		GetParent()->Close();
	}

	gcFrame* par = dynamic_cast<gcFrame*>(GetParent());
	if (par)
		par->setProgressState(gcFrame::P_ERROR);

	gcErrorBox(GetParent(), "#CDK_ERRTITLE", "#CDK_ERROR", e);
	GetParent()->Close();
}
