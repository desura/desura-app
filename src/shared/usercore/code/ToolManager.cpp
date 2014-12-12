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
#include "ToolManager.h"

#include "User.h"
#include "sqlite3x.hpp"
#include "sql/ToolManagerSql.h"

#include "DownloadToolTask.h"
#include "ToolTransaction.h"
#include "ToolInstallThread.h"

#ifdef WIN32
gcRefPtr<UserCore::ToolInfo> NewJSToolInfo(DesuraId id);
#endif

using namespace UserCore;

ToolManager::ToolManager(gcRefPtr<UserCore::User> user)
	: BaseManager()
{
	m_pUser = user;
	createToolInfoDbTables(user->getAppDataPath());
	m_tJSEngineExpireTime = time(nullptr);
}

ToolManager::~ToolManager()
{
	cleanup();
}

void ToolManager::cleanup()
{
	if (m_pToolThread)
		m_pToolThread->stop();

	unloadJSEngine(true);
	saveItems();

	{
		std::lock_guard<std::mutex> guard(m_DownloadLock);
		m_mDownloads.clear();
	}

	{
		std::lock_guard<std::mutex> guard(m_MapLock);
		m_mTransactions.clear();
	}

	safe_delete(m_pToolThread);
}

void ToolManager::loadItems()
{
	sqlite3x::sqlite3_connection db(getToolInfoDb(m_pUser->getAppDataPath()).c_str());

	std::vector<DesuraId> toolIdList;

	try
	{
		sqlite3x::sqlite3_command cmd(db, "SELECT internalid FROM toolinfo;");
		sqlite3x::sqlite3_reader reader = cmd.executereader();

		while (reader.read())
		{
			toolIdList.push_back(DesuraId(reader.getint64(0)));
		}
	}
	catch (std::exception &)
	{
	}

	for (size_t x=0; x<toolIdList.size(); x++)
	{
		auto tool = findItem(toolIdList[x].toInt64());

		bool bAdd = false;

		int32 negId = toolIdList[x].getItem();

		if (!tool)
		{
			if (negId >= 0)
				tool = gcRefPtr<UserCore::ToolInfo>::create(toolIdList[x]);
#ifdef WIN32
			else
				tool = NewJSToolInfo(toolIdList[x]);
#endif

			bAdd = true;
		}
		else if (negId < m_iLastCustomToolId)
		{
			m_iLastCustomToolId = negId;
		}

		tool->loadFromDb(&db);

		if (bAdd)
			addItem(tool);
	}
}

void ToolManager::saveItems()
{
	try
	{
		sqlite3x::sqlite3_connection db(getToolInfoDb(m_pUser->getAppDataPath()).c_str());
		sqlite3x::sqlite3_transaction trans(db);

		for (uint32 x=0; x<getCount(); x++)
		{
			auto tool = getItem(x);

			if (!tool)
				continue;

			try
			{
				tool->saveToDb(&db);
			}
			catch (std::exception& e)
			{
				Warning("Failed to save tool {0}: {1}\n", tool->getName(), e.what());
			}
		}

		trans.commit();
	}
	catch (std::exception &e)
	{
		Warning("Failed to save tool info: {0}\n", e.what());
	}
}


void ToolManager::removeTransaction(ToolTransactionId ttid, bool forced)
{
	gcRefPtr<Misc::ToolTransInfo> info;

	m_MapLock.lock();

	auto it = m_mTransactions.find(ttid);

	if (it != m_mTransactions.end())
	{
		info = it->second;
		m_mTransactions.erase(it);
	}

	m_MapLock.unlock();

	if (info)
	{
		if (info->isDownload())
			cancelDownload(info, forced);
		else
			cancelInstall(ttid);
	}
}

ToolTransactionId ToolManager::downloadTools(gcRefPtr<Misc::ToolTransaction> transaction)
{
	if (!areAllToolsValid(transaction->getList()))
	{
		safe_delete(transaction);
		return -1;
	}

	if (areAllToolsDownloaded(transaction->getList()))
	{
		safe_delete(transaction);
		return -1;
	}

	auto info = gcRefPtr<Misc::ToolTransInfo>::create(true, transaction, this);

	m_MapLock.lock();

	ToolTransactionId id = m_uiLastTransId;
	m_uiLastTransId++;
	m_mTransactions[id] = info;

	m_MapLock.unlock();

	startDownload(info);
	return id;
}


