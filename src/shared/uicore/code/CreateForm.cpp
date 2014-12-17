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
#include "CreateForm.h"

#include "CreateInfoPage.h"
#include "CreateProgPage.h"
#include "CreateOVPage.h"


BEGIN_EVENT_TABLE( CreateMCFForm, gcFrame )
	EVT_CLOSE( CreateMCFForm::onFormClose )
END_EVENT_TABLE()

CreateMCFForm::CreateMCFForm(wxWindow* parent, gcRefPtr<UserCore::ItemManagerI> pItemManager)
	: gcFrame(parent, wxID_ANY, wxT("Creating MCF"), wxDefaultPosition, wxSize( 415,120 ), wxCAPTION|wxCLOSE_BOX|wxFRAME_FLOAT_ON_PARENT|wxSYSTEM_MENU)
	, m_bsSizer(new wxBoxSizer(wxVERTICAL))
	, m_pItemManager(pItemManager)
{
	gcTrace("");

	if (!m_pItemManager)
		m_pItemManager = GetUserCore()->getItemManager();

	this->SetSizer( m_bsSizer );
	this->Layout();

	centerOnParent();
}

CreateMCFForm::~CreateMCFForm()
{
	auto userCore = GetUserCore();

	if (userCore)
		userCore->getItemsAddedEvent() -= guiDelegate(this, &CreateMCFForm::updateInfo);
}

void CreateMCFForm::onFormClose(wxCloseEvent& event)
{
	gcTrace("");

	if (m_bPromptClose)
	{
		if (event.CanVeto() && gcMessageBox(this, Managers::GetString(L"#CONFIRM_PROMPT"), Managers::GetString(L"#CONFIRM"), wxICON_QUESTION | wxYES_NO) != wxYES)
		{
			event.Veto();
			return;
		}
	}

	g_pMainApp->closeForm(this->GetId());
}


void CreateMCFForm::updateInfo(uint32& count)
{
	gcTrace("");

	auto item = m_pItemManager->findItemInfo(m_uiInternId);

	if (item)
	{
		setTitle(item->getName());

		if (item->getIcon())
			setIcon(item->getIcon());

		if (m_pPage)
			m_pPage->setInfo(m_uiInternId, item);

		auto userCore = GetUserCore();

		if (userCore)
			userCore->getItemsAddedEvent() -= guiDelegate(this, &CreateMCFForm::updateInfo);
	}
}

void CreateMCFForm::setInfo(DesuraId id)
{
	gcTrace("Id: {0}", id);

	auto item = m_pItemManager->findItemInfo(id);

	if (!item)
	{
		if (GetUserCore() && !GetUserCore()->isAdmin())
		{
			Close();
			return;
		}
		else
		{
			auto userCore = GetUserCore();

			if (userCore)
				userCore->getItemsAddedEvent() += guiDelegate(this, &CreateMCFForm::updateInfo);

			m_pItemManager->retrieveItemInfoAsync(id);
		}
	}

	m_uiInternId = id;

	if (!item)
	{
		gcString szId("{0}", id.getItem());
		setTitle(szId.c_str(), L"#CREATE_MCF_TITILE_ADMIN");
	}
	else
	{
		setTitle(item->getName());
	}
}

void CreateMCFForm::setTitle(const char* szItemName, const wchar_t* szFormat)
{
	SetTitle(gcWString(Managers::GetString(szFormat), szItemName));
}

void CreateMCFForm::showInfo()
{
	gcTrace("");

	cleanUpPages();

	CreateInfoPage *pPage = new CreateInfoPage(this);
	pPage->setInfo(m_uiInternId, m_pItemManager->findItemInfo(m_uiInternId));

	m_bPromptClose = false;

	m_pPage = pPage;
	m_bsSizer->Add( m_pPage, 1, wxEXPAND, 5 );
	Layout();
	m_pPage->run();
}

void CreateMCFForm::showProg(const char* path)
{
	gcTrace("Path: {0}", path);

	cleanUpPages();

	CreateProgPage* pPage = new CreateProgPage(this);
	pPage->setInfo(m_uiInternId, m_pItemManager->findItemInfo(m_uiInternId), path);

	m_bPromptClose = true;

	m_pPage = pPage;
	m_bsSizer->Add( m_pPage, 1, wxEXPAND, 5 );
	Layout();
	m_pPage->run();
}

void CreateMCFForm::showOverView(const char* path)
{
	gcTrace("Path: {0}", path);

	cleanUpPages();

	CreateMCFOverview* pPage = new CreateMCFOverview(this);
	pPage->setInfo(m_uiInternId, m_pItemManager->findItemInfo(m_uiInternId), path);
	pPage->onUploadTriggerEvent += delegate(&onUploadTriggerEvent);
	m_bPromptClose = false;

	m_pPage = pPage;
	m_bsSizer->Add( m_pPage, 1, wxEXPAND, 5 );

	Layout();
}

void CreateMCFForm::run()
{
	showInfo();
}

void CreateMCFForm::cleanUpPages()
{
	m_bsSizer->Clear(false);

	if (!m_pPage)
		return;

	CreateMCFOverview* temp = dynamic_cast<CreateMCFOverview*>(m_pPage);
	if (temp)
		temp->onUploadTriggerEvent -= delegate(&onUploadTriggerEvent);

	m_pPage->Show(false);
	m_pPage->Close();
	m_pPage->dispose();
	m_pPage->Destroy();
	m_pPage = nullptr;
}
