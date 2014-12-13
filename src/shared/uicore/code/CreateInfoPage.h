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

	void setInfo(DesuraId id, gcRefPtr<UserCore::Item::ItemInfoI> pItemInfo) override;
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