ToolTransactionId ToolManager::installTools(gcRefPtr<Misc::ToolTransaction> transaction)
{
	if (!areAllToolsValid(transaction->getList()))
	{
		safe_delete(transaction);
		return -1;
	}

	if (areAllToolsInstalled(transaction->getList()))
	{
		safe_delete(transaction);
		return -2;
	}

	auto info = gcRefPtr<Misc::ToolTransInfo>::create(false, transaction, this);

	m_MapLock.lock();

	ToolTransactionId ttid = m_uiLastTransId;
	m_uiLastTransId++;
	m_mTransactions[ttid] = info;

	m_MapLock.unlock();


	std::vector<DesuraId> idList;
	info->getIds(idList);

	for (size_t x=0; x<idList.size(); x++)
	{
		DesuraId id = idList[x];
		auto tool = findItem(id.toInt64());

		//This should not happen as there is a check before a install starts to make sure all tool ids are valid
		if (!tool)
			continue;

		if (tool->isInstalled())
			info->removeItem(id);
	}

	startInstall(ttid);
	return ttid;
}



void ToolManager::parseXml(const XML::gcXMLElement &toolinfoNode)
{
	if (!toolinfoNode.IsValid())
		return;

	auto toolsNode = toolinfoNode.FirstChildElement("tools");

	if (!toolsNode.IsValid())
		return;

	auto wcm = gcRefPtr<WildcardManager>::create();

	wcm->onNeedInstallSpecialEvent += delegate(this, &ToolManager::onSpecialCheck);
	wcm->onNeedSpecialEvent += delegate(&m_pUser->getNeedWildCardEvent());

	auto wildcardNode = toolinfoNode.FirstChildElement("wcards");

	if (wildcardNode.IsValid())
		wcm->parseXML(wildcardNode);

	//clear the java path value
	gcRefPtr<WildcardInfo> temp = wcm->findItem("JAVA_EXE");

	if (temp)
	{
		temp->m_szPath = "";
		temp->m_bResolved = true;
	}

	bool is64OS = UTIL::OS::is64OS();

	toolsNode.for_each_child("tool", [this, is64OS, wcm](const XML::gcXMLElement &toolEl)
	{
		bool isTool64 = false;
		toolEl.GetChild("bit64", isTool64);

		if (isTool64 && !is64OS)
			return;

		const std::string id = toolEl.GetAtt("siteareaid");

		if (id.empty())
			return;

		DesuraId tid(id.c_str(), "tools");
		auto tool = this->findItem(tid.toInt64());

		bool bAdd = false;

		if (!tool)
		{
			tool = gcRefPtr<ToolInfo>::create(tid);
			bAdd = true;
		}

		tool->parseXml(toolEl, wcm.get(), m_pUser->getAppDataPath());

		if (bAdd)
			this->addItem(tool);
	});

	postParseXml();
}


void ToolManager::startDownload(gcRefPtr<Misc::ToolTransInfo> info)
{
	if (!info)
		return;

	m_DownloadLock.lock();

	std::vector<DesuraId> idList;
	info->getIds(idList);

	for (size_t x=0; x<idList.size(); x++)
	{
		DesuraId id = idList[x];
		auto tool = findItem(id.toInt64());

		//This should not happen as there is a check before a download starts to make sure all tool ids are valid
		if (!tool)
			continue;

#ifdef WIN32
		tool->checkFile(m_pUser->getAppDataPath());
#endif

		if (tool->isDownloaded() || tool->isInstalled())
		{
			info->removeItem(id);
			continue;
		}

		auto it = m_mDownloads.find(id.toInt64());

		if (it == m_mDownloads.end())
			downloadTool(tool);
		else
			it->second->increseRefCount();
	}

	m_DownloadLock.unlock();
}

void ToolManager::cancelDownload(gcRefPtr<Misc::ToolTransInfo> info, bool force)
{
	if (!info)
		return;

	m_DownloadLock.lock();

	std::vector<DesuraId> idList;
	info->getIds(idList);

	for (size_t x=0; x<idList.size(); x++)
	{
		DesuraId id = idList[x];
		auto info = findItem(id.toInt64());

		//This should not happen as there is a check before a download starts to make sure all tool ids are valid
		if (!info)
			continue;

		auto it = m_mDownloads.find(id.toInt64());

		if (it != m_mDownloads.end())
			it->second->decreaseRefCount(force);
	}

	m_DownloadLock.unlock();
}

void ToolManager::downloadTool(gcRefPtr<ToolInfo> tool)
{
	auto dtt = gcRefPtr<UserCore::Task::DownloadToolTask>::create(m_pUser, tool);

	dtt->onCompleteEvent += extraDelegate(this, &ToolManager::onToolDLComplete, tool->getId());
	dtt->onErrorEvent +=extraDelegate(this, &ToolManager::onToolDLError, tool->getId());
	dtt->onProgressEvent += extraDelegate(this, &ToolManager::onToolDLProgress, tool->getId());

	m_mDownloads[tool->getId().toInt64()] = dtt;

	m_pUser->getThreadPool()->forceTask(dtt);
}

void ToolManager::eraseDownload(DesuraId id)
{
	m_DownloadLock.lock();
	auto it = m_mDownloads.find(id.toInt64());

	if (it != m_mDownloads.end())
		m_mDownloads.erase(it);

	m_DownloadLock.unlock();
}

