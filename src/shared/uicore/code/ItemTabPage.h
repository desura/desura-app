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

#ifndef DESURA_ITEMTABPAGE_H
#define DESURA_ITEMTABPAGE_H
#ifdef _WIN32
#pragma once
#endif

#include "HtmlTabPage.h"
#include "usercore/ItemInfoI.h"
#include "usercore/UploadInfo.h"
#include "util/gcOptional.h"

class ItemToolBarControl;
class BaseToolBarControl;

class ItemTabPage : public HtmlTabPage
{
public:
	ItemTabPage(wxWindow* parent, gcWString homePage);
	~ItemTabPage();

	virtual std::shared_ptr<BaseToolBarControl> getToolBarControl() override;
	virtual void reset() override;

protected:
	virtual void onSearch(const wchar_t* value){;}
	virtual void onSearchText(const wchar_t* value);
	virtual void onButtonClicked(int32& id);
	virtual void constuctBrowser();
	virtual void newBrowser(const char* homeUrl);

	void onNewUrl(newURL_s& info) override;

	void onSearchStr(gcString &text);

	void onItemsUpdate();
	void onRecentUpdate(DesuraId &id);
	void onFavoriteUpdate(DesuraId &id);
	void onItemUpdate(UserCore::Item::ItemInfoI::ItemInfo_s& info);

	void onUploadUpdate();
	void onUploadAction(gcString hash);
	void onUploadProgress(gcString hash, UserCore::Misc::UploadInfo& info);

	void onUpdatePoll(std::tuple<gcOptional<bool>, gcOptional<bool>, gcOptional<bool>> &info);
	void onNewItem(DesuraId &id);

	void onLoginItemsLoaded();
	void doneLoading();

	friend class MainForm;

	void onLowDiskSpace(std::pair<bool,char> &info);
	void onShowAlert(const gcString &text, uint32 time=15);

	void postEvent(const char* name, const char* arg1=nullptr, const char* arg2=nullptr);

	virtual void onFind();

	void onPingTimer(wxTimerEvent&);
	void onPing();

private:
	std::mutex m_UploadMutex;
	std::map<std::string, uint32> m_vUploadProgress;

	bool m_bNotifiedOfLowSpace = false;
	std::shared_ptr<ItemToolBarControl> m_pItemControlBar;

	wxTimer m_PingTimer;
	bool m_bPingBack = true;
};

#endif //DESURA_ITEMTABPAGE_H
