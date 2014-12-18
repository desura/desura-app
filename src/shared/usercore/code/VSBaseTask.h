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

#ifndef DESURA_VSBASETASK_H
#define DESURA_VSBASETASK_H
#ifdef _WIN32
#pragma once
#endif

#include "ItemHandle.h"
#include "mcfcore/MCFMain.h"
#include "mcfcore/MCFI.h"

namespace WebCore
{
	class WebCoreI;
}

namespace UserCore
{
	class UserI;

	namespace Item
	{
		class ItemHandle;
		class ItemInfo;
	}

	namespace ItemTask
	{
		class VSBaseTask
		{
		public:
			VSBaseTask();

			enum RESULT
			{
				RES_NONE,
				RES_COMPLETE,
				RES_DOWNLOAD,
				RES_INSTALL,
				RES_DOWNLOADMISSING,
				RES_STOPPED,
			};

			RESULT getResult();
			void stop();

			Event<gcException> onErrorEvent;

			void setWebCore(gcRefPtr<WebCore::WebCoreI> wc);
			void setUserCore(gcRefPtr<UserCore::UserI> uc);

			void setItemHandle(gcRefPtr<UserCore::Item::ItemHandleI> handle);
			void setMcfBuild(MCFBuild build);
			void setMcfBranch(MCFBranch branch);

			void setMcfHandle(MCFCore::MCFI* handle);

			virtual void onProgress(MCFCore::Misc::ProgressInfo& prog){}

		protected:
			DesuraId getItemId();

			gcRefPtr<UserCore::Item::ItemHandleI> getItemHandle();
			gcRefPtr<UserCore::Item::ItemInfoI> getItemInfo();
			gcRefPtr<UserCore::Item::ItemInfoI> getParentItemInfo();

			gcRefPtr<WebCore::WebCoreI> getWebCore();
			gcRefPtr<UserCore::UserI> getUserCore();

			MCFBuild getMcfBuild();
			MCFBranch getMcfBranch();

			bool isStopped();

			virtual void onStop(){ ; }

			void setResult(RESULT res);
			bool loadMcf(gcString mcfPath);

			MCFCore::MCFI* m_hMcf;

		private:
			volatile bool m_bIsStopped;

			gcRefPtr<UserCore::Item::ItemHandleI> m_pHandle;
			gcRefPtr<WebCore::WebCoreI> m_pWebCore;
			gcRefPtr<UserCore::UserI> m_pUserCore;

			MCFBranch m_uiMcfBranch;
			MCFBuild m_uiMcfBuild;

			RESULT m_Result;
		};
	}
}

#endif //DESURA_VSBASETASK_H
