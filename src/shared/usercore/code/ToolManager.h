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
		ToolManager(gcRefPtr<UserCore::User> user);
		~ToolManager();

		void cleanup();

		void removeTransaction(ToolTransactionId ttid, bool forced) override;

		ToolTransactionId downloadTools(gcRefPtr<Misc::ToolTransaction> transaction) override;
		ToolTransactionId installTools(gcRefPtr<Misc::ToolTransaction> transaction) override;

		bool updateTransaction(ToolTransactionId ttid, gcRefPtr<Misc::ToolTransaction> transaction) override;

		bool areAllToolsValid(const std::vector<DesuraId> &list) override;
		bool areAllToolsDownloaded(const std::vector<DesuraId> &list) override;
		bool areAllToolsInstalled(const std::vector<DesuraId> &list) override;

		void parseXml(const XML::gcXMLElement &toolinfoNode) override;
		std::string getToolName(DesuraId toolId) override;

		void loadItems();
		void saveItems() override;



		void findJSTools(gcRefPtr<UserCore::Item::ItemInfoI> item) override;
		bool initJSEngine() override;
		void destroyJSEngine() override;


		void invalidateTools(std::vector<DesuraId> &list) override;

	#ifdef NIX
		void symLinkTools(std::vector<DesuraId> &list, const char* path) override;
		int hasNonInstallableTool(std::vector<DesuraId> &list) override;
	#endif

		void reloadTools(DesuraId id) override;

		gc_IMPLEMENT_REFCOUNTING(ToolManager);

	protected:
		void startDownload(gcRefPtr<Misc::ToolTransInfo> info);
		void cancelDownload(gcRefPtr<Misc::ToolTransInfo> info, bool force);

		void startInstall(ToolTransactionId ttid);
		void cancelInstall(ToolTransactionId ttid);

		void onSpecialCheck(WCSpecialInfo &info);

		void downloadTool(gcRefPtr<ToolInfo> tool);
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
			for (auto p : m_mTransactions)
			{
				if (p.second)
					t(p.second);
			}
		}

	private:
		bool m_bDeleteThread = false;

		gcRefPtr<UserCore::User> m_pUser;
		ToolTransactionId m_uiLastTransId = 0;

		std::mutex m_saveLock;

		std::mutex m_MapLock;
		std::map<ToolTransactionId, gcRefPtr<Misc::ToolTransInfo>> m_mTransactions;

		std::mutex m_DownloadLock;
		std::map<uint64, gcRefPtr<UserCore::Task::DownloadToolTask>> m_mDownloads;


		gcRefPtr<UserCore::Misc::ToolInstallThread> m_pToolThread;


		//////////////////
		// js
		//////////////////

		friend class ItemExtender;
		void addJSTool(gcRefPtr<UserCore::Item::ItemInfo> item, uint32 branchId, gcString name, gcString exe, gcString args, gcString res);

		uint32 m_uiInstanceCount = 0;
		FactoryFn m_pFactory = nullptr;

		SharedObjectLoader m_ScriptCore;
		std::mutex m_ScriptLock;

		int32 m_iLastCustomToolId = -1;
		time_t m_tJSEngineExpireTime = 0;
	};
}

#endif //DESURA_TOOLMANAGER_H
