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

class ErrorInfo;

//! Provider manager handles all the providers for mcf download including error management
//!
class ProviderManager
{
public:
	//! Constructor
	//!
	//! @param source Provider source list
	//!
	ProviderManager(std::vector<std::shared_ptr<const MCFCore::Misc::DownloadProvider>> &source);
	~ProviderManager();

	//! Gets new url for download
	//!
	//! @param id Agent id
	//! @param errCode Reason why requesting new url
	//! @param errMsg Detail error message
	//! @return url if has valid url or "nullptr" if no valid url
	//!
	gcString requestNewUrl(uint32 id, uint32 errCode, const char* errMsg);

	//! Gets a url for agent
	//!
	//! @param id Agent id
	//! @return url if has valid url or "nullptr" if no valid url
	//!
	gcString getUrl(uint32 id);

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
	std::vector<std::shared_ptr<const MCFCore::Misc::DownloadProvider>>& getVector()
	{
		return m_vSourceList;
	}

	//! Event that gets triggered when using new providers
	//!
	Event<MCFCore::Misc::DP_s> onProviderEvent;

private:
	std::vector<std::shared_ptr<const MCFCore::Misc::DownloadProvider>> &m_vSourceList;
	std::vector<std::shared_ptr<ErrorInfo>> m_vErrorList;

	std::mutex m_WaitMutex;
};

}
}

#endif //DESURA_URLMANAGER_H
