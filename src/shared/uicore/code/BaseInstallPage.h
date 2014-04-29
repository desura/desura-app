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

#ifndef DESURA_BASEINSTALLPAGE_H
#define DESURA_BASEINSTALLPAGE_H
#ifdef _WIN32
#pragma once
#endif

#include "BasePage.h"
#include "usercore/ItemHandleI.h"

class ItemHandleHelper;

namespace UI
{
	namespace Forms
	{
		namespace ItemFormPage
		{

			class BaseInstallPage : public ::BasePage
			{
			public:
				BaseInstallPage(wxWindow* parent);
				~BaseInstallPage();

				virtual void init() = 0;

				virtual void onComplete(uint32& status);
				virtual void onProgressUpdate(uint32& progress);
				virtual void onError(gcException& e);

				virtual void onMcfProgress(MCFCore::Misc::ProgressInfo& info);
				virtual void onComplete(gcString& str);
				virtual void onDownloadProvider(UserCore::Misc::GuiDownloadProvider& provider);

				virtual void onItemUpdate(UserCore::Item::ItemInfoI::ItemInfo_s& info);
				virtual void onVerifyComplete(UserCore::Misc::VerifyComplete& info);
				virtual void onPause(bool &state);

				void setInfo(DesuraId id, UserCore::Item::ItemInfoI* pItemInfo);
				void setInfo(UserCore::Item::ItemHandleI* pItemHandle);

				void pause(bool state);
				void nonBlockStop();
				void stop();

				void registerHandle();
				void deregisterHandle();

			protected:
				UserCore::Item::ItemHandleI* getItemHandle()
				{
					return m_pItemHandle;
				};

				void run() override
				{
				}

				void dispose() override
				{
				}

			private:
				std::unique_ptr<ItemHandleHelper> m_pIHH;
				UserCore::Item::ItemHandleI* m_pItemHandle = nullptr;
			};

		}
	}
}


#endif //DESURA_BASEINSTALLPAGE_H
