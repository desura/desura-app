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
#include "CrashDumpThread.h"

#include "managers\CVar.h"
#include "MiniDumpGenerator_Extern.h"

CVar gc_lastusername("gc_lastusername", "");
const char* g_szDesura = "desura.exe";
const char* g_szDesuraService = "desura_service.exe";

CrashDumpThread::CrashDumpThread() : Thread::BaseThread("CrashDump Thread")
{
	UTIL::FS::Path path(UTIL::OS::getAppDataPath(L"\\dumps\\"), L"", false);
	UTIL::FS::recMakeFolder(path);

	m_szDumpPath = path.getFullPath();

	uploadCrash = nullptr;
	uploadCrashProg = nullptr;

	m_bSecond = false;
}

CrashDumpThread::~CrashDumpThread()
{
	m_DelMutex.lock();
	m_DelMutex.unlock();
}


bool CrashDumpThread::loadCrashReporter()
{
	if (!sol.load("crashuploader.dll"))
		return false;

	uploadCrash = sol.getFunction<UploadCrashFn>("UploadCrash");

	if (!uploadCrash)
		return false;

	uploadCrashProg = sol.getFunction<UploadCrashProgFn>("UploadCrashProg");

	if (!uploadCrashProg)
		return false;

	uploadCrashProgEx = sol.getFunction<UploadCrashProgExFn>("UploadCrashProgEx");
	return true;
}

void CrashDumpThread::uploadDump(const char* dumpFile)
{
	if (!uploadCrashProg)
		return;

	int build = 0;
	int branch = 0;

	std::string szAppid = UTIL::WIN::getRegValue(APPID);
	std::string szAppBuild = UTIL::WIN::getRegValue(APPBUILD);

	if (szAppid.size() > 0)
		branch = Safe::atoi(szAppid.c_str());

	if (szAppBuild.size() > 0)
		build = Safe::atoi(szAppBuild.c_str());

#ifdef TRACER_SHARED_MEM_NAME
	if (uploadCrashProgEx)
	{
		gcString strTracerName(TRACER_SHARED_MEM_NAME);
		uploadCrashProgEx(dumpFile, gc_lastusername.getString(), build, branch, delegate(this, &CrashDumpThread::uploadProgress), strTracerName.c_str());
		return;
	}
#endif

	uploadCrashProg(dumpFile, gc_lastusername.getString(), build, branch, delegate(this, &CrashDumpThread::uploadProgress));
}


void CrashDumpThread::run()
{
	std::lock_guard<std::mutex> al(m_DelMutex);

	if (!loadCrashReporter())
	{
		gcException e(ERR_NULLHANDLE, "Failed to load utilcore.dll or related function");
		onErrorEvent(e);
		return;
	}

	try
	{
		gcString res = generateDump(g_szDesura);
		uploadDump(res.c_str());
	}
	catch (gcException &e)
	{
		onErrorEvent(e);
		return;
	}

	m_bSecond = true;

	try
	{
		gcString res = generateDump(g_szDesuraService);
		uploadDump(res.c_str());
	}
	catch (gcException &e)
	{
		if (e.getSecErrId() == 0)
		{
			onErrorEvent(e);
			return;
		}
	}

	uint8 prog = 100;
	onProgressEvent(prog);
	onCompleteEvent();
}

void CrashDumpThread::uploadProgress(Prog_s& prog)
{
	uint8 total = 0;

	if (m_bSecond)
		total += 50;

	total += (uint8)(prog.ulnow*50.0/prog.ultotal);
	onProgressEvent(total);

	prog.abort = isStopped();
}

gcString CrashDumpThread::generateDump(const char* exeName)
{
	MiniDumpGenerator_Extern desuraMDG(exeName, m_szDumpPath.c_str());

	int dRes = desuraMDG.generate();

	if (dRes == -1)
		throw gcException(ERR_NULLHANDLE, dRes, gcString("Failed to find {0}. Is it running?", exeName));
	else if (dRes == 0)
		throw gcException(ERR_NULLHANDLE, dRes, gcString("Failed to produce a dump of {0}.", exeName));

	return gcString("{0}{1}{2}", m_szDumpPath, DIRS_STR, desuraMDG.getSavedFile());
}




