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

using namespace MCFCore::Misc;

const uint32 g_uiTimeOutDelay = 15;

namespace MCFCore
{
	namespace Misc
	{

		class ProviderInfo
		{
		public:
			ProviderInfo(std::shared_ptr<const MCFCore::Misc::DownloadProvider> &pProvider)
				: m_pProvider(pProvider)
				, m_tExpTime(gcTime() - std::chrono::seconds(getTimeOut()))
			{
			}

			void setTimeOut()
			{
				m_iOwner = -1;
				m_uiErrCount++;
				m_tExpTime = gcTime() + std::chrono::seconds(getTimeOut());
				gcAssert(isInTimeOut());
			}

			bool isInTimeOut()
			{
				return gcTime() < m_tExpTime;
			}

			void setOwner(uint32 id)
			{
				m_iOwner = id;
			}

			uint32 getOwner()
			{
				return m_iOwner;
			}

			std::shared_ptr<const MCFCore::Misc::DownloadProvider> getProvider()
			{
				return m_pProvider;
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
			std::shared_ptr<const MCFCore::Misc::DownloadProvider> m_pProvider;
		};

	}
}


ProviderManager::ProviderManager(std::shared_ptr<MCFCore::Misc::DownloadProvidersI> pDownloadProviders) 
	: m_pDownloadProviders(pDownloadProviders)
{
}

ProviderManager::~ProviderManager()
{
}

void ProviderManager::initProviderList()
{
	std::vector<std::shared_ptr<const DownloadProvider>> vProviders;
	m_pDownloadProviders->getDownloadProviders(vProviders);

	for (auto p : vProviders)
	{
		auto bFound = false;

		for (auto x : m_vProviderList)
		{
			if (gcString(p->getUrl()) == x.getProvider()->getUrl())
			{
				bFound = true;
				break;
			}
		}

		if (!bFound && p->isValidAndNotExpired())
			m_vProviderList.push_back(ProviderInfo(p));
	}
}

void ProviderManager::cleanExpiredProviders()
{
	auto it = std::remove_if(m_vProviderList.begin(), m_vProviderList.end(), [](ProviderInfo &p)
	{
		return !p.getProvider()->isValidAndNotExpired();
	});

	if (it != m_vProviderList.end())
		m_vProviderList.erase(it, m_vProviderList.end());
}

bool ProviderManager::getValidFreeProviders(std::vector<ProviderInfo*> &vValidProviders)
{
	for (auto &p : m_vProviderList)
	{
		if (!p.isInTimeOut() && p.getOwner() == UINT_MAX)
			vValidProviders.push_back(&p);
	}

	return !vValidProviders.empty();
}

std::shared_ptr<const MCFCore::Misc::DownloadProvider> ProviderManager::getUrl(uint32 id)
{
	std::shared_ptr<const MCFCore::Misc::DownloadProvider> pProvider;
	auto bInitList = false;

	{
		std::lock_guard<std::mutex> guard(m_WaitMutex);

		cleanExpiredProviders();

		for (auto p : m_vProviderList)
		{
			if (id == p.getOwner())
				return p.getProvider();
		}

		if (m_vProviderList.size() == 0)
		{
			initProviderList();
			bInitList = true;
		}

		std::vector<ProviderInfo*> vValidProviders;

		if (!getValidFreeProviders(vValidProviders))
		{
			//recheck to see if we can get some more providers
			if (bInitList)
				return pProvider;

			initProviderList();

			if (!getValidFreeProviders(vValidProviders))
				return pProvider;
		}
			
		//always take from the top of the list as they are preferred servers
		auto first = *vValidProviders.begin();

		first->setOwner(id);
		pProvider = first->getProvider();
	}

	MCFCore::Misc::DP_s dp;

	dp.action = MCFCore::Misc::DownloadProvider::ADD;
	dp.provider = pProvider;

	onProviderEvent(dp);

	return pProvider;
}

gcString ProviderManager::getName(uint32 id)
{
	std::lock_guard<std::mutex> guard(m_WaitMutex);

	for (auto p : m_vProviderList)
	{
		if (id == p.getOwner())
			return p.getProvider()->getName();
	}

	return "";
}

std::shared_ptr<const MCFCore::Misc::DownloadProvider> ProviderManager::requestNewUrl(uint32 id, uint32 errCode, const char* errMsg)
{
	Warning("Mcf download thread errored out. Id: {0}, Error: {2} [{1}]\n", id, errCode, errMsg);

	removeAgent(id, true);
	return getUrl(id);
}

void ProviderManager::removeAgent(uint32 id, bool setTimeOut)
{
	gcTrace("TimeOut: {0}", setTimeOut);

	std::shared_ptr<const MCFCore::Misc::DownloadProvider> pProvider;

	{
		std::lock_guard<std::mutex> guard(m_WaitMutex);

		for (auto &p : m_vProviderList)
		{
			if (id != p.getOwner())
				continue;

			if (setTimeOut)
				p.setTimeOut();
			
			p.setOwner(-1);
			pProvider = p.getProvider();
			break;
		}
	}

	if (pProvider)
	{
		MCFCore::Misc::DP_s dp;

		dp.action = MCFCore::Misc::DownloadProvider::REMOVE;
		dp.provider = pProvider;

		onProviderEvent(dp);
	}
}

bool ProviderManager::hasValidAgents()
{
	std::lock_guard<std::mutex> guard(m_WaitMutex);

	if (m_vProviderList.size() == 0)
		initProviderList();

	for (auto &p : m_vProviderList)
	{
		if (!p.isInTimeOut())
			return true;
	}

	return false;
}



#ifdef WITH_GTEST

#include <gtest/gtest.h>

namespace UnitTest
{

