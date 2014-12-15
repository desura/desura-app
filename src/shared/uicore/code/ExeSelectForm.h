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

#ifndef DESURA_EXESELECTFORM_H
#define DESURA_EXESELECTFORM_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcControls.h"

namespace UserCore
{
	class ItemManagerI;

	namespace Item
	{
		namespace Misc
		{
			class ExeInfoI;
		}
	}
}

class ExeSelectForm : public gcFrame
{
public:
	ExeSelectForm(wxWindow* parent, bool hasSeenCDKey, gcRefPtr<UserCore::ItemManagerI> pItemManager = nullptr);
	~ExeSelectForm();

	DesuraId getItemId(){return m_Id;}
	void setInfo(DesuraId id);

protected:
	wxStaticText* m_labInfo;
	wxFlexGridSizer* m_pButtonSizer;

	void onButtonClick(wxCommandEvent& event);
	void onFormClose(wxCloseEvent& event);

private:
	bool m_bHasSeenCDKey;

	std::vector<gcButton*> m_vButtonList;
	DesuraId m_Id;

	std::vector<gcRefPtr<UserCore::Item::Misc::ExeInfoI>> m_vExeList;
	gcRefPtr<UserCore::ItemManagerI> m_pItemManager;
};


#endif
