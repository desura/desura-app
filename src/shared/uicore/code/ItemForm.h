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

#ifndef DESURA_ITEM_FORM_H
#define DESURA_ITEM_FORM_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcControls.h"
#include "usercore/ItemHelpersI.h"
#include "usercore/ItemInfoI.h"
#include "usercore/ItemHandleI.h"
#include "LinkArgs.h"

class GatherInfoThread;
class GatherInfoHandlerHelper;
class LanguageTestDialog;

typedef std::pair<UserCore::Item::Helper::ACTION, gcString> SIPArg;

namespace UserCore
{
	class ToolManagerI;
}

namespace UI
{
	namespace Forms
	{

		enum class INSTALL_ACTION
		{
			IA_NONE,
			IA_INSTALL,
			IA_INSTALL_CHECK,
			IA_INSTALL_TESTMCF,
			IA_LAUNCH,
			IA_STARTUP_CHECK,
			IA_SWITCH_BRANCH,
			IA_UNINSTALL,
			IA_UPDATE,
			IA_VERIFY,
			IA_CLEANCOMPLEX,
		};

		namespace ItemFormPage
		{
			class BaseInstallPage;
		}


		class ItemFormProxy;

		///////////////////////////////////////////////////////////////////////////////
		/// Class ItemForm
		///////////////////////////////////////////////////////////////////////////////
		class ItemForm : public gcFrame
		{
		public:
			ItemForm(wxWindow* parent, const char* action = nullptr, const char* id = nullptr);
			~ItemForm();

			void finishUninstall(bool complete, bool account);
			void finishInstallCheck();

			void setItemId(DesuraId id);

			void init(INSTALL_ACTION action, MCFBranch branch = MCFBranch(), MCFBuild build = MCFBuild(), bool showForm = true, gcRefPtr<UserCore::Item::ItemHandleI> pItemHandle = gcRefPtr<UserCore::Item::ItemHandleI>());
			void newAction(INSTALL_ACTION action, MCFBranch branch = MCFBranch(), MCFBuild build = MCFBuild(), bool showForm = true);
			void setPaused(bool state = true);

			DesuraId getItemId();

			bool isStopped();

			bool startVerify(bool files, bool tools, bool hooks);
			bool startUninstall(bool complete, bool removeFromAccount);
			bool verifyAfterHashFail();

			void onError(gcException &e);

			bool isInit();

			void pushArgs(const LinkArgs &args);
			void popArgs();

			bool restorePage();

			void setIdealSize(int width, int height) override;

		protected:
			friend class ItemFormProxy;

			Event<bool> onVerifyAfterHashFailEvent;
			void verifyAfterHashFail(bool& res);

			ItemFormPage::BaseInstallPage* m_pPage;
			wxBoxSizer* m_bsSizer;

			void uninstall();

			void getNewPage(UserCore::Item::ITEM_STAGE stage, ItemFormPage::BaseInstallPage* &pPage, gcWString &strTitle);

			gcWString getTitleString(const wchar_t* key);
			void setTitle(const wchar_t* key);
			void onStageChange(UserCore::Item::ITEM_STAGE &stage);

			void onFormClose(wxCloseEvent& event);
			void onModalClose(wxCloseEvent& event);

			void cleanUpPages();
			void cleanUpPage(ItemFormPage::BaseInstallPage* pPage);

			void onItemInfoGathered();

			bool verifyItem();
			bool launchItem();
			bool installTestMcf(MCFBranch branch, MCFBuild build);

			void showUpdatePrompt();
			void showLaunchPrompt();
			void showComplexLaunchPrompt();
			void showEULAPrompt();
			void showPreOrderPrompt();
			void launchError(gcException& e);
			void showParentNoRunPrompt(DesuraId id);

			bool stopStagePrompt();

#ifdef NIX
			virtual void showWinLaunchDialog();
#endif

			gcRefPtr<UserCore::Item::Helper::GatherInfoHandlerHelperI> getGatherInfoHelper();
			gcRefPtr<UserCore::Item::Helper::InstallerHandleHelperI> getInstallHelper();

			void onShowPlatformError(bool& res);
			void onSelectBranch(std::pair<bool, MCFBranch> &info);
			void onShowComplexPrompt(bool &shouldContinue);
			void onShowInstallPrompt(SIPArg &args);
			void onGatherInfoComplete();
			void onShowError(std::pair<bool, uint8> &args);

#ifdef NIX
			void onShowWinLaunchDialog();
			void onShowToolPrompt(std::pair<bool, uint32> &args);
#endif

#ifdef NIX
			EventV onShowWinLaunchDialogEvent;
#endif
			void cleanUpCallbacks();

			friend class ::LanguageTestDialog;
			static void showLaunchError();

		private:
			class HiddenPage
			{
			public:
				ItemFormPage::BaseInstallPage* pPage = nullptr;
				gcWString strTitle;
				wxSize size;
			};


			GatherInfoThread* m_pGIThread;

			INSTALL_ACTION m_iaLastAction;
			gcWString m_szName;
			gcString m_szLaunchExe;

			DesuraId m_ItemId;
			gcRefPtr<UserCore::Item::ItemHandleI> m_pItemHandle;

			MCFBuild m_uiMCFBuild; //this holds the build number if we are processing an un authed item
			MCFBranch m_uiMCFBranch;

			bool m_bIsInit;
			bool m_bInitUninstall;

			wxDialog* m_pDialog;

			gcRefPtr<UserCore::ToolManagerI> m_pToolManager;

			std::vector<LinkArgs> m_vArgs;
			HiddenPage m_HiddenPage;
			gcRefPtr<ItemFormProxy> m_pProxy;
			gcRefPtr<GatherInfoHandlerHelper> m_GIHH;
		};
	}
}

#endif //DESURA_ItemForm_H
