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

#ifndef DESURA_URLMANAGER_H
#define DESURA_URLMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "mcfcore/DownloadProvider.h"
#include "mcfcore/MCFI.h"

#include "util_thread/BaseThread.h"


namespace MCFCore
{
	namespace Misc
	{
		class ProviderInfo;

		//! Provider manager handles all the providers for mcf download including error management
		//!
		class ProviderManager
		{
		public:
			//! Constructor
			//!
			//! @param source Provider source list
			//!
			ProviderManager(std::shared_ptr<Misc::DownloadProvidersI> pDownloadProviders);
			~ProviderManager();

			//! Gets new url for download
			//!
			//! @param id Agent id
			//! @param errCode Reason why requesting new url
			//! @param errMsg Detail error message
			//! @return url if has valid url or "nullptr" if no valid url
			//!
			std::shared_ptr<const MCFCore::Misc::DownloadProvider> requestNewUrl(uint32 id, uint32 errCode, const char* errMsg);

			//! Gets a url for agent
			//!
			//! @param id Agent id
			//! @return url if has valid url or "nullptr" if no valid url
			//!
			std::shared_ptr<const MCFCore::Misc::DownloadProvider> getUrl(uint32 id);

			//! Gets a name for agent
			//!
			//! @param id Agent id
			//! @return name
			//!
			gcString getName(uint32 id);

			//! Removes an Agent from downloading
			//!
			//! @param id Agent id
			//! @param setTimeOut Trigger timeout for that Agents current url
			//!
			void removeAgent(uint32 id, bool setTimeOut = false);

			//! Returns if any workers have valid urls at this stage
			//!
			//! @return True if valid urls, false if not
			//!
			bool hasValidAgents();

			//! Gets the providers list
			//!
			//! @return Provider list
			//!
			std::shared_ptr<Misc::DownloadProvidersI> getDownloadProviders() const
			{
				return m_pDownloadProviders;
			}

			std::shared_ptr<const GetFile_s> getDownloadAuth()
			{
				if (m_pDownloadProviders)
					return m_pDownloadProviders->getDownloadAuth();

				return std::shared_ptr<const GetFile_s>();
			}

			//! Event that gets triggered when using new providers
			//!
			Event<MCFCore::Misc::DP_s> onProviderEvent;

		protected:
			void initProviderList();
			void cleanExpiredProviders();
			bool getValidFreeProviders(std::vector<ProviderInfo*> &vValidProviders);

		private:
			std::shared_ptr<Misc::DownloadProvidersI> m_pDownloadProviders;
			std::vector<ProviderInfo> m_vProviderList;

			std::mutex m_WaitMutex;
		};
	}
}

#endif //DESURA_URLMANAGER_H
