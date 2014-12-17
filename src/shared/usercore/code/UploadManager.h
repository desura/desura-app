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
		UploadManager(gcRefPtr<UserCore::UserI> userCore);

		//if a upload is in progress this returns the file for a given key
		const char* findUpload(const char* key) override;

		//removes an upload given a key
		void removeUpload(const char* key, bool stopThread = true) override;

		//add new upload given info
		uint64 addUpload(DesuraId id, const char* key, const char* path) override;

		gcRefPtr<UserCore::Misc::UploadInfoThreadI> findItem(uint64 hash) override
		{
			return BaseManager<UserCore::Thread::UploadInfoThread>::findItem(hash);
		}

		gcRefPtr<UserCore::Misc::UploadInfoThreadI> findItem(const char* key) override
		{
			return BaseManager<UserCore::Thread::UploadInfoThread>::findItem(key);
		}

		EventV& getUpdateEvent() override
		{
			return onUpdateEvent;
		}

		gcRefPtr<UserCore::Misc::UploadInfoThreadI> getItem(uint32 index) override
		{
			return BaseManager<UserCore::Thread::UploadInfoThread>::getItem(index);
		}

		uint32 getCount() override
		{
			return BaseManager<UserCore::Thread::UploadInfoThread>::getCount();
		}

		EventV onUpdateEvent;

		void cleanup();

		gc_IMPLEMENT_REFCOUNTING(UploadManager);

	protected:
		uint8 saveToFile(const char* file){ return 0; }
		uint8 loadFromFile(const char* file){ return 0; } //doesnt return any thing but due to parent class must return uint8

		void load();
		void updateItemIds();

	private:
		gcRefPtr<UserCore::UserI> m_pUserCore;
		std::mutex m_mMutex;
	};
}

#endif //DESURA_UPLOADMANAGER_H
