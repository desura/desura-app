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
typedef bool (*UploadCrashProgExFn)(const char* path, const char* user, int build, int branch, DelegateI<Prog_s&>* prog, const char* szTracer);

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


	UploadCrashFn uploadCrash = nullptr;
	UploadCrashProgFn uploadCrashProg = nullptr;
	UploadCrashProgExFn uploadCrashProgEx = nullptr;

	gcString m_szDumpPath;

private:
	uint32 m_bSecond;
	SharedObjectLoader sol;

	std::mutex m_DelMutex;
};

#endif //DESURA_CRASHDUMPTHREAD_H
