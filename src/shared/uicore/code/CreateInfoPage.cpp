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
#include "CreateInfoPage.h"

//something with the wxLog fucks the compile of this up.
#define _WX_LOG_H_

#include "CreateInfoPage.h"
#include "MainApp.h"
#include "MainForm.h"

#include "CreateForm.h"

CreateInfoPage::CreateInfoPage(wxWindow* parent) 
	: BasePage(parent, wxID_ANY, wxDefaultPosition, wxSize( 445,100 ), wxTAB_TRAVERSAL)
{
	Bind(wxEVT_BUTTON, &CreateInfoPage::onButtonClicked, this);
	Bind(wxEVT_TEXT, &CreateInfoPage::onTextChange, this);

	gcTrace("");

	m_tbItemFiles = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);

	m_labText = new gcStaticText(this, wxID_ANY, Managers::GetString(L"#CF_PROMPT"), wxDefaultPosition, wxDefaultSize, 0);
	m_labText->Wrap(-1);

	m_butFile = new gcButton(this, wxID_ANY, Managers::GetString(L"#BROWSE"));
	m_butCreate = new gcButton(this, wxID_ANY, Managers::GetString(L"#OK"), wxDefaultPosition, wxDefaultSize, 0);
	m_butCancel = new gcButton(this, wxID_ANY, Managers::GetString(L"#CANCEL"), wxDefaultPosition, wxDefaultSize, 0);
	
	wxBoxSizer* bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	bSizer5->Add( m_labText, 0, wxALIGN_BOTTOM|wxALL, 5 );

	wxBoxSizer* bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	bSizer8->Add( m_tbItemFiles, 1, wxBOTTOM|wxLEFT, 5 );
	bSizer8->Add( m_butFile, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );

	wxBoxSizer* bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	bSizer2->Add( 0, 0, 1, wxEXPAND, 5 );
	bSizer2->Add( m_butCreate, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	bSizer2->Add( m_butCancel, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer(4, 1, 0, 0);
	fgSizer1->AddGrowableCol(0);
	fgSizer1->AddGrowableRow(2);
	fgSizer1->SetFlexibleDirection(wxBOTH);
	fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	fgSizer1->Add( bSizer5, 0, wxEXPAND, 5 );
	fgSizer1->Add( bSizer8, 0, wxEXPAND, 5 );
	fgSizer1->Add(0, 0, 0, wxEXPAND, 5);
	fgSizer1->Add( bSizer2, 0, wxEXPAND, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();
}

CreateInfoPage::~CreateInfoPage()
{
	dispose();
}

void CreateInfoPage::dispose()
{

}

void CreateInfoPage::onTextChange( wxCommandEvent& event )
{
	validateInput();
}

void CreateInfoPage::onButtonClicked( wxCommandEvent& event )
{
	if (event.GetId() == m_butCreate->GetId())
	{
		gcTrace("But Create");

		CreateMCFForm* temp = dynamic_cast<CreateMCFForm*>(GetParent());

		gcString path((const wchar_t*)m_tbItemFiles->GetValue().c_str());
		temp->showProg(path.c_str());
	}
	else if (event.GetId() == m_butCancel->GetId())
	{
		gcTrace("But Cancel");
		GetParent()->Close();
	}	
	else if (event.GetId() == m_butFile->GetId())
	{
		gcTrace("But Browse");
		showDialog();
	}
}

void CreateInfoPage::showDialog()
{
	wxString curPath = m_tbItemFiles->GetValue();
    wxDirDialog *p = new wxDirDialog(this, Managers::GetString(L"#CF_DIRPROMPT"), curPath, wxDIRP_DIR_MUST_EXIST );

    if (p->ShowModal() == wxID_OK)
    {
        m_tbItemFiles->SetValue(p->GetPath());
    }

    wxDELETE(p);
}

void CreateInfoPage::validateInput()
{
	if (!m_tbItemFiles)
		return;

	bool state = true;

	if (!validatePath(m_tbItemFiles, TYPE_FOLDER))
		state = false;

	m_butCreate->Enable( state );
};

bool CreateInfoPage::validatePath(wxTextCtrl* ctrl, bool type)
{
	bool doesExsist = false;

	gcString dir((const wchar_t*)ctrl->GetValue().c_str());

	if (type == TYPE_FOLDER)
		doesExsist = UTIL::FS::isValidFolder(dir);
	else
		doesExsist = UTIL::FS::isValidFile(dir);

	if (doesExsist)
		ctrl->SetForegroundColour( *wxBLACK );
	else
		ctrl->SetForegroundColour( *wxRED );

	ctrl->Refresh();
	return doesExsist;
}

void CreateInfoPage::resetAllValues()
{
	//m_dpFiles->SetPath(wxT(""));
	m_butCreate->Enable( false );
}

void CreateInfoPage::setInfo(DesuraId id, UserCore::Item::ItemInfoI* pItemInfo)
{
	if (!pItemInfo && GetUserCore() && !GetUserCore()->isAdmin())
	{	
		GetParent()->Close();
		return;
	}

	BasePage::setInfo(id, pItemInfo);
}
