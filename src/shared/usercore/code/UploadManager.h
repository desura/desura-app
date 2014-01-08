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

#ifndef DESURA_UPLOADMANAGER_H
#define DESURA_UPLOADMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "usercore/UploadManagerI.h"
#include "BaseManager.h"
#include "util_thread/BaseThread.h"
#include "UploadInfoThread.h"

namespace UserCore
{

class UserI;

class UploadManager : public UploadManagerI, public BaseManager<UserCore::Thread::UploadInfoThread>
{
public:
	UploadManager(UserCore::UserI* userCore);

	//if a upload is in progress this returns the file for a given key
	const char* findUpload(const char* key);

	//removes an upload given a key
	void removeUpload(const char* key, bool stopThread = true);

	//add new upload given info
	uint64 addUpload(DesuraId id, const char* key, const char* path);

	UserCore::Misc::UploadInfoThreadI* findItem(uint64 hash)
	{
		return BaseManager<UserCore::Thread::UploadInfoThread>::findItem(hash);
	}

	UserCore::Misc::UploadInfoThreadI* findItem(const char* key)
	{
		return BaseManager<UserCore::Thread::UploadInfoThread>::findItem(key);
	}

	virtual EventV* getUpdateEvent()
	{
		return &onUpdateEvent;
	}

	virtual UserCore::Misc::UploadInfoThreadI* getItem(uint32 index)
	{
		return BaseManager<UserCore::Thread::UploadInfoThread>::getItem(index);
	}

	virtual uint32 getCount()
	{
		return BaseManager<UserCore::Thread::UploadInfoThread>::getCount();
	}

	EventV onUpdateEvent;

protected:	
	uint8 saveToFile(const char* file){return 0;}
	uint8 loadFromFile(const char* file){return 0;} //doesnt return any thing but due to parent class must return uint8

	void load();
	void updateItemIds();

private:
	UserCore::UserI* m_pUserCore;
	std::mutex m_mMutex;
};

}

#endif //DESURA_UPLOADMANAGER_H
