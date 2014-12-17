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

#ifndef DESURA_CDKEYFORM_H
#define DESURA_CDKEYFORM_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcControls.h"

class BasePage;

namespace UserCore
{
	class ItemManagerI;
}

class CDKeyForm : public gcFrame
{
public:
	CDKeyForm(wxWindow* parent, const char* exe, bool launch = false, gcRefPtr<UserCore::ItemManagerI> pItemManager = gcRefPtr<UserCore::ItemManagerI>());
	~CDKeyForm();

	void setInfo(DesuraId id);
	void finish(const char* cdKey);

	DesuraId getItemId();

protected:
	void onFormClose(wxCloseEvent& event);
	void cleanUpPages();

private:
	friend class LanguageTestDialog;

	DesuraId m_ItemId;
	bool m_bLaunch;
	gcString m_szExe;

	BasePage* m_pPage;
	wxBoxSizer* m_bsSizer;

	gcRefPtr<UserCore::ItemManagerI> m_pItemManager;
};

#endif //DESURA_CDKEYFORM_H
