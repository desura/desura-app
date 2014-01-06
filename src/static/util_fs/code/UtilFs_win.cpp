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
#include "util/UtilFs.h"

namespace UTIL
{
namespace FS
{

#ifdef WIN32
	#include <winioctl.h>

FileHandle::FileHandle(const FileHandle& handle)
{
	DuplicateHandle( GetCurrentProcess(), handle.getHandle(), GetCurrentProcess(), &m_hFileHandle, 0, TRUE, DUPLICATE_SAME_ACCESS );
	m_bIsOpen = handle.isOpen();
}

FileHandle& FileHandle::operator=(const FileHandle& handle)
{
	if (this != &handle)
	{
		DuplicateHandle( GetCurrentProcess(), handle.getHandle(), GetCurrentProcess(), &m_hFileHandle, 0, TRUE, DUPLICATE_SAME_ACCESS );
		m_bIsOpen = handle.isOpen();
	}
	
	return *this;
}

void FileHandle::open(const char* fileName, FILE_MODE mode, uint64 offset)
{
	if (m_bIsOpen)
		close();

	m_uiOffset = offset;

	HANDLE fh = nullptr;

	DWORD dwDesiredAccess  = GENERIC_READ;
	DWORD dwShareMode = FILE_SHARE_READ;
	DWORD dwCreationDisposition = OPEN_EXISTING;

	switch (mode)
	{
	case FILE_READ:
		break;

	case FILE_WRITE:
		dwDesiredAccess  = GENERIC_WRITE;
		dwShareMode = 0;
		dwCreationDisposition = CREATE_ALWAYS;
		break;

	case FILE_APPEND:
		dwDesiredAccess  = GENERIC_WRITE;
		dwShareMode = 0;
		dwCreationDisposition = OPEN_ALWAYS;
		break;

	default:
		throw gcException(ERR_INVALID, "The mode was invalid");
		break;
	}

	gcWString file(fileName);
	fh = CreateFileW(file.c_str(), dwDesiredAccess, dwShareMode, nullptr, dwCreationDisposition, 0, nullptr);

	if (!fh || fh == INVALID_HANDLE_VALUE)
		throw gcException(ERR_INVALIDFILE, GetLastError(), gcString("Failed to open the file '{0}'", fileName));

	m_hFileHandle = fh;
	m_bIsOpen = true;

	if (mode == FILE_APPEND)
	{
		DWORD high = 0;
		DWORD low = GetFileSize(fh, &high);

		uint64 pos = (((uint64)high)<<32) + (uint64)low;
		seek(m_uiOffset + pos);
	}
}

void FileHandle::close()
{
	if (!m_bIsOpen)
		return;

	if (m_hFileHandle)
		CloseHandle(m_hFileHandle);

	m_hFileHandle = nullptr;
	m_bIsOpen = false;
}

void FileHandle::read(char* buff, uint32 size)
{
	if (!m_bIsOpen || !m_hFileHandle)
		throw gcException(ERR_NULLHANDLE);

	DWORD dwRead = 0;
	uint8 readRet = ReadFile(m_hFileHandle, buff, size, &dwRead, nullptr);

	int err = GetLastError();

	if (readRet == FALSE)
		throw gcException(ERR_FAILEDREAD, err, gcString("Failed to read the file [{0}]", err));

	if (dwRead != size)
		throw gcException(ERR_PARTREAD, err);
}

void FileHandle::write(const char* buff, uint32 size)
{
	if (size == 0)
		return;

	if (!m_bIsOpen || !m_hFileHandle)
		throw gcException(ERR_NULLHANDLE);

	DWORD dwWrite = 0;
	uint8 writeRet = WriteFile(m_hFileHandle, buff, size, &dwWrite, nullptr);

	int err = GetLastError();

	if (writeRet == FALSE)
	{
		if (err == ERROR_DISK_FULL)
			throw gcException(ERR_FAILEDWRITE, err, "Failed to write. HDD is full");

		throw gcException(ERR_FAILEDWRITE, err);
	}

	if ((uint32)dwWrite != size)
		throw gcException(ERR_PARTWRITE, err);
}

void FileHandle::seek(uint64 pos)
{
	if (!m_bIsOpen || !m_hFileHandle)
		throw gcException(ERR_NULLHANDLE);

	LARGE_INTEGER mov;
	mov.QuadPart = m_uiOffset + pos;

	BOOL res = SetFilePointerEx(m_hFileHandle, mov, nullptr, FILE_BEGIN);

	if (res == 0)
		throw gcException(ERR_FAILEDSEEK, GetLastError());
}




#endif

}
}
