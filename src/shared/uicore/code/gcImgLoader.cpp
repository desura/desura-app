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
#include "gcImgLoader.h"
#include "MainApp.h"

REGISTER_SCHEME(ImgLoaderScheme);

ImgLoaderScheme::ImgLoaderScheme() : DesuraSchemeBase<ImgLoaderScheme>("desura", "themeimage")
{
}

bool ImgLoaderScheme::processRequest(ChromiumDLL::SchemeRequestI* request, bool* redirect)
{
	char wurl[255];
	request->getURL(wurl, 255);

	// desura://themeimage/ is 20 chars!

	gcString url = wurl;
	url = url.substr(20);

	UTIL::FS::Path file(UTIL::OS::getDataPath(L"themes"), L"", false);
	UTIL::FS::Path img = UTIL::FS::PathWithFile(url);

	file += img;
	file += img.getFile();


	if (!UTIL::FS::isValidFile(file))
		return false;

	UTIL::FS::FileHandle handle(file, UTIL::FS::FILE_READ);

	unsigned char buff[5];

	try
	{
		handle.read((char*)buff, 5);
		handle.seek(0);
	}
	catch (gcException &)
	{
		return false;
	}

	switch (UTIL::MISC::isValidImage(buff))
	{
	case IMAGE_GIF: m_szMimeType = "image/gif"; break;
	case IMAGE_JPG: m_szMimeType = "image/jpeg"; break;
	case IMAGE_PNG: m_szMimeType = "image/png"; break;
	default:
		return false;
	};

	m_uiResponseSize = UTIL::FS::getFileSize(file);
	m_hImgHandle = handle;

	m_pCallback->responseReady();
	return true;
}

void ImgLoaderScheme::cancel()
{
	if (m_hImgHandle.isValidFile())
		m_hImgHandle.close();
}

bool ImgLoaderScheme::read(char* buffer, int size, int* readSize)
{
	*readSize = 0;

	if (!m_hImgHandle.isValidFile())
		return false;

	try
	{
		m_hImgHandle.read((char*)buffer, size);
	}
	catch (gcException &)
	{
		return false;
	}

	*readSize = size; 
	return true;
}
