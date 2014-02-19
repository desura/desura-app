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

#ifndef DESURA_DOWNLOADPROVIDER_H
#define DESURA_DOWNLOADPROVIDER_H
#ifdef _WIN32
#pragma once
#endif

#include "XMLMacros.h"
#include <mutex>
#include <array>

namespace MCFCore
{
namespace Misc
{

	enum class DownloadProviderType
	{
		Mcf,
		Http,
		Cdn,
	};

//! DownloadProvider class stores information about servers that are available to download MCF content from
//! 
class DownloadProvider
{
public:
	enum PROVIDER
	{
		ADD,	//!< Using new provider (i.e. add to banner)
		REMOVE,	//!< Stop using provider (i.e. remove from banner)
	};

	//! Default Constructor
	DownloadProvider()
	{
	}

	//! Copy Constructor
	DownloadProvider(const MCFCore::Misc::DownloadProvider& prov)
	{
		m_szName = prov.getName();
		m_szUrl = prov.getUrl();
		m_szProvUrl = prov.getProvUrl();
		m_eType = prov.getType();

		setBanner(prov.getBanner());
	}

	DownloadProvider& operator=(const DownloadProvider& prov)
	{
		if (this == &prov)
			return *this;

		m_szName = prov.getName();
		m_szUrl = prov.getUrl();
		m_szProvUrl = prov.getProvUrl();
		m_eType = prov.getType();

		setBanner(prov.getBanner());

		return *this;
	}

	//! Alt Constructor
	//!
	//! @param n Provider Name
	//! @param u Download Url
	//! @param b Banner Url
	//! @param p Provider Url
	//!
	DownloadProvider(const char* n, const char* u, const char* b, const char* p)
		: m_szName(n)
		, m_szUrl(u)
		, m_szProvUrl(p)
	{
		setBanner(b);
	}

	//! Alt Constructor from xml
	//! 
	//! @param node Xml node to get info from
	//!
	DownloadProvider(const XML::gcXMLElement &xmlElement)
	{
		assert(xmlElement.IsValid());

		const std::string strBanner = xmlElement.GetChild("banner");
		setBanner(strBanner.c_str());

		xmlElement.GetChild("provider", m_szName);
		xmlElement.GetChild("provlink", m_szProvUrl);
		xmlElement.GetChild("link", m_szUrl);

		gcString type;
		xmlElement.GetChild("type", type);

		if (type == "cdn")
			m_eType = DownloadProviderType::Cdn;
		else if (type == "http")
			m_eType = DownloadProviderType::Http;
		else
			m_eType = DownloadProviderType::Mcf;
	}

	//! Checks to see if its a valid banner
	//! 
	//! @return True if valid, False if not
	//!
	bool isValid() const
	{
		return (m_szName.size() > 0 && m_szUrl.size() > 0 && m_szBanner.size() > 0 && m_szProvUrl.size() > 0);
	}

	//! Gets the provider name
	//!
	//! @return Provider name
	//!
	const char* getName() const
	{
		return m_szName.c_str();
	}

	//! Gets the download url
	//!
	//! @return Download url
	//!
	const char* getUrl() const
	{
		return m_szUrl.c_str();
	}

	//! Gets the banner url
	//!
	//! @return Banner Url
	//!
	const char* getBanner() const
	{
		return m_szBanner.c_str();
	}

	//! Gets the providers url
	//!
	//! @return Provider url
	//!
	const char* getProvUrl() const
	{
		return m_szProvUrl.c_str();
	}

	//! Sets the banner url
	//!
	//! @param banner New banner Url
	//!
	void setBanner(const char* banner) const
	{
		m_szBanner = banner;
	}

	void setType(DownloadProviderType type)
	{
		m_eType = type;
	}


	DownloadProviderType getType() const
	{
		return m_eType;
	}

private:
	DownloadProviderType m_eType = DownloadProviderType::Mcf;
	gcString m_szName;
	gcString m_szUrl;
	gcString m_szProvUrl;

	mutable gcFixedString<255> m_szBanner;
};

}
}

#endif //DESURA_DOWNLOADPROVIDER_H
