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
#include "gcThemeLoader.h"
#include "MainApp.h"

REGISTER_SCHEME(ThemeLoaderScheme);

ThemeLoaderScheme::ThemeLoaderScheme() : DesuraSchemeBase<ThemeLoaderScheme>("desura", "theme")
{
}

bool ThemeLoaderScheme::processRequest(ChromiumDLL::SchemeRequestI* request, bool* redirect)
{
	char wurl[255];
	request->getURL(wurl, 255);

	// desura://theme/ is 15 chars!

	gcString url = wurl;
	url = url.substr(15);
	url = UTIL::STRING::urlDecode(url);

	const char* themeFolder = GetGCThemeManager()->getThemeFolder();

	UTIL::FS::Path path(themeFolder, url, false);

	if (!UTIL::FS::isValidFile(path))
		path = UTIL::FS::Path(UTIL::OS::getDataPath(L"themes/default"),
			UTIL::STRING::toWStr(url), false);

	if (!UTIL::FS::isValidFile(path))
		return false;

#ifdef WIN32
	gcString localUrl("file:///{0}", path.getFullPath());
#else
	gcString localUrl("file://{0}", path.getFullPath());
#endif

	m_szRedirectUrl = UTIL::STRING::sanitizeFilePath(localUrl.c_str(), '/');
	*redirect = true;

	return true;
}

void ThemeLoaderScheme::cancel()
{
}

bool ThemeLoaderScheme::read(char* buffer, int size, int* readSize)
{
	return false;
}








REGISTER_SCHEME(StaticLoaderScheme);


StaticLoaderScheme::StaticLoaderScheme() : DesuraSchemeBase<StaticLoaderScheme>("desura", "static")
{
}

bool StaticLoaderScheme::processRequest(ChromiumDLL::SchemeRequestI* request, bool* redirect)
{
	char wurl[255];
	request->getURL(wurl, 255);

	// desura://static/ is 16 chars!

	gcString url = wurl;
	url = url.substr(16);
	url = UTIL::STRING::urlDecode(url);

	UTIL::FS::Path path(gcString("{0}/static", UTIL::OS::getDataPath()), url, false);

	if (!UTIL::FS::isValidFile(path))
		return false;

	gcString localUrl("file:///{0}", path.getFullPath());
	m_szRedirectUrl = UTIL::STRING::sanitizeFilePath(localUrl.c_str(), '/');

	*redirect = true;
	return true;
}

void StaticLoaderScheme::cancel()
{
}

bool StaticLoaderScheme::read(char* buffer, int size, int* readSize)
{
	return false;
}



REGISTER_SCHEME(ExternalLoaderScheme);

ExternalLoaderScheme::ExternalLoaderScheme() : DesuraSchemeBase<ExternalLoaderScheme>("desura", "external")
{
}

bool ExternalLoaderScheme::processRequest(ChromiumDLL::SchemeRequestI* request, bool* redirect)
{
	auto pWebCore = GetWebCore();

	if (!pWebCore)
		return false;

	char wurl[255];
	request->getURL(wurl, 255);

	// desura://external/ is 18 chars!

	gcString url = wurl;
	url = url.substr(18);

	if (url == "play.js")
	{
		m_szRedirectUrl = pWebCore->getUrl(WebCore::WebCoreUrl::PlayJavaScript);
		*redirect = true;
		return true;
	}
	else
	{
		return false;
	}
}

void ExternalLoaderScheme::cancel()
{
}

bool ExternalLoaderScheme::read(char* buffer, int size, int* readSize)
{
	return false;
}
