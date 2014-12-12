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

#ifndef DESURA_INSTALLINFO_H
#define DESURA_INSTALLINFO_H
#ifdef _WIN32
#pragma once
#endif

#include "managers/WildcardManager.h"
#include "usercore/InstallInfoI.h"


namespace XML
{
	class gcXMLElement;
}

namespace UserCore
{
	namespace Misc
	{
		class InstallInfo : public InstallInfoI
		{
		public:
			InstallInfo(DesuraId id, DesuraId pid = 0);
			~InstallInfo();

			void loadXmlData(const XML::gcXMLElement &xmlNode, gcRefPtr<WildcardManager> &pWildCard);

			const char* getName(){ return m_szName.c_str(); }
			const char* getPath(){ return m_szPath.c_str(); }
			bool isInstalled(){ return m_bInstalled; }
			DesuraId getId(){ return m_iID; }
			DesuraId getParentId(){ return m_iParentID; }

		private:
			gcString m_szName;
			gcString m_szPath;

			bool m_bInstalled;

			DesuraId m_iID;
			DesuraId m_iParentID;
		};
	}
}

#endif //DESURA_INSTALLINFO_H
