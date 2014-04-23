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

#ifndef DESURA_PAGE_HTML_H
#define DESURA_PAGE_HTML_H
#ifdef _WIN32
#pragma once
#endif

#include "BaseTabPage.h"

#ifdef WIN32
#include "gcJSBinding.h"
#endif

#include "MainAppI.h"
#include "gcWebControlI.h"

class HtmlToolBarControl;
class BaseToolBarControl;
class gcWebControlI;

class HtmlTabPage : public BaseTabPage
{
public:
	HtmlTabPage(wxWindow* parent, gcString homePage, PAGE area = NONE);
	~HtmlTabPage();

	void loadUrl(const char* url);
	void goHome();
	void setSelected(bool state);

	virtual std::shared_ptr<BaseToolBarControl> getToolBarControl();

protected:
	virtual void onButtonClicked(int32& id);
	virtual void constuctBrowser();
	virtual void newBrowser(const char* homeUrl);

	void onSearch(gcString &text);
	void onFullSearch(gcString &text);

	void setCurUrl(const char* page);
	void onNewUrl(newURL_s& info);
	void onPageLoad();

	void clearCrumbs();
	void addCrumb(Crumb& curmb);
	int getId();

	void killControlBar();
	void onCIPUpdate();

	gcWebControlI* m_pWebControl = nullptr;
	gcPanel* m_pWebPanel = nullptr;

	std::shared_ptr<HtmlToolBarControl> m_pControlBar;


	friend class MainForm;

	gcString m_szCurUrl;


	virtual void onFind();

private:
	wxBoxSizer* m_pBSBrowserSizer;
	wxBoxSizer* m_bBSSizerControl;

	gcString m_szHomePage;
	PAGE m_SearchArea = PAGE::NONE;
};

#endif //DESURA_PageHtml_H
