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

#ifndef DESURA_CHANGEDIRTHREAD_H
#define DESURA_CHANGEDIRTHREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "util_thread\BaseThread.h"
#include "sqlite3x.hpp"
#include "usercore\UserCoreI.h"

typedef std::pair<UTIL::FS::Path,UTIL::FS::Path> FilePair;
typedef std::vector<FilePair> FileList;

class ChangeDirThread : public ::Thread::BaseThread
{
public:
	ChangeDirThread(const char* dest, gcRefPtr<UserCore::UserI> user);
	~ChangeDirThread();

	Event<gcException> onErrorEvent;

	Event<uint32> onProgressEvent;
	Event<uint32> onStageEvent;

	EventV onCompleteEvent;

protected:
	virtual void run();
	virtual void onStop();

	void copyFiles(FileList &list);
	void updateDb(FileList &list);

	void getListOfFiles(UTIL::FS::Path path, std::vector<UTIL::FS::Path> &outList);
	void fixFilePath(UTIL::FS::Path &file);

	void fixMcfUploads(UTIL::FS::Path dest, FileList &list);
	void fixMcfStore(UTIL::FS::Path dest, FileList &list);

	void delLeftOverMcf();
	void delLeftOverMcf(UTIL::FS::Path path, std::vector<std::string> &extList);

private:
	gcString m_szDest;
	gcString m_szCurDir;

	bool m_bStopped;
	bool m_bCompleted;
	gcRefPtr<UserCore::UserI> m_pUser;
};


#endif //DESURA_CHANGEDIRTHREAD_H
