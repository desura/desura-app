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

#ifndef DESURA_NEWSFORM_H
#define DESURA_NEWSFORM_H
#ifdef _WIN32
#pragma once
#endif

#include "gcMiscWebControl.h"
#include "usercore/NewsItem.h"
#include "wx_controls/gcControls.h"
#include "wx/wx.h"


typedef gcMiscWebControl NewsBrowser;

class NewsForm : public gcFrame
{
public:
	NewsForm(wxWindow* parent);
	~NewsForm();

	void loadNewsItems(const std::vector<gcRefPtr<UserCore::Misc::NewsItem>> &itemList);
	void setAsGift();

protected:
	NewsBrowser* m_ieBrowser;

	gcButton* m_butPrev;
	gcButton* m_butNext;
	gcButton* m_butClose;

	void onFormClose( wxCloseEvent& event );
	void onButClick( wxCommandEvent& event );
	void loadSelection();

private:
	uint32 m_uiSelected;

	gcWString m_szTitle;
	gcWString m_szLoadingUrl;

	std::vector<gcRefPtr<UserCore::Misc::NewsItem>> m_vItemList;
};

#endif //DESURA_NEWSFORM_H
