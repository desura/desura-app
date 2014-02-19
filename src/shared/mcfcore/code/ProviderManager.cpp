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

#include "Common.h"
#include "ProviderManager.h"
#include "util/gcTime.h"

namespace MCFCore
{
namespace Misc
{

const uint32 g_uiTimeOutDelay = 15;

class ErrorInfo
{
public:
	void setTimeOut()
	{
		m_iOwner = -1;
		m_tExpTime = gcTime() + std::chrono::seconds( getTimeOut() );
		m_uiErrCount++;
	}

	bool isInTimeOut()
	{
		return m_tExpTime > gcTime();
	}

	void setOwner(uint32 id)
	{
		m_iOwner = id;
	}

	uint32 getOwner()
	{
		return m_iOwner;
	}

protected:
	uint32 getTimeOut()
	{
		return (g_uiTimeOutDelay * m_uiErrCount);
	}

private:
	uint32 m_uiErrCount = 0;
	uint32 m_iOwner = -1;
	gcTime m_tExpTime;
};


ProviderManager::ProviderManager(std::vector<std::shared_ptr<const MCFCore::Misc::DownloadProvider>> &source) 
	: m_vSourceList(source)
{
	for (auto i : m_vSourceList)
		m_vErrorList.push_back(std::make_shared<ErrorInfo>());
}

ProviderManager::~ProviderManager()
{
}

gcString ProviderManager::getUrl(uint32 id, DownloadProviderType &eType)
{
	gcString url("nullptr");
	std::vector<uint32> validList;

	m_WaitMutex.lock();

	for (size_t x=0; x<m_vErrorList.size(); x++)
	{
		if (!m_vErrorList[x]->isInTimeOut() && m_vErrorList[x]->getOwner() == UINT_MAX)
			validList.push_back(x);
	}

	//always take from the top of the list as they are prefered servers
	if (validList.size() > 0)
	{
		m_vErrorList[validList[0]]->setOwner(id);

		url = gcString(m_vSourceList[validList[0]]->getUrl());
		eType = m_vSourceList[validList[0]]->getType();

		MCFCore::Misc::DP_s dp;
		dp.action = MCFCore::Misc::DownloadProvider::ADD;
		dp.provider = m_vSourceList[validList[0]];
		onProviderEvent(dp);
	}

	m_WaitMutex.unlock();

	return url;
}

gcString ProviderManager::getName(uint32 id)
{
	gcString name;

	for (size_t x=0; x<m_vSourceList.size(); x++)
	{
		if (!m_vSourceList[x])
			continue;

		if (id == m_vErrorList[x]->getOwner())
		{
			name = gcString(m_vSourceList[x]->getName());
		}
	}

	return name;
}

gcString ProviderManager::requestNewUrl(uint32 id, DownloadProviderType &eType, uint32 errCode, const char* errMsg)
{
	Warning(gcString("Mcf download thread errored out. Id: {0}, Error: {2} [{1}]\n", id, errCode, errMsg));

	removeAgent(id, true);
	return getUrl(id, eType);
}

void ProviderManager::removeAgent(uint32 id, bool setTimeOut)
{
	m_WaitMutex.lock();

	for (size_t x=0; x<m_vSourceList.size(); x++)
	{
		if (!m_vSourceList[x])
			continue;

		if (id == m_vErrorList[x]->getOwner())
		{
			if (setTimeOut)
				m_vErrorList[x]->setTimeOut();
			else
				m_vErrorList[x]->setOwner(-1);

			MCFCore::Misc::DP_s dp;
			dp.action = MCFCore::Misc::DownloadProvider::REMOVE;
			dp.provider = m_vSourceList[x];
			onProviderEvent(dp);
		}
	}

	m_WaitMutex.unlock();
}

bool ProviderManager::hasValidAgents()
{
	bool res = false;

	m_WaitMutex.lock();

	for (size_t x=0; x<m_vSourceList.size(); x++)
	{
		if (!m_vSourceList[x])
			continue;

		if (!m_vErrorList[x]->isInTimeOut())
		{
			res = true;
			break;
		}
	}

	m_WaitMutex.unlock();

	return res;
}

}
}
