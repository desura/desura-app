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
#include "UploadForm.h"
#include "MainApp.h"

BEGIN_EVENT_TABLE( UploadMCFForm, gcFrame )
	EVT_CLOSE( UploadMCFForm::onFormClose )
END_EVENT_TABLE()

UploadMCFForm::UploadMCFForm(wxWindow* parent, UserCore::ItemManagerI* pItemManager) 
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
	if (GetUserCore())
		*GetUserCore()->getItemsAddedEvent() -= guiDelegate(this, &UploadMCFForm::updateInfo);
}

void UploadMCFForm::onFormClose( wxCloseEvent& event )
{
	Show(false);

	if (m_bTrueClose)	
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

		if (GetUserCore())
			*GetUserCore()->getItemsAddedEvent() -= guiDelegate(this, &UploadMCFForm::updateInfo);
	}
}

void UploadMCFForm::setInfo(DesuraId id)
{
	m_pItemInfo = m_pItemManager->findItemInfo(id);

	if (!m_pItemInfo)
	{	
		if (GetUserCore() && !GetUserCore()->isAdmin())
		{
			Close();
			return;
		}
		else if (GetUserCore())
		{
			*GetUserCore()->getItemsAddedEvent() += guiDelegate(this, &UploadMCFForm::updateInfo);
			GetUserCore()->getItemManager()->retrieveItemInfoAsync(id);
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