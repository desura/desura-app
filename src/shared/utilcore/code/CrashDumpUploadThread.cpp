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
#include "CrashDumpUploadThread.h"

#include "managers\CVar.h"

extern CVar gc_lastusername;

CrashDumpUploadThread::CrashDumpUploadThread()
{

}

void CrashDumpUploadThread::run()
{
	if (!loadCrashReporter())
		return;

	std::vector<UTIL::FS::Path> files;
	std::vector<std::string> filter;

	filter.push_back("dmp");
	filter.push_back("bz2");

	UTIL::FS::getAllFiles(UTIL::FS::PathWithFile(m_szDumpPath), files, &filter);

	for (size_t x=0; x<files.size(); x++)
	{
		uploadCrash(files[x].getFullPath().c_str(), gc_lastusername.getString(), -1, -1);

		uint8 prog = x*100/files.size();
		onProgressEvent(prog);
	}

	uint8 tot = 100;
	onProgressEvent(tot);
}
