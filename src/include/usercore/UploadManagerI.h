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


#ifndef DESURA_UPLOADMANAGERI_H
#define DESURA_UPLOADMANAGERI_H
#ifdef _WIN32
#pragma once
#endif

#include "usercore/UploadInfoThreadI.h"

namespace UserCore
{
	namespace Thread
	{
		class UploadInfoThread;
	}

	//! Handles the status of current mcf uploads
	//!
	class UploadManagerI : public gcRefBase
	{
	public:
		virtual ~UploadManagerI(){}

		//! If a upload is in progress this returns the file for a given key
		//!
		//! @param key Upload key
		//! @return Upload mcf file
		//!
		virtual const char* findUpload(const char* key) = 0;

		//! Removes an upload given a key
		//!
		//! @param key Upload key
		//! @param stopThread Stop the upload thread
		//!
		virtual void removeUpload(const char* key, bool stopThread = true) = 0;

		//! Add new upload given info
		//!
		//! @param id Item id
		//! @param key Upload key
		//! @param path Upload mcf path
		//! @return Upload hash
		//!
		virtual uint64 addUpload(DesuraId id, const char* key, const char* path) = 0;

		//! Finds an upload thread
		//!
		//! @param hash Upload hash
		//! @return Upload thread
		//!
		virtual gcRefPtr<UserCore::Misc::UploadInfoThreadI> findItem(uint64 hash) = 0;
		virtual gcRefPtr<UserCore::Misc::UploadInfoThreadI> findItem(const char* key) = 0;

		//! Gets an upload item at an index
		//!
		//! @param index Item index
		//! @return Upload thread
		//!
		virtual gcRefPtr<UserCore::Misc::UploadInfoThreadI> getItem(uint32 index) = 0;

		//! Gets the upload count
		//!
		//! @return Upload count
		//!
		virtual uint32 getCount() = 0;

		//! Gets the Upload update event handler. Triggers when an upload updates
		//!
		//! @return Update event
		//!
		virtual EventV& getUpdateEvent() = 0;
	};
}


#endif //DESURA_UPLOADMANAGERI_H
