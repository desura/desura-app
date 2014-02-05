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
#include "MCF.h"

#include "mcfcore/DownloadProvider.h"
#include "mcfcore/UserCookies.h"
#include "XMLMacros.h"

#include "thread/HGTController.h"
#include "BZip2.h"


#include <array>

namespace MCFCore
{

void MCF::getDownloadProviders(const char* url, MCFCore::Misc::UserCookies *pCookies, bool *unauthed, bool local)
{
	if (!pCookies)
		throw gcException(ERR_INVALID, "Cookies are null (getDownloadProviders)");

	HttpHandle wc(url);
	pCookies->set(wc);

	DesuraId id = m_sHeader->getDesuraId();
	gcString type = id.getTypeString();

	wc->addPostText("siteareaid", id.getItem());
	wc->addPostText("sitearea", type.c_str());
	wc->addPostText("branch", m_sHeader->getBranch());

	{
		MCFBuild build = m_sHeader->getBuild();

		if (build != 0)
			wc->addPostText("build", build);

		if (local)
			wc->addPostText("local", "yes");
	}

	wc->postWeb();

	if (wc->getDataSize() == 0)
		throw gcException(ERR_BADRESPONSE);
	
	XML::gcXMLDocument doc(const_cast<char*>(wc->getData()), wc->getDataSize());

	doc.ProcessStatus("itemdownloadurl");

	auto uNode = doc.GetRoot("itemdownloadurl");
	auto iNode = uNode.FirstChildElement("item");

	if (!iNode.IsValid())
		throw gcException(ERR_BADXML);

	auto mNode = iNode.FirstChildElement("mcf");

	if (!mNode.IsValid())
		throw gcException(ERR_BADXML);

	{
		const std::string build = mNode.GetAtt("build");
		const std::string branch = mNode.GetAtt("branch");

		//Debug(gcString("MCF: R: {0}.{1} G: {2}.{3}\n", m_sHeader->getBranch(), m_sHeader->getBuild(), build, branch));

		if (!build.empty())
			m_sHeader->setBuild(MCFBuild::BuildFromInt(atoi(build.c_str())));

		if (!branch.empty())
			m_sHeader->setBranch(MCFBranch::BranchFromInt(atoi(branch.c_str())));
	}

	const std::string szAuthCode = mNode.GetChild("authhash");

	if (szAuthCode.empty())
		throw gcException(ERR_BADXML);

	

	auto temp = std::make_shared<Misc::GetFile_s>();

	Safe::strncpy(temp->authhash.data(), 33, szAuthCode.c_str(), szAuthCode.size());
	Safe::snprintf(temp->authkey.data(), 10, "%d", pCookies->getUserId());

	m_pFileAuth = temp;


	auto urlNode = mNode.FirstChildElement("urls");

	if (!urlNode.IsValid())
		throw gcException(ERR_BADXML);

	const std::string szAuthed = mNode.GetChild("authed");

	if (unauthed && !szAuthed.empty())
	{
		*unauthed = (atoi(szAuthed.c_str()) == 0);
	}


#ifdef DEBUG
#if 0
	m_vProviderList.clear();
	m_vProviderList.push_back(new MCFCore::Misc::DownloadProvider("localhost", "mcf://10.0.0.121:62001", "", ""));
	return;
#endif
#endif

	urlNode.for_each_child("url", [this](const XML::gcXMLElement &xmlChild)
	{
		m_vProviderList.push_back(std::make_shared<MCFCore::Misc::DownloadProvider>(xmlChild));
	});

	if (m_vProviderList.size() == 0)
		throw gcException(ERR_ZEROSIZE);
}

void MCF::dlHeaderFromHttp(const char* url)
{
	if (m_bStopped)
		return;

	if (!url)
		throw gcException(ERR_BADURL);

	//FIXME: Needs error checking on getweb
	HttpHandle wc(url);
	wc->setDownloadRange(0, MCFCore::MCFHeader::getSizeS());

	wc->getWeb();

	if (wc->getDataSize() != MCFCore::MCFHeader::getSizeS())
		throw gcException(ERR_BADHEADER);

	MCFCore::MCFHeader webHeader((uint8*)wc->getData());
	setHeader(&webHeader);
	
	if (!webHeader.isValid())
		throw gcException(ERR_BADHEADER);

	wc->cleanUp();
	wc->setDownloadRange(webHeader.getXmlStart(), webHeader.getXmlSize());
	wc->getWeb();

	if (wc->getDataSize() == 0 || wc->getDataSize() != webHeader.getXmlSize())
		throw gcException(ERR_WEBDL_FAILED, "Failed to download MCF xml from web (size is ether zero or didnt match header size)");

	uint32 bz2BuffLen = webHeader.getXmlSize()*25;
	char* bz2Buff = nullptr;

	if ( isCompressed() )
	{
		bz2Buff = new char[bz2BuffLen];
		UTIL::STRING::zeroBuffer(bz2Buff, bz2BuffLen);

		try
		{
			UTIL::BZIP::BZ2DBuff((char*)bz2Buff, &bz2BuffLen, const_cast<char*>(wc->getData()), wc->getDataSize());
			parseXml(bz2Buff, bz2BuffLen);
			safe_delete(bz2Buff);
		}
		catch (gcException &)
		{
			safe_delete(bz2Buff);
			throw;
		}
	}
	else
	{
		parseXml(const_cast<char*>(wc->getData()), wc->getDataSize());
	}

	//we remove the complete flag due to the files not existing in the MCF
	for (size_t x=0; x< m_pFileList.size(); x++)
	{
		m_pFileList[x]->delFlag(MCFCore::MCFFileI::FLAG_COMPLETE);
	}
}

void MCF::dlFilesFromHttp(const char* url, const char* installDir)
{
	assert(!m_pTHandle);

	if (m_bStopped)
		return;

	if (!url)
		throw gcException(ERR_BADURL);

	//save the header first incase we fail
	saveMCF_Header();

	MCFCore::Thread::HGTController *temp = new MCFCore::Thread::HGTController(url, this, installDir);
	temp->onProgressEvent +=delegate(&onProgressEvent);
	temp->onErrorEvent += delegate(&onErrorEvent);

	m_pTHandle = temp;

	try
	{
		m_pTHandle->start();
		m_pTHandle->join();
		safe_delete(m_pTHandle);
	}
	catch (gcException &)
	{
		safe_delete(m_pTHandle);
		throw;
	}

	saveMCF_Header();
}

}