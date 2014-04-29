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


#ifndef DESURA_ITEMHELPERSI_H
#define DESURA_ITEMHELPERSI_H
#ifdef _WIN32
#pragma once
#endif

#include "usercore/GuiDownloadProvider.h"
#include "usercore/ItemInfoI.h"
#include "mcfcore/ProgressInfo.h"
#include "managers/WildcardManager.h"
#include "usercore/VerifyComplete.h"

namespace UserCore
{
	namespace Item
	{
		namespace Helper
		{
			enum TOOL
			{
				JAVA_SUN = 1,
				JAVA = 2,
				MONO = 3,
				AIR = 4,
			};

			enum VALIDATION
			{
				V_OK = 0,
				V_FREESPACE = 1 << 0,
				V_PARENT = 1 << 1,
				V_BADINFO = 1 << 2,
				V_NONEMPTY = 1 << 3,
				V_BADPATH = 1 << 4,
				V_FREESPACE_INS = 1 << 5,
				V_FREESPACE_DL = 1 << 6,

				V_JAVA_SUN = JAVA_SUN << 28,
				V_JAVA = JAVA << 28,
				V_MONO = MONO << 28,
				V_AIR = AIR << 28,
			};

			enum ACTION
			{
				C_NONE,
				C_REMOVE,
				C_INSTALL,
				C_VERIFY,
			};

			class GatherInfoHandlerHelperI
			{
			public:
				virtual void gatherInfoComplete() = 0;

				//! Return false to cancel install
				//!
				virtual bool selectBranch(MCFBranch& branch) = 0;

				//! Show complex warning
				//!
				virtual bool showComplexPrompt() = 0;

				//! Return false to continue with install
				//!
				virtual bool showError(uint8 flags) = 0;

				//! Cant install due to existing files. 
				//!
				virtual ACTION showInstallPrompt(const char* path) = 0;

				//! Destroys this class
				//!
				virtual void destroy() = 0;

				//! Branch belongs to another platform. Return true to select a new branch
				//!
				virtual bool showPlatformError() = 0;

#ifdef NIX
				//! If we are missing a tool show this to ask the user to install it or skip
				//!
				//! @return true to ignore
				//!
				virtual bool showToolPrompt(TOOL tool) = 0;
#endif		

			};

			class InstallerHandleHelperI
			{
			public:
				virtual bool verifyAfterHashFail() = 0;
				virtual void destroy() = 0;
			};

			class ItemHandleFactoryI
			{
			public:
				virtual void getGatherInfoHelper(Helper::GatherInfoHandlerHelperI** helper) = 0;
				virtual void getInstallHelper(Helper::InstallerHandleHelperI** helper) = 0;
			};

			class ItemHandleHelperI
			{
			public:
				virtual void onComplete(uint32 status) = 0;
				virtual void onComplete(gcString& string) = 0;

				virtual void onMcfProgress(MCFCore::Misc::ProgressInfo& info) = 0;
				virtual void onProgressUpdate(uint32 progress) = 0;

				virtual void onError(gcException e) = 0;
				virtual void onNeedWildCard(WCSpecialInfo& info) = 0;

				virtual void onDownloadProvider(UserCore::Misc::GuiDownloadProvider &provider) = 0;
				virtual void onVerifyComplete(UserCore::Misc::VerifyComplete& info) = 0;

				virtual uint32 getId() = 0;
				virtual void setId(uint32 id) = 0;

				virtual void onPause(bool state) = 0;

			protected:
				virtual ~ItemHandleHelperI(){}
			};


			class ItemLaunchHelperI
			{
			public:
				virtual void showUpdatePrompt() = 0;
				virtual void showLaunchPrompt() = 0;
				virtual void showEULAPrompt() = 0;
				virtual void showPreOrderPrompt() = 0;
				virtual void launchError(gcException& e) = 0;
#ifdef NIX
				virtual void showWinLaunchDialog() = 0;
#endif
			};

			class ItemUninstallHelperI
			{
			public:
				//! Should we stop other stages to uninstall this item?
				//!
				virtual bool stopStagePrompt() = 0;
			};

#ifdef LINK_WITH_GMOCK
			class ItemHandleHelperMock : public ItemHandleHelperI
			{
			public:
				MOCK_METHOD1(onComplete, void (uint32) );
				MOCK_METHOD1(onComplete, void (gcString&));
				MOCK_METHOD1(onMcfProgress,  void (MCFCore::Misc::ProgressInfo&));
				MOCK_METHOD1(onProgressUpdate, void (uint32));
				MOCK_METHOD1(onError, void (gcException));
				MOCK_METHOD1(onNeedWildCard, void (WCSpecialInfo&));
				MOCK_METHOD1(onDownloadProvider, void (UserCore::Misc::GuiDownloadProvider &));
				MOCK_METHOD1(onVerifyComplete, void (UserCore::Misc::VerifyComplete&));

				MOCK_METHOD0(getId, uint32 ());
				MOCK_METHOD1(setId, void (uint32));
				MOCK_METHOD1(onPause, void (bool));
			};

			class ItemLaunchHelperMock : public ItemLaunchHelperI
			{
			public:
				MOCK_METHOD0(showUpdatePrompt, void());
				MOCK_METHOD0(showLaunchPrompt, void());
				MOCK_METHOD0(showEULAPrompt, void());
				MOCK_METHOD0(showPreOrderPrompt, void());
				MOCK_METHOD1(launchError, void(gcException&));
#ifdef NIX
				MOCK_METHOD0(showWinLaunchDialog, void());
#endif
			};
#endif
		}
	}
}

#endif //DESURA_ITEMHELPERSI_H