void ToolManager::onToolDLComplete(DesuraId id)
{
	eraseDownload(id);

	{
		std::lock_guard<std::mutex> al(m_MapLock);
		for_each([id](gcRefPtr<Misc::ToolTransInfo> info){
			info->onDLComplete(id);
		});
	}

	saveItems();
}

void ToolManager::onToolDLError(DesuraId id, gcException &e)
{
	eraseDownload(id);

	if (e.getErrId() == ERR_USERCANCELED)
		return;

	std::lock_guard<std::mutex> al(m_MapLock);

	for_each([id, e](gcRefPtr<Misc::ToolTransInfo> &info){
		info->onDLError(id, e);
	});
}

void ToolManager::onToolDLProgress(DesuraId id, UserCore::Misc::ToolProgress &prog)
{
	std::lock_guard<std::mutex> al(m_MapLock);

	for_each([id, &prog](gcRefPtr<Misc::ToolTransInfo> &info){
		info->onDLProgress(id, prog);
	});
}

bool ToolManager::updateTransaction(ToolTransactionId ttid, gcRefPtr<Misc::ToolTransaction> transaction)
{
	bool found = false;

	m_MapLock.lock();
	std::map<ToolTransactionId, gcRefPtr<Misc::ToolTransInfo>>::iterator it = m_mTransactions.find(ttid);

	if (it != m_mTransactions.end())
	{
		it->second->updateTransaction(transaction);
		found = true;
	}

	m_MapLock.unlock();
	safe_delete(transaction);

	return found;
}

bool ToolManager::areAllToolsValid(const std::vector<DesuraId> &list)
{
	for (auto t : list)
	{
		auto info = findItem(t.toInt64());

		if (!info)
			return false;
	}

	return true;
}

bool ToolManager::areAllToolsDownloaded(const std::vector<DesuraId> &list)
{
	for (auto t : list)
	{
		auto info = findItem(t.toInt64());

		if (!info || (!info->isDownloaded() && !info->isInstalled()))
			return false;
	}

	return true;
}

bool ToolManager::areAllToolsInstalled(const std::vector<DesuraId> &list)
{
	for (auto t : list)
	{
		auto info = findItem(t.toInt64());

		if (!info || !info->isInstalled())
			return false;
	}

	return true;
}


std::string ToolManager::getToolName(DesuraId toolId)
{
	auto info = findItem(toolId.toInt64());

	if (!info)
		return "";

	return info->getName();
}

void ToolManager::startInstall(ToolTransactionId ttid)
{
	if (!m_pToolThread || m_bDeleteThread)
	{
		safe_delete(m_pToolThread);
		m_bDeleteThread = false;

#ifdef WIN32
		m_pToolThread = gcRefPtr<UserCore::Misc::ToolInstallThread>::create(m_pUser->getInternal(), this, m_MapLock, m_mTransactions, m_pUser->getUserName(), m_pUser->getMainWindowHandle());
		m_pToolThread->onPipeDisconnectEvent += delegate(this, &ToolManager::onPipeDisconnect);
#else
		m_pToolThread = gcRefPtr<UserCore::Misc::ToolInstallThread>::create(this, m_MapLock, m_mTransactions);
#endif

		m_pToolThread->onFailedToRunEvent += delegate(this, &ToolManager::onFailedToRun);
		m_pToolThread->start();
	}

	m_pToolThread->startInstall(ttid);
}

void ToolManager::cancelInstall(ToolTransactionId ttid)
{
	if (m_pToolThread)
		m_pToolThread->cancelInstall(ttid);
}

void ToolManager::invalidateTools(std::vector<DesuraId> &list)
{
	for (size_t x=0; x<list.size(); x++)
	{
		auto info = findItem(list[x].toInt64());

		if (info)
			info->setInstalled(false);
	}
}

void ToolManager::onFailedToRun()
{
	//cant delete thread strait up as it will result in deadlock
	m_bDeleteThread = true;
}

#ifdef WIN32
void ToolManager::postParseXml()
{
}
#endif



void ToolManager::reloadTools(DesuraId id)
{
	//missing tools. Gather info again
	XML::gcXMLDocument doc;

	m_pUser->getWebCore()->getItemInfo(id, doc, MCFBranch(), MCFBuild());

	auto uNode = doc.GetRoot("iteminfo");

	if (!uNode.IsValid())
		return;

	uint32 ver = doc.ProcessStatus("iteminfo");

	if (ver == 1)
	{
		auto toolNode = uNode.FirstChildElement("toolinfo");

		if (toolNode.IsValid())
			parseXml(toolNode);
	}
	else
	{
		uNode.FirstChildElement("platforms").for_each_child("platform", [&](const XML::gcXMLElement &platform)
		{
			if (!m_pUser->platformFilter(platform, PlatformType::Tool))
				parseXml(platform.FirstChildElement("toolinfo"));
		});
	}
}

