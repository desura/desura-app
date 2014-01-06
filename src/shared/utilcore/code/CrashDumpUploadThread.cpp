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
}