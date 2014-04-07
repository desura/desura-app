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

#ifndef DESURA_CRASHDUMPTHREAD_H
#define DESURA_CRASHDUMPTHREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "util_thread\BaseThread.h"
#include "SharedObjectLoader.h"

#include "Event.h"

typedef bool (*UploadCrashFn)(const char* path, const char* user, int build, int branch);
typedef bool (*UploadCrashProgFn)(const char* path, const char* user, int build, int branch, DelegateI<Prog_s&>* prog);

class CrashDumpThread : public Thread::BaseThread
{
public:
	CrashDumpThread();
	~CrashDumpThread();

	EventV onCompleteEvent;
	Event<gcException> onErrorEvent;
	Event<uint8> onProgressEvent;

protected:
	virtual void run();

	void uploadDump(const char* dumpFile);
	gcString generateDump(const char* exeName);

	bool loadCrashReporter();
	void uploadProgress(Prog_s& prog);


	UploadCrashFn uploadCrash;
	UploadCrashProgFn uploadCrashProg;

	gcString m_szDumpPath;

private:
	uint32 m_bSecond;
	SharedObjectLoader sol;

	std::mutex m_DelMutex;
};

#endif //DESURA_CRASHDUMPTHREAD_H
