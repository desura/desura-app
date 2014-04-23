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

	void loadNewsItems(const std::vector<std::shared_ptr<UserCore::Misc::NewsItem>> &itemList);
	void loadNewsItems(const std::vector<UserCore::Misc::NewsItem> &itemList);

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

	std::vector<UserCore::Misc::NewsItem> m_vItemList;
};

#endif //DESURA_NEWSFORM_H
