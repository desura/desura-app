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

#ifndef DESURA_BRANCHINFO_H
#define DESURA_BRANCHINFO_H
#ifdef _WIN32
#pragma once
#endif

#include "usercore/BranchInfoI.h"

namespace XML
{
	class gcXMLElement;
}

namespace sqlite3x
{
	class sqlite3_connection;
}


namespace UserCore
{
	namespace Item
	{

		class BranchInstallInfo;

		class BranchInfo : public BranchInfoI
		{
		public:
			BranchInfo(MCFBranch branchId, DesuraId itemId, gcRefPtr<BranchInstallInfo> bii, uint32 platformId, uint32 userid);
			~BranchInfo();

			virtual uint32 getFlags();
			virtual MCFBranch getBranchId();
			virtual MCFBranch getGlobalId();
			virtual DesuraId getItemId();

			virtual const char* getName();
			virtual const char* getCost();
			virtual const char* getEulaUrl();
			virtual const char* getPreOrderExpDate();
			virtual const char* getInstallScriptPath();

			virtual void getToolList(std::vector<DesuraId> &toolList);

			virtual bool isAvaliable();
			virtual bool isDownloadable();
			virtual bool isPreOrder();
			bool isPreOrderAndNotPreload() override;
			virtual bool hasAcceptedEula();
			virtual bool hasCDKey();
			virtual bool isCDKeyValid();

			virtual bool isWindows();
			virtual bool isLinux();
			virtual bool isMacOsX();
			virtual bool is32Bit();
			virtual bool is64Bit();

			virtual bool isSteamGame();

			//! Accepts the eula
			//!
			void acceptEula();

			//! Removes this branch from the db
			//!
			//! @param db Sqlite db connection
			//!
			void deleteFromDb(sqlite3x::sqlite3_connection* db);

			//! Save regular changed vars to db
			//!
			//! @param db Sqlite db connection
			//!
			void saveDb(sqlite3x::sqlite3_connection* db);

			//! Save all vars to db
			//!
			//! @param db Sqlite db connection
			//!
			void saveDbFull(sqlite3x::sqlite3_connection* db);

			//! Load vars from db
			//!
			//! @param db Sqlite db connection
			//!
			void loadDb(sqlite3x::sqlite3_connection* db);

			//! Load data for this branch from xml
			//!
			//! @param xmlNode Xml to get data from
			//!
			void loadXmlData(const XML::gcXMLElement &xmlNode);

			//! Returns the last mcf build for this branch
			//!
			//! @return build
			//!
			MCFBuild getLatestBuild();

			void getCDKey(std::vector<gcString> &vKeys) const;
			void setCDKey(gcString key);

			EventV onBranchInfoChangedEvent;
			EventV onBranchCDKeyChangedEvent;

			void addJSTool(DesuraId toolId);

			void setLinkInfo(const char* name);

			gcRefPtr<BranchInstallInfo> getInstallInfo();

			gc_IMPLEMENT_REFCOUNTING(BranchInfo);

		protected:
			gcString encodeCDKey(const gcString& strRawKey);
			gcString decodeCDKey(const gcString& strEncodedKey);

			void processInstallScript(const XML::gcXMLElement &scriptNode);

		private:
			DesuraId m_ItemId;

			gcString m_szName;
			gcString m_szCost;
			gcString m_szEulaUrl;
			gcString m_szEulaDate;
			gcString m_szPreOrderDate;
			gcString m_szInstallScript;

			uint32 m_uiInstallScriptCRC;
			uint32 m_uiFlags;
			uint32 m_uiUserId;

			MCFBranch m_uiGlobalId;
			MCFBranch m_uiBranchId;
			MCFBuild m_uiLatestBuild;

			std::vector<DesuraId> m_vToolList;
			std::vector<gcString> m_vCDKeyList;

			gcRefPtr<BranchInstallInfo> m_InstallInfo;

			mutable std::mutex m_BranchLock;
		};



		inline uint32 BranchInfo::getFlags()
		{
			return m_uiFlags;
		}

		inline MCFBranch BranchInfo::getBranchId()
		{
			return m_uiBranchId;
		}

		inline MCFBranch BranchInfo::getGlobalId()
		{
			return m_uiGlobalId;
		}

		inline MCFBuild BranchInfo::getLatestBuild()
		{
			return m_uiLatestBuild;
		}



		inline const char* BranchInfo::getName()
		{
			return m_szName.c_str();
		}

		inline const char* BranchInfo::getCost()
		{
			return m_szCost.c_str();
		}

		inline const char* BranchInfo::getEulaUrl()
		{
			return m_szEulaUrl.c_str();
		}

		inline const char* BranchInfo::getPreOrderExpDate()
		{
			return m_szPreOrderDate.c_str();
		}

		inline const char* BranchInfo::getInstallScriptPath()
		{
			if (m_szInstallScript.size() > 0)
				return m_szInstallScript.c_str();

			return nullptr;
		}

	}
}


#endif //DESURA_BRANCHINFO_H