	class TestDownloadProvidersPM : public DownloadProvidersI
	{
	public:
		bool getDownloadProviders(std::vector<std::shared_ptr<const DownloadProvider>> &vDownloadProviders) override
		{
			vDownloadProviders = m_vDownloadProviders;
			return !m_vDownloadProviders.empty();
		}

		void setInfo(DesuraId id, MCFBranch branch, MCFBuild build) override
		{

		}

		std::shared_ptr<const GetFile_s> getDownloadAuth() override
		{
			return std::shared_ptr<const GetFile_s>();
		}

		size_t size() override
		{
			return m_vDownloadProviders.size();
		}

		std::vector<std::shared_ptr<const DownloadProvider>> m_vDownloadProviders;
	};


	//test getUrl gives valid download provider
	TEST(ProviderManager, BasicOneRequest)
	{
		auto dp = std::make_shared<TestDownloadProvidersPM>();
		dp->m_vDownloadProviders.push_back(std::make_shared<const DownloadProvider>("a1","a2","a3","a4"));

		ProviderManager pm(dp);

		{
			auto res = pm.getUrl(1);

			ASSERT_TRUE(!!res.get());
			ASSERT_STREQ("a1", res->getName());
		}

		//Getting it a second time should return same provider
		{
			auto res = pm.getUrl(1);

			ASSERT_TRUE(!!res.get());
			ASSERT_STREQ("a1", res->getName());
		}
	}


	//test getUrl gives unique download provider each time and in order
	TEST(ProviderManager, BasicTwoRequests)
	{
		auto dp = std::make_shared<TestDownloadProvidersPM>();
		dp->m_vDownloadProviders.push_back(std::make_shared<const DownloadProvider>("a1", "a2", "a3", "a4"));
		dp->m_vDownloadProviders.push_back(std::make_shared<const DownloadProvider>("b1", "b2", "b3", "b4"));

		ProviderManager pm(dp);

		{
			auto res = pm.getUrl(1);

			ASSERT_TRUE(!!res.get());
			ASSERT_STREQ("a1", res->getName());
		}

		{
			auto res = pm.getUrl(2);

			ASSERT_TRUE(!!res.get());
			ASSERT_STREQ("b1", res->getName());
		}
	}

