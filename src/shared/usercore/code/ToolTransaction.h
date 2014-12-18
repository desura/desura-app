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
		enum class ToolStartRes
		{
			Success,
			Failed,
			NoToolsLeft
		};

		class ToolTransInfo : public gcRefBase
		{
		public:
			ToolTransInfo(bool download, gcRefPtr<Misc::ToolTransaction> transaction, gcRefPtr<ToolManager> pToolManager);
			~ToolTransInfo();

			void removeItem(DesuraId id);

			void onDLComplete(DesuraId id);
			void onDLError(DesuraId id, gcException e);
			void onDLProgress(DesuraId id, UserCore::Misc::ToolProgress &prog);

			void onINComplete();
			void onINError(gcException &e);

			bool isDownload();
			ToolStartRes startNextInstall(std::shared_ptr<IPCToolMain> pToolMain, DesuraId &toolId);


			void getIds(std::vector<DesuraId> &idList);
			void startingIPC();
			void updateTransaction(gcRefPtr<Misc::ToolTransaction> pTransaction);


		protected:
			size_t getIndex(DesuraId id);


		private:
			bool m_bIsDownload = false;
			uint32 m_uiCompleteCount = 0;

			std::vector<UserCore::Misc::ToolProgress> m_vProgress;

			gcRefPtr<Misc::ToolTransaction> m_pTransaction;
			gcRefPtr<ToolManager> m_pToolManager;

			gc_IMPLEMENT_REFCOUNTING(ToolTransInfo)
		};
	}
}

#endif //DESURA_TOOLTRANSACTION_H
