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

#ifndef DESURA_MCFDOWNLOADPROVIDER_H
#define DESURA_MCFDOWNLOADPROVIDER_H
#ifdef _WIN32
#pragma once
#endif

#include "mcfcore/DownloadProvider.h"
#include "XMLMacros.h"
#include "mcfcore/MCFMain.h"

namespace WebCore
{
	class WebCoreI;
}

namespace UserCore
{

	class UserI;

	class MCFDownloadProviders : public MCFCore::Misc::DownloadProvidersI
	{
	public:
		MCFDownloadProviders(gcRefPtr<WebCore::WebCoreI> pWebCore, int nUserId);

		void setInfo(DesuraId id, MCFBranch branch, MCFBuild build) override;

		bool getDownloadProviders(std::vector<std::shared_ptr<const MCFCore::Misc::DownloadProvider>> &vDownloadProviders) override;

		std::shared_ptr<const MCFCore::Misc::GetFile_s> getDownloadAuth() override;

		size_t size() override;

		void forceLoad(MCFCore::MCFI* pMcf);

		static void forceLoad(McfHandle &mcf, std::shared_ptr<MCFDownloadProviders> &dp)
		{
			mcf->setDownloadProvider(dp);
			dp->forceLoad(mcf.handle());
		}

		bool isUnAuthed() const;

	protected:
		void downloadXml();
		void processXml(XML::gcXMLDocument &xmlDoc);
		void purgeExpiredProviders();

	private:
		bool m_bInit = false;
		bool m_bUnAuthed = false;
		uint32 m_nUserId = 666;
		uint32 m_nFirstCount = 0;

		DesuraId m_Id;
		MCFBuild m_Build;
		MCFBranch m_Branch;

		gcTime m_tLastUpdate;
		gcRefPtr<WebCore::WebCoreI> m_pWebCore;

		std::mutex m_UpdateLock;
		std::vector<std::shared_ptr<const MCFCore::Misc::DownloadProvider>> m_vDownloadProviders;
		std::shared_ptr<const MCFCore::Misc::GetFile_s> m_DownloadAuth;
	};


}

#endif
