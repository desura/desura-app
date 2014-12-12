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


#ifndef DESURA_JSFS_H
#define DESURA_JSFS_H
#ifdef _WIN32
#pragma once
#endif

#include "gcJSBase.h"

class FileSystemJSBinding : public DesuraJSBase<FileSystemJSBinding>
{
public:
	FileSystemJSBinding();
	~FileSystemJSBinding();

protected:
	bool IsValidFile(gcString file);
	bool IsValidFolder(gcString folder);

	bool CopyFile(gcString file, gcString dest);
	bool DeleteFile(gcString file);

	bool DeleteFolder(gcString path);

	bool SetFolderPermissions(gcString folder);

	int32 GetFileSize(gcString file);

	int32 OpenFileForWrite(gcString file);
	bool WriteFile(int32 handle, gcString string);
	void CloseFile(int32 handle);

private:
	std::vector<UTIL::FS::FileHandle*> m_vFileHandles;
};



#endif //DESURA_JSFS_H
