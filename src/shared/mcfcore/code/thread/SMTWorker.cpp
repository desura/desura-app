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
#include "SMTWorker.h"

#include "util_thread/BaseThread.h"
#include "mcf/MCFFile.h"
#include "mcf/MCFHeader.h"
#include "SMTController.h"

#include "util/MD5Progressive.h"
#include "ProgressiveCRC.h"
#include "Courgette.h"

namespace MCFCore
{
namespace Thread
{

SMTWorker::SMTWorker(SMTController* controller, uint32 id, UTIL::FS::FileHandle* fileHandle) : BaseThread( "SaveMCF Thread" )
{
	m_pCT = controller;
	m_uiId = id;

	m_phFhSink = fileHandle;

	if (id == 0)
		m_uiCurOffset = MCFCore::MCFHeader::getSizeS();
	else
		m_uiCurOffset = 0;

	m_uiDiffCurOffset = 0;
	m_uiCompressSize = 0;
	m_uiTotRead = 0;

	m_pCRC = nullptr;
	m_pMD5Norm = nullptr;
	m_pMD5Comp = nullptr;

	m_BZ2Worker = nullptr;
}

SMTWorker::~SMTWorker()
{
	safe_delete(m_phFhSink);
	safe_delete(m_BZ2Worker);
}


void SMTWorker::run()
{
	gcAssert(m_pCT);

	while (!isStopped())
	{
		MCFThreadStatus status = m_pCT->getStatus(m_uiId);

		while (status ==  MCFThreadStatus::SF_STATUS_PAUSE)
		{
			gcSleep(500);
			status = m_pCT->getStatus(m_uiId);
		}

		if (status ==  MCFThreadStatus::SF_STATUS_STOP)
			break;

		if (status ==  MCFThreadStatus::SF_STATUS_NULL)
		{
			if (!newTask())
				continue;
		}

		//make sure we dont do compression if we are ment to be stopped
		if (!isStopped() && status ==  MCFThreadStatus::SF_STATUS_CONTINUE)
		{
			try
			{
				doWork();
			}
			catch (gcException &e)
			{
				gcString name("Null File");

				if (m_pCurFile)
					name = m_pCurFile->getName();

				gcException e2((ERROR_ID)e.getErrId(), e.getSecErrId(), gcString("{0} [{1}]", e.getErrMsg(), name));

				finishTask();
				m_pCT->reportError(m_uiId, e2);
				return;
			}
		}
	}

	m_phFhSink->close();
}

void SMTWorker::doWork()
{
	gcAssert(m_pCurFile);
	gcAssert(m_phFhSink);

	uint32 buffSize = BLOCKSIZE;
	bool endFile = false;

	if ((m_pCurFile->getSize() - m_uiTotRead) < buffSize)
	{
		buffSize = (uint32)(((uint64)m_pCurFile->getSize()) - m_uiTotRead);
		endFile = true;
	}

	if (buffSize == 0)
	{
		if (m_pCurFile->isCompressed())
		{
			//need to make sure we finish the compression off
			doCompression(nullptr, 0, true);
		}
		else
		{
			finishTask();
			m_pCT->endTask(m_uiId);
		}
	}
	else
	{
		UTIL::MISC::Buffer buff(buffSize);
		m_hFhSource.read(buff, buffSize);

		m_uiTotRead += buffSize;
		m_uiTotFileRead += buffSize;
		m_pMD5Norm->update(buff, buffSize);

		if (!m_pCurFile->isCompressed())
			writeFile(buff, buffSize, endFile);
		else
			doCompression(buff, buffSize, endFile);
	}
}

void SMTWorker::doCompression(const char* buff, uint32 buffSize, bool endFile)
{
	auto updateCallback = [this](const unsigned char* tbuff, uint32 tsize) -> bool
	{
		m_uiCompressSize += tsize;

		if (m_pMD5Comp)
			m_pMD5Comp->update((const char*)tbuff, tsize);

		writeFile((const char*)tbuff, tsize, false);
		return true;
	};

	m_BZ2Worker->write(buff, buffSize, updateCallback);

	if (endFile)
	{
		m_BZ2Worker->end(updateCallback);
		writeFile(nullptr, 0, true);
	}
}

void SMTWorker::writeFile(const char* buff, uint32 buffSize, bool endFile)
{
	if (buff && buffSize > 0)
	{
		m_phFhSink->write(buff, buffSize);

		m_uiCurOffset += buffSize;
		m_pCT->reportProgress(m_uiId, m_uiTotFileRead);
		m_pCRC->addData((unsigned char*)buff, buffSize);
	}

	if (endFile)
	{
		if (m_pCurFile->isCompressed())
			m_pCurFile->setCSize(m_uiCompressSize);

		finishTask();
		m_pCT->endTask(m_uiId);
	}
}


void SMTWorker::finishTask()
{
	m_hFhSource.close();
	
	if (m_pCRC)
		m_pCurFile->setCRC(m_pCRC->getVector());

	if (m_pMD5Norm)
		m_pCurFile->setCsum(m_pMD5Norm->finish().c_str());

	if (m_pCurFile->isCompressed() && m_pMD5Comp)
		m_pCurFile->setCCsum(m_pMD5Comp->finish().c_str());

	safe_delete(m_pMD5Norm);
	safe_delete(m_pCRC);
	safe_delete(m_BZ2Worker);
	safe_delete(m_pMD5Comp);
}

bool SMTWorker::newTask()
{
	m_pCurFile = m_pCT->newTask(m_uiId);

	if (!m_pCurFile)
		return false;

	m_uiTotRead = 0;
	m_uiCompressSize = 0;

	std::string path = m_pCurFile->getFullPath();

	try
	{
		m_hFhSource.open(path.c_str(), UTIL::FS::FILE_READ);
	}
	catch (gcException &)
	{
	}

	//zero file
	if (!m_hFhSource.isValidFile())
	{
		m_pCT->endTask(m_uiId);
		return true;
	}

	m_pCurFile->setOffSet(m_uiCurOffset);

	m_pMD5Norm = new MD5Progressive();
	m_pCRC = new MCFCore::Misc::ProgressiveCRC(m_pCurFile->getBlockSize());
	
	if (m_pCurFile->isCompressed())
	{
		m_pMD5Comp = new MD5Progressive();
		m_BZ2Worker = new UTIL::MISC::BZ2Worker(UTIL::MISC::BZ2_COMPRESS);
	}

	m_uiTotFileRead = 0;
	return true;
}




}
}
