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

#ifndef DESURA_SMWORKERTHREAD_H
#define DESURA_SMWORKERTHREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "BZip2.h"
#include "util_thread/BaseThread.h"

class MD5Progressive;

namespace MCFCore
{
class MCFFile;

namespace Misc 
{
	class ProgressiveCRC;
}

namespace Thread
{
class SMTController;




//! Save MCF Worker thread. Does the work of compressing the files and writing them to the mcf
//!
class SMTWorker : public ::Thread::BaseThread
{
public:
	//! Constuctor
	//!
	//! @param controller Parent controller
	//! @param id Worker id
	//! @param fileHandle Handle to the mcf file to save the data to
	//!
	SMTWorker(SMTController* controller, uint32 id, UTIL::FS::FileHandle* fileHandle);
	~SMTWorker();

protected:
	void run();

	//! Compresses and saves the files to the mcf
	//!
	void doWork();

	//! Starts a new task
	//!
	//! @return True if has new task, false if dont
	//!
	bool newTask();

	//! Finsihes current task
	//!
	void finishTask();

	void writeFile(const char* buff, uint32 buffSize, bool endFile);
	void doCompression(const char* buff, uint32 buffSize, bool endFile);

private:
	MD5Progressive* m_pMD5Norm;
	MD5Progressive* m_pMD5Comp;

	MCFCore::Misc::ProgressiveCRC* m_pCRC;

	uint64 m_uiCurOffset;
	uint64 m_uiDiffCurOffset;

	uint64 m_uiTotRead;
	uint64 m_uiCompressSize;
	uint64 m_uiTotFileRead;

	uint32 m_uiId;

	SMTController *m_pCT;

	std::shared_ptr<MCFCore::MCFFile> m_pCurFile;
	UTIL::MISC::BZ2Worker *m_BZ2Worker;

	UTIL::FS::FileHandle m_hFhSource;
	UTIL::FS::FileHandle* m_phFhSink;
};

}
}


#endif //DESURA_SMWORKERTHREAD_H
