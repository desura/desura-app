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
#include "MCFDownloadProviders.h"
#include "mcfcore/DownloadProvider.h"
#include "mcfcore/MCFI.h"
#include "webcore/WebCoreI.h"

using namespace UserCore;

MCFDownloadProviders::MCFDownloadProviders(WebCore::WebCoreI* pWebCore, int nUserId)
	: m_pWebCore(pWebCore)
	, m_nUserId(nUserId)
{
}

void MCFDownloadProviders::setInfo(DesuraId id, MCFBranch branch, MCFBuild build)
{
	gcAssert(!m_bInit);

	m_Id = id;
	m_Build = build;
	m_Branch = branch;
}

bool MCFDownloadProviders::getDownloadProviders(std::vector<std::shared_ptr<const MCFCore::Misc::DownloadProvider>> &vDownloadProviders)
{
	std::lock_guard<std::mutex> guard(m_UpdateLock);

	purgeExpiredProviders();

	if (m_vDownloadProviders.size() < m_nFirstCount)
		downloadXml();

	vDownloadProviders = m_vDownloadProviders;
	return !m_vDownloadProviders.empty();
}

std::shared_ptr<const MCFCore::Misc::GetFile_s> MCFDownloadProviders::getDownloadAuth()
{
	std::lock_guard<std::mutex> guard(m_UpdateLock);

	if (!m_bInit)
		downloadXml();

	return m_DownloadAuth;
}

size_t MCFDownloadProviders::size()
{
	std::vector<std::shared_ptr<const MCFCore::Misc::DownloadProvider>> vDownloadProviders;
	getDownloadProviders(vDownloadProviders);

	return vDownloadProviders.size();
}

void MCFDownloadProviders::forceLoad(MCFCore::MCFI* pMcf)
{
	gcAssert(pMcf);

	{
		std::lock_guard<std::mutex> guard(m_UpdateLock);
		downloadXml();
	}

	auto header = pMcf->getHeader();
	gcAssert(header);
	
	header->setBuild(m_Build);
	header->setBranch(m_Branch);
}

void MCFDownloadProviders::downloadXml()
{
	//only update once every 5 mins
	if (m_bInit && gcTime() < (m_tLastUpdate + std::chrono::minutes(5)))
		return;

	XML::gcXMLDocument doc;
	m_pWebCore->getDownloadProviders(m_Id, doc, m_Branch, m_Build);
	processXml(doc);

	m_tLastUpdate = gcTime();
}

void MCFDownloadProviders::purgeExpiredProviders()
{
	auto it = std::remove_if(m_vDownloadProviders.begin(), m_vDownloadProviders.end(), [](std::shared_ptr<const MCFCore::Misc::DownloadProvider> &p)
	{
		return !p->isValidAndNotExpired();
	});

	if (it != m_vDownloadProviders.end())
		m_vDownloadProviders.erase(it, m_vDownloadProviders.end());
}

void MCFDownloadProviders::processXml(XML::gcXMLDocument &doc)
{
	auto uNode = doc.GetRoot("itemdownloadurl");
	auto iNode = uNode.FirstChildElement("item");

	if (!iNode.IsValid())
		throw gcException(ERR_BADXML);

	auto mNode = iNode.FirstChildElement("mcf");

	if (!mNode.IsValid())
		throw gcException(ERR_BADXML);

	{
		const std::string szBuild = mNode.GetAtt("build");
		const std::string szBranch = mNode.GetAtt("branch");

		MCFBuild build;
		MCFBranch branch;

		if (!szBuild.empty())
			build = MCFBuild::BuildFromInt(Safe::atoi(szBuild.c_str()));

		if (!szBranch.empty())
			branch = MCFBranch::BranchFromInt(Safe::atoi(szBranch.c_str()));

		if (m_bInit)
		{
			if (m_Build != build)
				throw new gcException(ERR_BADRESPONSE, "Build didn't match expected build");

			if (m_Branch != branch)
				throw new gcException(ERR_BADRESPONSE, "Branch didn't match expected branch");
		}
		else
		{
			m_Branch = branch;
			m_Build = build;
		}
	}

	if (!m_bInit)
	{
		const std::string szAuthCode = mNode.GetChild("authhash");

		if (szAuthCode.empty())
			throw gcException(ERR_BADXML);

		auto temp = std::make_shared<MCFCore::Misc::GetFile_s>();

		Safe::strncpy(temp->authhash.data(), 33, szAuthCode.c_str(), szAuthCode.size());
		Safe::snprintf(temp->authkey.data(), 10, "%d", m_nUserId);

		m_DownloadAuth = temp;

		bool bAuthed = true;
		if (mNode.GetChild("authed", bAuthed))
			m_bUnAuthed = !bAuthed;
	}

	auto urlNode = mNode.FirstChildElement("urls");

	if (!urlNode.IsValid())
		throw gcException(ERR_BADXML);

	urlNode.for_each_child("url", [this](const XML::gcXMLElement &xmlChild)
	{
		auto p = std::make_shared<MCFCore::Misc::DownloadProvider>(xmlChild);

		auto bFound = false;

		for (auto x : m_vDownloadProviders)
		{
			if (gcString(p->getUrl()) == x->getUrl())
			{
				bFound = true;
				break;
			}
		}

		if (!bFound)
			m_vDownloadProviders.push_back(p);
	});

	if (!m_bInit)
		m_nFirstCount = m_vDownloadProviders.size();

	m_bInit = true;
}

bool MCFDownloadProviders::isUnAuthed() const
{
	return m_bUnAuthed;
}