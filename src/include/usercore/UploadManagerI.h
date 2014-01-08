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
class UploadManagerI
{
public:
	virtual ~UploadManagerI(){;}

	//! If a upload is in progress this returns the file for a given key
	//!
	//! @param key Upload key
	//! @return Upload mcf file
	//!
	virtual const char* findUpload(const char* key)=0;

	//! Removes an upload given a key
	//! 
	//! @param key Upload key
	//! @param stopThread Stop the upload thread
	//!
	virtual void removeUpload(const char* key, bool stopThread = true)=0;

	//! Add new upload given info
	//!
	//! @param id Item id
	//! @param key Upload key
	//! @param path Upload mcf path
	//! @return Upload hash
	//!
	virtual uint64 addUpload(DesuraId id, const char* key, const char* path)=0;

	//! Finds an upload thread
	//!
	//! @param hash Upload hash
	//! @return Upload thread
	//!
	virtual UserCore::Misc::UploadInfoThreadI* findItem(uint64 hash)=0;
	virtual UserCore::Misc::UploadInfoThreadI* findItem(const char* key)=0;

	//! Gets an upload item at an index
	//!
	//! @param index Item index
	//! @return Upload thread
	//!
	virtual UserCore::Misc::UploadInfoThreadI* getItem(uint32 index)=0;

	//! Gets the upload count
	//!
	//! @return Upload count
	//!
	virtual uint32 getCount()=0;

	//! Gets the Upload update event handler. Triggers when an upload updates
	//!
	//! @return Update event
	//!
	virtual EventV* getUpdateEvent()=0;
};

}


#endif //DESURA_UPLOADMANAGERI_H
