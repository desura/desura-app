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
#include "WebCore.h"
#include "XMLMacros.h"

#include "BZip2.h"
#include "sqlite3x.hpp"
#include "sql/WebCoreSql.h"

#include "mcfcore/DownloadProvider.h"
#include "webcore/DownloadImageInfo.h"


namespace WebCore
{
	extern gcString genUserAgent();
}


using namespace WebCore;



void WebCoreClass::sendPassReminder(const char* email)
{
	if (!email)
		throw gcException(ERR_INVALIDDATA);

	HttpHandle hh(getPassWordReminderUrl().c_str());

	gcString useragent = genUserAgent();
	
	if (useragent.size() > 0)
		hh->setUserAgent(useragent.c_str());

	hh->addPostText("emailreset", email);
	hh->postWeb();

	if (hh->getDataSize() == 0)
		throw gcException(ERR_BADRESPONSE, "Data size was zero");

	XML::gcXMLDocument doc(const_cast<char*>(hh->getData()), hh->getDataSize());
	doc.ProcessStatus("memberpasswordreminder");
}

void WebCoreClass::getInstalledItemList(XML::gcXMLDocument &xmlDocument)
{
	HttpHandle hh(getInstalledWizardUrl().c_str());
	setWCCookies(hh);
	hh->getWeb();
	
	if (hh->getDataSize() == 0)
		throw gcException(ERR_BADRESPONSE);

	if (strncmp(hh->getData(), "BZh", 3)==0)
	{
		uint32 bufSize = hh->getDataSize() *25;

		UTIL::MISC::Buffer buff(bufSize);
		UTIL::BZIP::BZ2DBuff(buff, &bufSize, const_cast<char*>(hh->getData()), hh->getDataSize());

		xmlDocument.LoadBuffer(buff, bufSize);
	}
	else
	{
		xmlDocument.LoadBuffer(const_cast<char*>(hh->getData()), hh->getDataSize());
	}

	xmlDocument.ProcessStatus("itemwizard");
}

void WebCoreClass::onHttpProg(std::atomic<bool>* stop, Prog_s& prog)
{
	prog.abort = *stop;
}

void WebCoreClass::downloadImage(WebCore::Misc::DownloadImageInfo *dii, std::atomic<bool> &stop)
{
	if (!dii)
		throw gcException(ERR_BADITEM);

	const char* imageUrl = dii->url.c_str();
	uint32 hash = UTIL::MISC::RSHash_CSTR(imageUrl);

	std::string szPath = m_ImageCache.getImagePath(hash);

	if (UTIL::FS::isValidFile(szPath))
	{
		dii->outPath = szPath;
		return;
	}

	if (strncmp(imageUrl,"http://", 7) != 0)
		throw gcException(ERR_INVALIDDATA, gcString("The url {0} is not a valid http url", imageUrl));

	gcString appDataPath = UTIL::OS::getCachePath(L"images");
	gcString fileName = UTIL::FS::Path(imageUrl, "", true).getFile().getFile();

	if (dii->id.isOk())
		fileName = gcString("{0}_{1}", dii->id.toInt64(), fileName);
	else
		fileName = gcString("{0}_{1}", hash, fileName);

	HttpHandle hh(imageUrl);

	hh->setUserAgent(getUserAgent());
	hh->getProgressEvent() += extraDelegate(this, &WebCoreClass::onHttpProg, &stop);

	hh->getWeb();
		
	if (hh->getDataSize() == 0)
		throw gcException(ERR_BADRESPONSE);

	if (UTIL::MISC::isValidImage((const unsigned char*)hh->getData()) == IMAGE_VOID)
		throw gcException(ERR_INVALIDDATA, gcString("The url {0} is not a valid image format", imageUrl));

	UTIL::FS::Path path(appDataPath, fileName, false);

	UTIL::FS::recMakeFolder(path);
	UTIL::FS::FileHandle fh(path, UTIL::FS::FILE_WRITE);

	fh.write(hh->getData(), hh->getDataSize());
			
	dii->outPath = path.getFullPath();
	m_ImageCache.updateImagePath(dii->outPath.c_str(), hash);
}

void WebCoreClass::downloadBanner(MCFCore::Misc::DownloadProvider* dlp, const char* saveFolder)
{
	if (!dlp)
		throw gcException(ERR_BADITEM);

	WebCore::Misc::DownloadImageInfo dii(dlp->getBanner());

	std::atomic<bool> stop;
	downloadImage(&dii, stop);
	dlp->setBanner(dii.outPath.c_str());
}
