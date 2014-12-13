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

#ifndef DESURA_MODWIZARDTHREAD_H
#define DESURA_MODWIZARDTHREAD_H
#ifdef _WIN32
#pragma once
#endif

namespace XML
{
	class gcXMLElement;
}

namespace UserCore
{
	namespace Misc
	{
		class InstallInfo;
	}
}

class WildcardManager;

#include "MCFThread.h"

namespace UserCore
{
	namespace Item
	{
		class ItemTaskGroup;
	}

	namespace Thread
	{
		//! Installed wizard looks for items already installed on user computer
		//!
		class InstalledWizardThread : public MCFThread
		{
		public:
			InstalledWizardThread();
			virtual ~InstalledWizardThread();

		protected:
			void doRun();

			void parseItemsQuick(const XML::gcXMLElement &fNode);

			void parseItems1(const XML::gcXMLElement &fNode, gcRefPtr<WildcardManager> &pWildCard, std::map<uint64, XML::gcXMLElement> *vMap = nullptr);
			void parseItems2(const XML::gcXMLElement &fNode, gcRefPtr<WildcardManager> &pWildCard);

			bool selectBranch(gcRefPtr<UserCore::Item::ItemInfoI> &item);
			void onItemFound(gcRefPtr<UserCore::Item::ItemInfoI> &item);

			void onGameFound(UserCore::Misc::InstallInfo &game);
			void onModFound(UserCore::Misc::InstallInfo &mod);

			std::vector<UserCore::Misc::InstallInfo> m_vGameList;

			bool m_bTriggerNewEvent;


			void triggerProgress();

			void parseGameQuick(const XML::gcXMLElement &game);

			void parseGame(DesuraId id, const XML::gcXMLElement &game, gcRefPtr<WildcardManager> &pWildCard, const XML::gcXMLElement &info);
			void parseMod(DesuraId parId, DesuraId id, const XML::gcXMLElement &mod, gcRefPtr<WildcardManager> &pWildCard, const XML::gcXMLElement &info);

			void cleanup() override
			{
				m_pTaskGroup.reset();
				MCFThread::cleanup();
			}

		private:
			gcRefPtr<UserCore::Item::ItemTaskGroup> m_pTaskGroup;

			uint32 m_uiDone;
			uint32 m_uiTotal;

			gcString m_szDbName;
		};
	}
}

#endif //DESURA_MODWIZARDTHREAD_H
