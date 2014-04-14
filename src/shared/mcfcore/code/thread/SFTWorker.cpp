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
#include "util_thread/BaseThread.h"
#include "thread/SFTWorker.h"
#include "thread/SFTController.h"
#include "mcf/MCFFile.h"

#include <time.h>
#include <time.h>

#ifdef NIX
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace MCFCore 
{
namespace Thread
{

SFTWorker::SFTWorker(SFTController* controller, uint32 id) : BaseThread( "SafeFiles Thread" )
{
	m_uiId = id;
	m_pCT = controller;

	m_pCurFile = nullptr;
	m_pBzs = nullptr;
}

SFTWorker::~SFTWorker()
{
	stop();
	safe_delete(m_pBzs);
}

void SFTWorker::run()
{
	gcAssert(m_pCT);

	while (true)
	{
		auto status = m_pCT->getStatus(m_uiId);
		int32 res = BZ_OK;

		while (status == MCFThreadStatus::SF_STATUS_PAUSE)
		{
			gcSleep(500);
			status = m_pCT->getStatus(m_uiId);
		}

		if (status == MCFThreadStatus::SF_STATUS_STOP)
		{
			break;
		}
		else if (status == MCFThreadStatus::SF_STATUS_NULL)
		{
			if (!newTask())
				gcSleep(500);

			continue;
		}
		else if (status == MCFThreadStatus::SF_STATUS_ENDFILE)
		{
			do
			{
				res = doWork();
				if (bzErrorCheck(res))
					break;
			}
			while (res != BZ_STREAM_END);
		}
		else if (status == MCFThreadStatus::SF_STATUS_CONTINUE)
		{
			res = doWork();
			bzErrorCheck(res);
		}

		if (res == BZ_STREAM_END || status == MCFThreadStatus::SF_STATUS_SKIP)
			finishFile();
	}
}

static gcTime parseTimeStamp(gcString &str)
{
	//Boost throws exception if no time string. :(
	if (str.find('T') == std::string::npos)
	{
		str = str.substr(0, 8) + "T" + str.substr(8);
	}

	return gcTime::from_iso_string(str);
}

void SFTWorker::finishFile()
{
	m_hFh.close();

	std::string file = m_pCurFile->getFullPath();
	gcString str("{0}", m_pCurFile->getTimeStamp());

	try
	{
		UTIL::FS::setLastWriteTime(file, parseTimeStamp(str));
	}
	catch (...)
	{
		Warning("Failed to change {0} time stamp to {1}.\n", m_pCurFile->getName(), str);
	}

	safe_delete(m_pBzs);

	std::string hash = "Failed to generate hash";

#ifdef NIX
	bool isWinExe = false;
	
	if (file.size() > 4)
	{
		std::string lastFour(file.end() - 4, file.end());
		std::transform(lastFour.begin(), lastFour.end(), lastFour.begin(), ::toupper);
		if (lastFour == ".EXE")
			isWinExe = true;
	}
	
	//( && stat(file.c_str(), &s) == 0) 
	
	bool isExecutable = HasAnyFlags(m_pCurFile->getFlags(), MCFCore::MCFFileI::FLAG_XECUTABLE);

	if (isExecutable || isWinExe)
		chmod(file.c_str(), (S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH));
#endif

	if (m_pCurFile->isZeroSize() || m_pCurFile->hashCheckFile(&hash))
	{
		m_pCT->endTask(m_uiId, MCFThreadStatus::SF_STATUS_COMPLETE);
	}
	else
	{
		Warning("Hash check failed for file [{0}]: Cur: {1} !=  Should: {2}\n", m_pCurFile->getName(), hash, m_pCurFile->getCsum());
		m_pCT->endTask(m_uiId, MCFThreadStatus::SF_STATUS_HASHMISSMATCH);
	}
}

int32 SFTWorker::doWork()
{
	if (m_pCurFile->isZeroSize())
		return BZ_STREAM_END;

	MCFThreadStatus status = MCFThreadStatus::SF_STATUS_NULL;
	std::shared_ptr<SFTWorkerBuffer> temp(m_pCT->getBlock(m_uiId, status));

	bool endFile = (status == MCFThreadStatus::SF_STATUS_ENDFILE);

	//if temp is null we are waiting on data to be read. Lets nap for a bit
	if (!temp)
	{
		if (endFile)
		{
			if (m_pCurFile->isCompressed())
				return doDecompression(nullptr, 0, true);

			return BZ_STREAM_END;
		}
		else
		{
			//gcSleep(100);
			return BZ_OK;
		}
	}

	if (m_pCurFile->isCompressed())
		return doDecompression(temp->buff, temp->size, false);

	return doWrite(temp->buff, temp->size);
}

int SFTWorker::doDecompression(const char* buff, uint32 buffSize, bool endFile)
{
	if (!m_pBzs)
	{
		gcException e(ERR_BZ2, 0, "Bzip2 handle was nullptr");
		return reportError(BZ_STREAM_END, e);
	}

	m_pBzs->write(buff, buffSize, endFile);

	try
	{
		m_pBzs->doWork();
	}
	catch (gcException &e)
	{
		return reportError(BZ_STREAM_END, e);
	}

	size_t outBuffSize = m_pBzs->getReadSize();

	if (outBuffSize == 0)
		return m_pBzs->getLastStatus();

	AutoDelete<char> outBuff(new char[outBuffSize]);

	m_pBzs->read(outBuff, outBuffSize);
	int32 res = doWrite(outBuff, outBuffSize);

	if (res == BZ_OK)
		return m_pBzs->getLastStatus();

	return res;
}

int32 SFTWorker::doWrite(const char* buff, uint32 buffSize)
{
	try
	{
		m_hFh.write(buff, buffSize);
	}
	catch (gcException &e)
	{
		return reportError(BZ_STREAM_END, e);
	}

	m_pCT->reportProgress(m_uiId, buffSize);
	m_pCT->pokeThread();

	return BZ_OK;
}

int32 SFTWorker::reportError(int32 bz2Code, gcException &e)
{
	m_pCT->reportError(m_uiId, e);
	m_pCT->pokeThread();

	m_hFh.close();
	safe_delete(m_pBzs);

	return bz2Code;
}

bool SFTWorker::newTask()
{
	m_pCurFile = m_pCT->newTask(m_uiId);

	if (!m_pCurFile)
		return false;

	std::string file = m_pCurFile->getFullPath();

	safe_delete(m_pBzs);
	m_pBzs = new UTIL::MISC::BZ2Worker(UTIL::MISC::BZ2_DECOMPRESS);

	int res = 0;
	if (m_pBzs->isInit(res) == false)
	{
		gcException e(ERR_BZ2DFAIL, res, gcString("Failed to init bzip2 decompression for file {0}", file));
		return reportError(false, e)?true:false;
	}

	UTIL::FS::Path path(file, "", true);
	UTIL::FS::recMakeFolder(path);

	try
	{
		m_hFh.open(path, UTIL::FS::FILE_WRITE);
	}
	catch (gcException &e)
	{
		return reportError(false, e)?true:false;
	}

	return true;
}

bool SFTWorker::bzErrorCheck(int32 bzStatus)
{
	if (bzStatus == BZ_OK || bzStatus == BZ_STREAM_END)
		return false;

	gcException e(ERR_BZ2, bzStatus, gcString("Failed bz2 error check for file {0}", m_pCurFile->getFullPath()));
	return reportError(true, e)?true:false;
}


}
}


#ifdef WITH_GTEST

#include <gtest/gtest.h>

namespace UnitTest
{
	TEST(SFTWorker, ParseTimeStamp)
	{
		tm t = {0};
		t.tm_hour = 8;
		t.tm_min = 6;
		t.tm_sec = 56;

		t.tm_year = 113;
		t.tm_mon = 8;
		t.tm_mday = 10;
		t.tm_isdst = -1;

		auto timet = mktime(&t);
		gcTime e(std::chrono::system_clock::from_time_t(timet));

		auto b = e.to_iso_string();
		auto strTestTimeStamp(gcTime::to_iso_string(timet));

		gcString strTimeStamp("20130910080656");
		auto p = MCFCore::Thread::parseTimeStamp(strTimeStamp);

		ASSERT_EQ(strTestTimeStamp, strTimeStamp);
		ASSERT_EQ(e, p);
	}
}

#endif
