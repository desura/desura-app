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