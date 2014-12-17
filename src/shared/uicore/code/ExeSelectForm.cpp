/*
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)
Copyright (C) 2014 Bad Juju Games, Inc.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA.

Contact us at legal@badjuju.com.

*/

#include "Common.h"
#include "ExeSelectForm.h"

#include "MainApp.h"

#include "InternalLink.h"

//""

ExeSelectForm::ExeSelectForm(wxWindow* parent, bool hasSeenCDKey, gcRefPtr<UserCore::ItemManagerI> pItemManager)
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
			g_pMainApp->handleInternalLink(m_Id, ACTION_LAUNCH, FormatArgs(std::string("exe=") + m_vExeList[x]->getName(), m_bHasSeenCDKey ? "cdkey" : ""));
			break;
		}
	}

	Close();
}

void ExeSelectForm::setInfo(DesuraId id)
{
	m_Id = id;

	auto item = m_pItemManager->findItemInfo(id);

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

	item->getExeList(m_vExeList);
	gcButton* def = nullptr;

	for (auto exe : m_vExeList)
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
