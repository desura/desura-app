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
		MCFDownloadProviders(WebCore::WebCoreI* pWebCore, int nUserId);

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
		WebCore::WebCoreI* m_pWebCore;

		std::mutex m_UpdateLock;
		std::vector<std::shared_ptr<const MCFCore::Misc::DownloadProvider>> m_vDownloadProviders;
		std::shared_ptr<const MCFCore::Misc::GetFile_s> m_DownloadAuth;
	};


}

#endif