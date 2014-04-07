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

#ifndef DESURA_TOOLTRANSACTION_H
#define DESURA_TOOLTRANSACTION_H
#ifdef _WIN32
#pragma once
#endif

class IPCToolMain;

#include "ToolManager.h"

namespace UserCore
{
	namespace Misc
	{
		class ToolTransInfo
		{
		public:
			ToolTransInfo(bool download, ToolTransaction* transaction, ToolManager* pToolManager);
			~ToolTransInfo();

			void removeItem(DesuraId id);

			void onDLComplete(DesuraId id);
			void onDLError(DesuraId id, gcException e);
			void onDLProgress(DesuraId id, UserCore::Misc::ToolProgress &prog);

			void onINComplete();
			void onINError(gcException &e);

			bool isDownload();
			bool startNextInstall(IPCToolMain* pToolMain, DesuraId &toolId);


			void getIds(std::vector<DesuraId> &idList);
			void startingIPC();
			void updateTransaction(Misc::ToolTransaction* pTransaction);


		protected:
			size_t getIndex(DesuraId id);


		private:
			bool m_bIsDownload = false;
			uint32 m_uiCompleteCount = 0;

			std::shared_ptr<Misc::ToolTransaction> m_pTransaction;
			std::vector<UserCore::Misc::ToolProgress> m_vProgress;

			ToolManager* m_pToolManager = nullptr;
		};
	}
}

#endif //DESURA_TOOLTRANSACTION_H