	//test requestNewUrl gives new download provider and doest reallocate old provider
	TEST(ProviderManager, BasicTwoRequestsWithError)
	{
		auto dp = std::make_shared<TestDownloadProvidersPM>();
		dp->m_vDownloadProviders.push_back(std::make_shared<const DownloadProvider>("a1", "a2", "a3", "a4"));
		dp->m_vDownloadProviders.push_back(std::make_shared<const DownloadProvider>("b1", "b2", "b3", "b4"));

		ProviderManager pm(dp);

		{
			auto res = pm.getUrl(1);

			ASSERT_TRUE(!!res.get());
			ASSERT_STREQ("a1", res->getName());
		}

		{
			auto res = pm.requestNewUrl(1, 1, "error");

			ASSERT_TRUE(!!res.get());
			ASSERT_STREQ("b1", res->getName());
		}

		{
			auto res = pm.getUrl(1);

			ASSERT_TRUE(!!res.get());
			ASSERT_STREQ("b1", res->getName());
		}
	}

	//test getUrl gives valid provider when all old providers have expired
	TEST(ProviderManager, ProvExpire)
	{
		auto pc = std::make_shared<const DownloadProvider>("a1", "a2", "a3", "a4");
		
		DownloadProvider *p = const_cast<DownloadProvider*>(&*pc);
		p->setType(DownloadProviderType::Cdn);
		p->setExpireTime(gcTime() + std::chrono::minutes(1));

		auto dp = std::make_shared<TestDownloadProvidersPM>();
		dp->m_vDownloadProviders.push_back(pc);
		dp->m_vDownloadProviders.push_back(std::make_shared<const DownloadProvider>("b1", "b2", "b3", "b4"));
		

		ProviderManager pm(dp);

		{
			auto res = pm.getUrl(1);

			ASSERT_TRUE(!!res.get());
			ASSERT_STREQ("a1", res->getName());
		}

		p->setExpireTime(gcTime() - std::chrono::minutes(1));

		//Getting it a second time should return same provider
		{
			auto res = pm.getUrl(1);

			ASSERT_TRUE(!!res.get());
			ASSERT_STREQ("b1", res->getName());
		}
	}

	//test getUrl gives valid provider when all old providers have expired (requests new providers)
	TEST(ProviderManager, ProvExpireWithNewProviders)
	{
		auto pc = std::make_shared<const DownloadProvider>("a1", "a2", "a3", "a4");

		DownloadProvider *p = const_cast<DownloadProvider*>(&*pc);
		p->setType(DownloadProviderType::Cdn);
		p->setExpireTime(gcTime() + std::chrono::minutes(1));

		auto dp = std::make_shared<TestDownloadProvidersPM>();
		dp->m_vDownloadProviders.push_back(pc);

		ProviderManager pm(dp);

		{
			auto res = pm.getUrl(1);

			ASSERT_TRUE(!!res.get());
			ASSERT_STREQ("a1", res->getName());
		}

		p->setExpireTime(gcTime() - std::chrono::minutes(1));

		dp->m_vDownloadProviders.clear();
		dp->m_vDownloadProviders.push_back(std::make_shared<const DownloadProvider>("b1", "b2", "b3", "b4"));

		//Getting it a second time should return same provider
		{
			auto res = pm.getUrl(1);

			ASSERT_TRUE(!!res.get());
			ASSERT_STREQ("b1", res->getName());
		}
	}

	TEST(ProviderManager, hasValidAgentsCallsInit)
	{
		auto pc = std::make_shared<const DownloadProvider>("a1", "a2", "a3", "a4");

		DownloadProvider *p = const_cast<DownloadProvider*>(&*pc);
		auto dp = std::make_shared<TestDownloadProvidersPM>();
		dp->m_vDownloadProviders.push_back(pc);

		ProviderManager pm(dp);
		ASSERT_TRUE(pm.hasValidAgents());
	}



	//test getUrl gives back errored providers after time out


}



#endif