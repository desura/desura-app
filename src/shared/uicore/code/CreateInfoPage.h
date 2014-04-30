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

#ifndef DESURA_CREATEINFOPAGE_H
#define DESURA_CREATEINFOPAGE_H
#ifdef _WIN32
#pragma once
#endif

#include "BasePage.h"
#include "wx_controls/gcControls.h"

#include "wx/wx.h"
#include <wx/filepicker.h>

#include "Managers.h"
#include "usercore/ItemInfoI.h"


///////////////////////////////////////////////////////////////////////////////
/// Class CreateMCF_Form
///////////////////////////////////////////////////////////////////////////////
class CreateInfoPage : public BasePage 
{
public:
	CreateInfoPage(wxWindow* parent);
	~CreateInfoPage();

	void dispose() override;

	void setInfo(DesuraId id, UserCore::Item::ItemInfoI* pItemInfo) override;
	void run() override {}
	
protected:
	gcStaticText* m_labText;

	wxTextCtrl* m_tbItemFiles = nullptr;
	
	gcButton* m_butCreate;
	gcButton* m_butCancel;
	gcButton* m_butFile;
	
	bool validatePath(wxTextCtrl* ctrl, bool type);

	void onButtonClicked( wxCommandEvent& event );
	void onTextChange( wxCommandEvent& event );

	void resetAllValues();
	void validateInput();

	void showDialog();
};


#endif //DESURA_CREATEINFOPAGE_H
