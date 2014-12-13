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
#include "UploadForm.h"
#include "MainApp.h"

BEGIN_EVENT_TABLE( UploadMCFForm, gcFrame )
	EVT_CLOSE( UploadMCFForm::onFormClose )
END_EVENT_TABLE()

UploadMCFForm::UploadMCFForm(wxWindow* parent, gcRefPtr<UserCore::ItemManagerI> pItemManager)
	: gcFrame(parent, wxID_ANY, wxT("Uploading Item"), wxDefaultPosition, wxSize( 370,130 ), wxCAPTION|wxCLOSE_BOX|wxFRAME_FLOAT_ON_PARENT|wxSYSTEM_MENU)
	, m_pItemManager(pItemManager)
{
	if (!m_pItemManager)
		m_pItemManager = GetUserCore()->getItemManager();


	m_bsSizer = new wxBoxSizer( wxVERTICAL );

	this->SetSizer( m_bsSizer );
	this->Layout();

	centerOnParent();
}

UploadMCFForm::~UploadMCFForm()
{
	auto userCore = GetUserCore();

	if (userCore)
		userCore->getItemsAddedEvent() -= guiDelegate(this, &UploadMCFForm::updateInfo);
}

void UploadMCFForm::onFormClose( wxCloseEvent& event )
{
	Show(false);

	if (!event.CanVeto() || m_bTrueClose)
		g_pMainApp->closeForm(this->GetId());
	else
		event.Veto();
}

void UploadMCFForm::updateInfo(uint32& itemId)
{
	m_pItemInfo = m_pItemManager->findItemInfo(m_uiInternId);

	if (m_pItemInfo)
	{
		setTitle(m_pItemInfo->getName());

		if (m_pItemInfo->getIcon())
			setIcon(m_pItemInfo->getIcon());

		if (m_pPage)
			m_pPage->setInfo(m_uiInternId);

		auto userCore = GetUserCore();

		if (userCore)
			userCore->getItemsAddedEvent() -= guiDelegate(this, &UploadMCFForm::updateInfo);
	}
}

void UploadMCFForm::setInfo(DesuraId id)
{
	m_pItemInfo = m_pItemManager->findItemInfo(id);

	if (!m_pItemInfo)
	{
		auto userCore = GetUserCore();

		if (userCore && !userCore->isAdmin())
		{
			Close();
			return;
		}
		else if (userCore)
		{
			userCore->getItemsAddedEvent() += guiDelegate(this, &UploadMCFForm::updateInfo);
			userCore->getItemManager()->retrieveItemInfoAsync(id);
		}
	}

	m_uiInternId = id;

	if (!m_pItemInfo)
	{
		gcString szId("{0}", id.getItem());
		setTitle(szId.c_str(), L"#UPLOAD_MCF_TITILE_ADMIN");
	}
	else
	{
		setTitle(m_pItemInfo->getName());
	}
}

void UploadMCFForm::setTitle(const char* szItemName, const wchar_t* szFormat)
{
	SetTitle(gcWString(Managers::GetString(szFormat), szItemName));
}

void UploadMCFForm::setInfo_path(DesuraId id, const char* path)
{
	setInfo(id);
	m_szPath = gcString(path);
}

void UploadMCFForm::setInfo_key(DesuraId id,  const char* key)
{
	setInfo(id);
	m_szKey = gcString(key);
}

void UploadMCFForm::setInfo_uid(DesuraId id, const char* uploadId)
{
	setInfo(id);

	m_szUid = uploadId;

	uint32 hash = Safe::atoi(uploadId);
	auto pUpload = GetUploadMng()->findItem(hash);

	if (pUpload)
		m_szKey = pUpload->getKey();
}

void UploadMCFForm::showInfo()
{
	cleanUpPages();

	UploadInfoPage *pPage = new UploadInfoPage(this);

	if (!m_szPath.empty() && m_szKey.empty())
	{
		pPage->setInfo_path(m_uiInternId, m_pItemInfo, m_szPath.c_str());
	}
	else if (!m_szKey.empty() && m_szPath.empty())
	{
		pPage->setInfo_key(m_uiInternId, m_pItemInfo, m_szKey.c_str());
	}
	else if (m_szKey.empty() && m_szPath.empty())
	{
		pPage->setInfo(m_uiInternId, m_pItemInfo);
	}
	else
	{
		gcMessageBox(this, wxT("Both path and key are set. Only one should be set."), wxT("Upload MCF Error"));
		Close();
		return;
	}

	m_pPage = pPage;
	m_bsSizer->Add( m_pPage, 1, wxEXPAND, 5 );
	Layout();
	m_pPage->run();
}

void UploadMCFForm::showProg(uint32 hash, uint32 start)
{
	m_bTrueClose = false;

	cleanUpPages();

	UploadProgPage *pPage = new UploadProgPage(this);
	pPage->setInfo(m_uiInternId, m_pItemInfo, hash, start);

	m_pPage = pPage;
	m_bsSizer->Add( m_pPage, 1, wxEXPAND, 5 );
	Layout();
	m_pPage->run();
}

void UploadMCFForm::run()
{
	showInfo();
}

void UploadMCFForm::cleanUpPages()
{
	m_bsSizer->Clear(false);

	if (m_pPage)
	{
		m_pPage->Show(false);
		m_pPage->Close();
		m_pPage->dispose();
		m_pPage->Destroy();
		m_pPage = nullptr;
	}
}
