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

#ifndef DESURA_MCF_CREATE_H
#define DESURA_MCF_CREATE_H
#ifdef _WIN32
#pragma once
#endif


#include "MCFThread.h"

namespace XML
{
	class gcXMLElement;
}

namespace UserCore
{
	namespace Thread
	{
		class CreateMCFThread : public MCFThread
		{
		public:
			CreateMCFThread(DesuraId id, const char* path);

			//need this here otherwise stop gets called to late
			~CreateMCFThread()
			{
				stop();
			}

			void pauseThread(bool state = true);

		protected:
			void doRun();

			void onPause();
			void onUnpause();
			void onStop();

			void compareBranches(std::vector<gcRefPtr<UserCore::Item::BranchInfo>> &vBranchList);
			void createMcf();

			void waitForItemInfo();

			void retrieveBranchList(std::vector<gcRefPtr<UserCore::Item::BranchInfo>> &outList);

			void processGames(std::vector<gcRefPtr<UserCore::Item::BranchInfo>> &outList, const XML::gcXMLElement &platform);
			void processMods(std::vector<gcRefPtr<UserCore::Item::BranchInfo>> &outList, const XML::gcXMLElement &game);
			void processBranches(std::vector<gcRefPtr<UserCore::Item::BranchInfo>> &outList, const XML::gcXMLElement &item);


		private:
			gcString m_szPath;
			gcString m_szFilePath;

			bool m_bComplete;
			uint32 m_iInternId;
		};
	}
}

#endif //DESURA_MCF_CREATE_H
