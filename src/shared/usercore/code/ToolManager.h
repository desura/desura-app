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

#ifndef DESURA_TOOLMANAGER_H
#define DESURA_TOOLMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "usercore/ToolManagerI.h"

#include "ToolInfo.h"
#include "util_thread/BaseThread.h"
#include "BaseManager.h"
#include "managers/WildcardManager.h"

#include "SharedObjectLoader.h"

#ifdef WIN32
#include "IPCToolMain.h"
#endif

typedef void* (*FactoryFn)(const char*);
typedef void(*RegLogFn)(LogCallback*);

namespace UserCore
{

#ifdef WIN32
	class ToolIPCPipeClient;
#endif

	class User;

	namespace Misc
	{
		class ToolTransInfo;
		class ToolInstallThread;
	}

	namespace Task
	{
		class DownloadToolTask;
	}

	namespace Item
	{
		class ItemInfo;
	}


	class ToolManager : public ToolManagerI, public BaseManager<ToolInfo>
	{
	public:
		ToolManager(UserCore::User* user);
		~ToolManager();


		void removeTransaction(ToolTransactionId ttid, bool forced) override;

		ToolTransactionId downloadTools(Misc::ToolTransaction* transaction) override;
		ToolTransactionId installTools(Misc::ToolTransaction* transaction) override;

		bool updateTransaction(ToolTransactionId ttid, Misc::ToolTransaction* transaction) override;

		bool areAllToolsValid(const std::vector<DesuraId> &list) override;
		bool areAllToolsDownloaded(const std::vector<DesuraId> &list) override;
		bool areAllToolsInstalled(const std::vector<DesuraId> &list) override;

		void parseXml(const XML::gcXMLElement &toolinfoNode) override;
		std::string getToolName(DesuraId toolId) override;

		void loadItems();
		void saveItems() override;



		void findJSTools(UserCore::Item::ItemInfo* item) override;
		bool initJSEngine() override;
		void destroyJSEngine() override;


		void invalidateTools(std::vector<DesuraId> &list) override;

	#ifdef NIX
		void symLinkTools(std::vector<DesuraId> &list, const char* path) override;
		int hasNonInstallableTool(std::vector<DesuraId> &list) override;
	#endif

		void reloadTools(DesuraId id) override;

	protected:
		void startDownload(Misc::ToolTransInfo* info);
		void cancelDownload(Misc::ToolTransInfo* info, bool force);

		void startInstall(ToolTransactionId ttid);
		void cancelInstall(ToolTransactionId ttid);

		void onSpecialCheck(WCSpecialInfo &info);

		void downloadTool(ToolInfo* tool);
		void eraseDownload(DesuraId id);

		void onToolDLComplete(DesuraId id);
		void onToolDLError(DesuraId id, gcException &e);
		void onToolDLProgress(DesuraId id, UserCore::Misc::ToolProgress &prog);

		void postParseXml();

		void onPipeDisconnect();
		void onFailedToRun();


		bool loadJSEngine();
		void unloadJSEngine(bool forced = false);

		template <typename T>
		void for_each(const T &t)
		{
			std::for_each(m_mTransactions.begin(), m_mTransactions.end(), [&t](std::pair<ToolTransactionId, Misc::ToolTransInfo*> info)
			{
				if (info.second)
					t(info.second);
			});
		}

	private:
		bool m_bDeleteThread;

		UserCore::User* m_pUser;
		ToolTransactionId m_uiLastTransId; 

		std::mutex m_MapLock;
		std::map<ToolTransactionId, Misc::ToolTransInfo*> m_mTransactions;

		std::mutex m_DownloadLock;
		std::map<uint64, UserCore::Task::DownloadToolTask*> m_mDownloads;


		UserCore::Misc::ToolInstallThread* m_pToolThread;


		//////////////////
		// js
		//////////////////

		friend class ItemExtender;
		void addJSTool(UserCore::Item::ItemInfo* item, uint32 branchId, gcString name, gcString exe, gcString args, gcString res);

		uint32 m_uiInstanceCount;
		FactoryFn m_pFactory;

		SharedObjectLoader m_ScriptCore;
		std::mutex m_ScriptLock;

		int32 m_iLastCustomToolId;
		time_t m_tJSEngineExpireTime;
	};
}

#endif //DESURA_TOOLMANAGER_H
