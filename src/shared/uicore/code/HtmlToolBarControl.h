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

#ifndef DESURA_PAGE_HTML_CONTROL_H
#define DESURA_PAGE_HTML_CONTROL_H
#ifdef _WIN32
#pragma once
#endif

#include "BaseToolBarControl.h"

#include "BreadCrumb.h"


class DispLoading;
class CrumbInfo;

enum
{
	BUTTON_STOP = 1000,
	BUTTON_REFRESH,
	BUTTON_BACK,
	BUTTON_FOWARD,
	BUTTON_LAUNCH,
	BUTTON_HOME,
	BUTTON_LAST,
};


class SearchControl;

class HtmlToolBarControl : public BaseToolBarControl 
{
public:
	HtmlToolBarControl(wxWindow* parent);
	~HtmlToolBarControl();

	void addCrumb(const char* name, const char* url);
	void clearCrumbs();
	const wchar_t* getCrumbUrl(int32 id);

	virtual void onActiveToggle(bool state);

	void focusSearch();

protected:
	void showLoading(bool state);

	//html page progress update
	void onPageStartLoading();
	void onPageFinishLoading();

private:
	gcImageButton* m_butBack;
	gcImageButton* m_butFoward;
	gcImageButton* m_butRefresh;
	gcImageButton* m_butStop;
	gcImageButton* m_butLaunch;
	gcImageButton* m_butHome;

	DispLoading* m_dispLoading;
	BreadCrump* m_pBreadCrumb;

	std::vector<CrumbInfo*> m_vCrumbList;

	wxColor m_NormColor;
	wxColor m_NonActiveColor;

	SearchControl* m_pSearch;
};

#endif //DESURA_PageHtmlCONTROL_H
