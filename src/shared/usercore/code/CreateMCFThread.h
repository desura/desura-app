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
