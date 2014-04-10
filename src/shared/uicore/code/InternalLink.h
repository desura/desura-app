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

#ifndef DESURA_INTERNALLINK_H
#define DESURA_INTERNALLINK_H
#ifdef _WIN32
#pragma once
#endif

#include "MainApp.h"
#include "ItemForm.h"
#include "LinkArgs.h"

namespace UserCore
{
	namespace Misc
	{
		class NewsItem;
	}
}

class wxWindow;


class InternalLink
{
public:
	InternalLink(wxWindow *parent);
	~InternalLink();

	void handleInternalLink(const char* link);
	void handleInternalLink(DesuraId id, uint8 action, const LinkArgs &argsList = LinkArgs());
	void closeForm(int32 wxId);

	void closeAll();
	void showNews(const std::vector<std::shared_ptr<UserCore::Misc::NewsItem>> &newsItems, const std::vector<std::shared_ptr<UserCore::Misc::NewsItem>> &giftItems);
	void showAppUpdate(uint32 version);

protected:
	void showPreorderPrompt(DesuraId id, bool isPreload);

	void installedWizard();
	void showSettings(LinkArgs &args);

	void installCheck(DesuraId id);
	void installTestMCF(DesuraId id, LinkArgs args);
	void uploadMCF(DesuraId id);
	void resumeUploadMCF(DesuraId id, LinkArgs args);
	void createMCF(DesuraId id);


	void cleanComplexMod(DesuraId id);
	void installItem(DesuraId id, LinkArgs args);
	void launchItem(DesuraId id, LinkArgs args);
	void verifyItem(DesuraId id, LinkArgs args);
	void uninstallMCF(DesuraId id);

	void updateItem(DesuraId id, LinkArgs args);
	
	void showEULA(DesuraId id);
	void showUpdateLog(DesuraId id);
	void showUpdateLogApp(uint32 version);

	void showGameDisk(DesuraId id, const char* exe, bool cdKey);
	void showUpdateForm(DesuraId id, LinkArgs args);
	void showCDKey(DesuraId id, LinkArgs args);
	void showExeSelect(DesuraId id, bool hasSeenCDKey = false);

	void setPauseItem(DesuraId id, bool state = true);
	void switchBranch(DesuraId id, LinkArgs args);


	void regForm(DesuraId id, gcFrame *form);
	bool checkForm(DesuraId id);

	void onUploadTrigger(ut& info);

	void showPrompt(DesuraId id, LinkArgs args);

	UI::Forms::ItemForm* showItemForm(DesuraId id, UI::Forms::INSTALL_ACTION action, bool showForm, LinkArgs args = LinkArgs());
	UI::Forms::ItemForm* showItemForm(DesuraId id, UI::Forms::INSTALL_ACTION action, MCFBranch branch = MCFBranch(), MCFBuild build = MCFBuild(), bool showForm = true, LinkArgs args = LinkArgs());

	bool processItemLink(bool &badLink, std::vector<gcString> &list, const char* link);
	bool switchTab(bool &badLink, std::vector<gcString> &list, const char* link);

	bool checkForPreorder(DesuraId id);

private:
	int32 m_iNewsFormId;
	int32 m_iGiftFormId;
	wxWindow* m_pParent;

	std::map<uint64, gcFrame*> m_mFormMap;
	std::vector<wxFrame*> m_vSubForms;

	std::map<gcString, gcFrame*> m_mWaitingItemFromMap;
};


#endif //DESURA_INTERNALLINK_H
