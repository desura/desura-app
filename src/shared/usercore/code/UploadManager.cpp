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

#include "Common.h"
#include "UploadManager.h"
#include "sqlite3x.hpp"
#include "sql/McfUploadSql.h"
#include "User.h"


using namespace UserCore;
using namespace UserCore::Thread;


UploadManager::UploadManager(gcRefPtr<UserCore::UserI> userCore) 
	: BaseManager()
	, m_pUserCore(userCore)
{
	const char* appDataPath = m_pUserCore->getAppDataPath();
	createMcfUploadDbTables(appDataPath);

	updateItemIds();
	load();
}


void UploadManager::updateItemIds()
{
	const char* appDataPath = m_pUserCore->getAppDataPath();
	gcString szItemDb = getMcfUploadDb(appDataPath);

	try
	{
		sqlite3x::sqlite3_connection db(szItemDb.c_str());
		sqlite3x::sqlite3_command cmd(db, "SELECT internalid FROM mcfupload;");
		sqlite3x::sqlite3_reader reader = cmd.executereader();

		while(reader.read()) 
		{
			DesuraId id(reader.getint64(0));

			sqlite3x::sqlite3_command cmd(db, "UPDATE mcfupload SET internalid=? WHERE internalid=?;");
			cmd.bind(1, (long long int)id.toInt64());
			cmd.bind(2, (long long int)reader.getint64(0));
			cmd.executenonquery();
		}
	}
	catch (std::exception &)
	{
	}
}



//if a upload is in progress this returns the file for a given key
const char* UploadManager::findUpload(const char* key)
{
	gcRefPtr<UploadInfoThread> temp;

	{
		std::lock_guard<std::mutex> guard(m_mMutex);
		temp = BaseManager::findItem(key);
	}

	if (temp && !temp->isDeleted())
		return temp->getFile();
	
	return nullptr;
}

//lazy delete
void UploadManager::removeUpload(const char* key, bool stopThread)
{
	gcRefPtr<UploadInfoThread> temp;

	{
		std::lock_guard<std::mutex> guard(m_mMutex);
		temp = BaseManager::findItem(key);
	}

	if (temp)
	{
		if (stopThread)
			temp->stop();

		temp->setDeleted();
	}

	const char* appDataPath = m_pUserCore->getAppDataPath();
	gcString szItemDb = getMcfUploadDb(appDataPath);

	try
	{
		sqlite3x::sqlite3_connection db(szItemDb.c_str());
		sqlite3x::sqlite3_command cmd(db, "DELETE FROM mcfupload WHERE key=?;");

		cmd.bind(1, std::string(key));
		cmd.executenonquery();
	}
	catch (std::exception &e)
	{
		Warning("Failed to delete mcf upload: {0}\n", e.what());
	}

	onUpdateEvent();
}


uint64 UploadManager::addUpload(DesuraId id, const char* key, const char* path)
{
	gcRefPtr<UploadInfoThread> ui;

	{
		std::lock_guard<std::mutex> guard(m_mMutex);
		auto ui = BaseManager::findItem(key);

		if (ui && !ui->isDeleted())
			return ui->getHash();

		ui = gcRefPtr<UploadInfoThread>::create(id, key, path);

		ui->setWebCore(m_pUserCore->getWebCore());
		ui->setUserCore(m_pUserCore);
		ui->setUpLoadManager(this);

		addItem(ui);
	}

	const char* appDataPath = m_pUserCore->getAppDataPath();
	gcString szItemDb = getMcfUploadDb(appDataPath);

	try
	{
		sqlite3x::sqlite3_connection db(szItemDb.c_str());
		sqlite3x::sqlite3_command cmd(db, "INSERT INTO mcfupload (key, file, internalid) VALUES (?,?,?);");

		cmd.bind(1, std::string(key));
		cmd.bind(2, UTIL::OS::getRelativePath(path));
		cmd.bind(3, (long long int)id.toInt64());

		cmd.executenonquery();
	}
	catch (std::exception &e)
	{
		Warning("Failed to add mcf upload: {0}\n", e.what());
	}

	onUpdateEvent();
	return ui->getHash();
}


void UploadManager::load()
{
	std::lock_guard<std::mutex> guard(m_mMutex);

	const char* appDataPath = m_pUserCore->getAppDataPath();
	gcString szItemDb = getMcfUploadDb(appDataPath);

	try
	{
		sqlite3x::sqlite3_connection db(szItemDb.c_str());
		sqlite3x::sqlite3_command cmd(db, "SELECT key, file, internalid FROM mcfupload;");

		sqlite3x::sqlite3_reader reader = cmd.executereader();

		while (reader.read())
		{
			DesuraId id(reader.getint64(2));
			std::string key = reader.getstring(0);
			std::string file = UTIL::OS::getAbsPath(reader.getstring(1));

			if (key != "" && file != "" && id != 0)
			{
				auto temp = BaseManager<UserCore::Thread::UploadInfoThread>::findItem(key.c_str());

				if (!temp)
				{
					temp = gcRefPtr<UserCore::Thread::UploadInfoThread>::create(id, key.c_str(), file.c_str());
					temp->setWebCore(m_pUserCore->getWebCore());
					temp->setUserCore(m_pUserCore);
					temp->setUpLoadManager(this);
					addItem( temp );
				}
			}
		}
	}
	catch (std::exception &e)
	{
		Warning("Failed to load mcf uploads: {0}\n", e.what());
	}
}