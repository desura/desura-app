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
#include "UDFProgressPanel.h"
#include "usercore\UserCoreI.h"

#include "UninstallAllThread.h"
#include "UninstallDesuraForm.h"

UDFProgressPanel::UDFProgressPanel(wxWindow* parent, gcRefPtr<UserCore::UserI> user, uint32 flags)
	: gcPanel( parent, wxID_ANY, wxDefaultPosition, wxSize( 500,-1 ), wxTAB_TRAVERSAL, false)
{
	m_labTotal = new wxStaticText( this, wxID_ANY, Managers::GetString("#DUN_TOTAL"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labCurrent = new wxStaticText( this, wxID_ANY, Managers::GetString("#DUN_CURRENT"), wxDefaultPosition, wxDefaultSize, 0 );

	m_pbTotal = new gcSpinnerProgBar(this, wxID_ANY, wxDefaultPosition, wxSize( -1,22 ) );
	m_pbCurrent = new gcSpinnerProgBar( this, wxID_ANY, wxDefaultPosition, wxSize( -1,22 ) );
	m_tbInfo = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );

	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 5, 1, 0, 0 );
	fgSizer7->AddGrowableCol( 0 );
	fgSizer7->AddGrowableRow( 4 );
	fgSizer7->SetFlexibleDirection( wxBOTH );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	fgSizer7->Add( m_labTotal, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	fgSizer7->Add( m_pbTotal, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	fgSizer7->Add( m_labCurrent, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	fgSizer7->Add( m_pbCurrent, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	fgSizer7->Add( m_tbInfo, 0, wxEXPAND, 5 );
	
	this->SetSizer( fgSizer7 );
	this->Layout();


	m_pThread = new UninstallAllThread(flags, user);

	m_pThread->onCompleteEvent += guiDelegate(this, &UDFProgressPanel::onComplete);
	m_pThread->onProgressEvent += guiDelegate(this, &UDFProgressPanel::onProgress);
	m_pThread->onLogEvent += guiDelegate(this, &UDFProgressPanel::onLog);

	m_pThread->start();
}

UDFProgressPanel::~UDFProgressPanel()
{
	safe_delete(m_pThread);
}

void UDFProgressPanel::onProgress(std::pair<uint32, uint32> &prog)
{
	m_pbTotal->setProgress(prog.first);
	m_pbCurrent->setProgress(prog.second);
}

void UDFProgressPanel::onLog(gcString &msg)
{
	m_tbInfo->AppendText(msg);
}

void UDFProgressPanel::onComplete()
{
	m_pbTotal->setProgress(100);
	m_pbCurrent->setProgress(100);

	gcString str("\n\n{0}", Managers::GetString("#DUN_COMPLETED"));
	onLog(str);

	UninstallForm* uf = dynamic_cast<UninstallForm*>(GetParent());

	if (uf)
		uf->completedUninstall();
}