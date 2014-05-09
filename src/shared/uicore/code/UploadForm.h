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
	gcString m_szPath;

	DesuraId m_uiInternId;
	gcRefPtr<UserCore::Item::ItemInfoI> m_pItemInfo = nullptr;
	gcRefPtr<UserCore::ItemManagerI> m_pItemManager = nullptr;

	DECLARE_EVENT_TABLE();
};

#endif //DESURA_UPLOADFORM_H
