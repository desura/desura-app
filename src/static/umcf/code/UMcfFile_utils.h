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


#ifndef DESURA_UMCF_FILE_UTILS_H
#define DESURA_UMCF_FILE_UTILS_H
#ifdef _WIN32
#pragma once
#endif

#define FILE_WRITE 1
#define FILE_READ 2

#define BUFFSIZE (10*1024)

#ifdef WIN32
	typedef HANDLE FILEHANDLE;
#else
	typedef FILE* FILEHANDLE;
#endif

bool FileOpen(FILEHANDLE &fh, const wchar_t* path, uint32 mode);
bool FileSeek(FILEHANDLE fh, uint64 pos);
bool FileRead(FILEHANDLE fh, uint32 size, char* buff);
bool FileWrite(FILEHANDLE fh, uint32 size, char* buff);

void FileDelete(const wchar_t* file);
void FileClose(FILEHANDLE fh);
uint64 FileSize(FILEHANDLE fh);

bool IsValidFileHandle(FILEHANDLE fh);
bool CreateDir(const wchar_t* path);

#endif //DESURA_UMCF_FILE_UTILS_H
