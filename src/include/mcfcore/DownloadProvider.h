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
	class GetFile_s;

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
		: m_szName("")
		, m_szUrl("")
		, m_szProvUrl("")
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
			gcAssert(xmlElement.IsValid());

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

			uint64 ttl = -1;
			xmlElement.GetChild("ttl", ttl);

			if (ttl != -1)
				m_tExpireTime = gcTime() + std::chrono::seconds(ttl);
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
			std::lock_guard<std::mutex> guard(m_BannerLock);
			return m_szBanner.data();
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
			std::lock_guard<std::mutex> guard(m_BannerLock);

			gcString strBanner(banner);
			Safe::strncpy(m_szBanner.data(), m_szBanner.size(), strBanner.c_str(), strBanner.size());
		}

		void setType(DownloadProviderType type)
		{
			m_eType = type;
		}


		DownloadProviderType getType() const
		{
			return m_eType;
		}

		void setExpireTime(gcTime time)
		{
			m_tExpireTime = time;
		}

		bool hasExpired() const
		{
			if (m_eType != DownloadProviderType::Cdn)
				return false;

			return gcTime() > m_tExpireTime;
		}

		bool isValidAndNotExpired() const
		{
			return !hasExpired() && !m_szUrl.empty();
		}

	private:
		DownloadProviderType m_eType = DownloadProviderType::Mcf;

		gcTime m_tExpireTime;
		std::string m_szName;
		std::string m_szUrl;
		std::string m_szProvUrl;

		mutable std::mutex m_BannerLock;
		mutable std::array<char, 256> m_szBanner;
	};


	class DownloadProvidersI
	{
	public:
		virtual ~DownloadProvidersI(){}

		virtual void setInfo(DesuraId id, MCFBranch branch, MCFBuild build)=0;

		//! Get a list of download providers
		//!
		virtual bool getDownloadProviders(std::vector<std::shared_ptr<const DownloadProvider>> &vDownloadProviders)=0;

		//! Auth token for downloads
		//!
		virtual std::shared_ptr<const GetFile_s> getDownloadAuth()=0;

		//! Number of download providers
		//!
		virtual size_t size()=0;
	};
}
}

#endif //DESURA_DOWNLOADPROVIDER_H
