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
#include "Common.h"
#include "MCF.h"

#include "XMLMacros.h"

#include "thread/HGTController.h"
#include "BZip2.h"


#include <array>

namespace MCFCore
{

void MCF::dlHeaderFromHttp(const char* url)
{
	gcTrace("Url: {0}", url);

	if (m_bStopped)
		return;

	if (!url)
		throw gcException(ERR_BADURL);

	//FIXME: Needs error checking on getweb
	HttpHandle wc(url);
	wc->setDownloadRange(0, MCFCore::MCFHeader::getSizeS());

	wc->setUserAgent(USERAGENT_UPDATE);
	wc->getWeb();

	if (wc->getDataSize() != MCFCore::MCFHeader::getSizeS())
		throw gcException(ERR_BADHEADER);

	MCFCore::MCFHeader webHeader(wc->getData());
	setHeader(&webHeader);

	if (!webHeader.isValid())
		throw gcException(ERR_BADHEADER);

	wc->cleanUp();
	wc->setUserAgent(USERAGENT_UPDATE);
	wc->setDownloadRange(webHeader.getXmlStart(), webHeader.getXmlSize());
	wc->getWeb();

	if (wc->getDataSize() == 0 || wc->getDataSize() != webHeader.getXmlSize())
		throw gcException(ERR_WEBDL_FAILED, "Failed to download MCF xml from web (size is ether zero or didnt match header size)");

	uint32 bz2BuffLen = webHeader.getXmlSize()*25;
	char* bz2Buff = nullptr;

	if ( isCompressed() )
	{
		bz2Buff = new char[bz2BuffLen];
		AutoDelete<char> ad(bz2Buff);

		UTIL::STRING::zeroBuffer(bz2Buff, bz2BuffLen);
		UTIL::BZIP::BZ2DBuff((char*)bz2Buff, &bz2BuffLen, const_cast<char*>(wc->getData()), wc->getDataSize());
		parseXml(bz2Buff, bz2BuffLen);
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
	gcTrace("Url: {0}, Dir: {1}", url, installDir);

	gcAssert(!m_pTHandle);

	if (m_bStopped)
		return;

	if (!url)
		throw gcException(ERR_BADURL);

	//save the header first incase we fail
	saveMCF_Header();

	MCFCore::Thread::HGTController *temp = new MCFCore::Thread::HGTController(url, this, installDir);
	temp->onProgressEvent +=delegate(&onProgressEvent);
	temp->onErrorEvent += delegate(&onErrorEvent);

	runThread(temp);
	saveMCF_Header();
}

}
