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

#ifndef DESURA_UPLOADFORM_H
#define DESURA_UPLOADFORM_H
#ifdef _WIN32
#pragma once
#endif


#include "wx_controls/gcControls.h"
#include "BasePage.h"

#include "usercore/ItemInfoI.h"
#include "UploadInfoPage.h"
#include "UploadProgPage.h"

///////////////////////////////////////////////////////////////////////////////
/// Class create form
///////////////////////////////////////////////////////////////////////////////
class UploadMCFForm : public gcFrame
{
public:
	UploadMCFForm(wxWindow* parent, gcRefPtr<UserCore::ItemManagerI> pItemManager = nullptr);
	~UploadMCFForm();

	//this is used for a new upload
	void setInfo(DesuraId id);

	//this is used to upload a file from the MCF create process
	void setInfo_path(DesuraId id, const char* path);

	//this is used to resume uploading of a file
	void setInfo_key(DesuraId id, const char* key);

	void setInfo_uid(DesuraId id, const char* uploadId);

	void run();

	void showInfo();
	void showProg(uint32 hash, uint32 start);

	DesuraId getItemId(){return m_uiInternId;}

	void setTrueClose(){m_bTrueClose = true;}

	std::string getKey() const
	{
		return m_szKey;
	}

	std::string getUid() const
	{
		return m_szUid;
	}

protected:
	friend class LanguageTestDialog;

	void setTitle(const char* szItemName, const wchar_t* szFormat = L"#UPLOAD_MCF_TITILE");
	void updateInfo(uint32& itemId);

	BasePage* m_pPage = nullptr;
	wxBoxSizer* m_bsSizer;

	void onFormClose( wxCloseEvent& event );
	void cleanUpPages();

private:

	bool m_bTrueClose = true;

	gcString m_szKey;
	gcString m_szUid;
	gcString m_szPath;

	DesuraId m_uiInternId;
	gcRefPtr<UserCore::Item::ItemInfoI> m_pItemInfo = nullptr;
	gcRefPtr<UserCore::ItemManagerI> m_pItemManager = nullptr;

	DECLARE_EVENT_TABLE();
};

#endif //DESURA_UPLOADFORM_H
