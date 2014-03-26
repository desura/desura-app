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
#include "ExeSelectForm.h"

#include "MainApp.h"

#include "InternalLink.h"

//""

ExeSelectForm::ExeSelectForm(wxWindow* parent, bool hasSeenCDKey, UserCore::ItemManagerI* pItemManager) 
	: gcFrame(parent, wxID_ANY, "#ES_TITLE", wxDefaultPosition, wxSize( 370,150 ), wxCAPTION|wxCLOSE_BOX|wxSYSTEM_MENU|wxWANTS_CHARS|wxMINIMIZE_BOX)
	, m_pItemManager(pItemManager)
	, m_bHasSeenCDKey(hasSeenCDKey)
{
	if (!m_pItemManager)
		m_pItemManager = GetUserCore()->getItemManager();

	m_labInfo = new wxStaticText(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0 );
	m_pButtonSizer = new wxFlexGridSizer( 8, 2, 0, 0 );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 5, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableRow( 2 );
	fgSizer1->AddGrowableRow( 4 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	fgSizer1->Add( 0, 5, 1, wxEXPAND, 5 );
	fgSizer1->Add( m_labInfo, 0, wxALL, 5 );
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	fgSizer1->Add( m_pButtonSizer, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0 );
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();
	
	centerOnParent();

	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ExeSelectForm::onButtonClick, this);
	Bind(wxEVT_CLOSE_WINDOW, &ExeSelectForm::onFormClose, this);
}

ExeSelectForm::~ExeSelectForm()
{
}

void ExeSelectForm::onFormClose( wxCloseEvent& event )
{
	g_pMainApp->closeForm(this->GetId());
}


void ExeSelectForm::onButtonClick(wxCommandEvent& event)
{
	for (size_t x=0; x<m_vButtonList.size(); x++)
	{
		if (m_vButtonList[x]->GetId() == event.GetId())
		{
			UserCore::Item::ItemInfoI *item = m_pItemManager->findItemInfo(m_Id);

			std::vector<UserCore::Item::Misc::ExeInfoI*> vExeList;
			item->getExeList(vExeList);

			g_pMainApp->handleInternalLink(m_Id, ACTION_LAUNCH, FormatArgs(std::string("exe=") + vExeList[x]->getName(), m_bHasSeenCDKey?"cdkey":""));
			break;
		}
	}

	Close();
}

void ExeSelectForm::setInfo(DesuraId id)
{
	m_Id = id;

	UserCore::Item::ItemInfoI *item = m_pItemManager->findItemInfo(id);

	if (!item)
	{	
		Close();
		return;
	}

	if (item->getIcon() && UTIL::FS::isValidFile(UTIL::FS::PathWithFile(item->getIcon())))
	{
		setIcon(item->getIcon());
	}

	gcWString title(Managers::GetString(L"#ES_TITLE"), item->getName());
	SetTitle(title);

	gcWString text(Managers::GetString(L"#ES_LABEL"), item->getName());
	m_labInfo->SetLabel(text);
	m_labInfo->Wrap(350);

	std::vector<UserCore::Item::Misc::ExeInfoI*> vExeList;
	item->getExeList(vExeList);

	gcButton* def = nullptr;

	for (auto exe : vExeList)
	{
		gcWString name(exe->getName());
		gcButton* but = new gcButton(this, wxID_ANY, name, wxDefaultPosition, wxSize(150, -1));
		m_vButtonList.push_back(but);

		m_pButtonSizer->Add( but, 0, wxALL, 2 );

		if (name.find(L"Play") != std::wstring::npos)
			def = but;
	}

	this->Layout();

	if (def)
		def->SetDefault();
}
