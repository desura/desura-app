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
#include "SMTController.h"
#include "SMTWorker.h"

#include "mcf/MCF.h"
#include "mcf/MCFFile.h"
#include "mcf/MCFHeader.h"

using namespace MCFCore::Thread;

namespace MCFCore
{
	namespace Thread
	{
		//! Save MCF Worker container class
		class SMTWorkerInfo
		{
		public:
			SMTWorkerInfo(uint32 i)
				: id(i)
			{
			}

			void init(SMTController* con, UTIL::FS::FileHandle* fileHandle, gcString f)
			{
				workThread = std::make_unique<SMTWorker>(con, id, fileHandle);
				workThread->setPriority(::Thread::BaseThread::BELOW_NORMAL);
				file = f;
			}

			uint64 ammountDone = 0;

			const uint32 id = 0;
			MCFThreadStatus status = MCFThreadStatus::SF_STATUS_NULL;

			gcString file;

			std::shared_ptr<MCFCore::MCFFile> curFile;
			std::unique_ptr<SMTWorker> workThread;
			std::vector<uint32> vFileList;
		};
	}
}

namespace
{
	class tFile
	{
	public:
		tFile(uint64 s, uint32 p)
		{
			size = s;
			pos = p;
		}

		uint64 size;
		uint32 pos;
	};

	struct file_sortkey
	{
		bool operator()(tFile *lhs, tFile *rhs)
		{
			return (lhs->size < rhs->size);
		}
	};
}


SMTController::SMTController(uint16 num, MCFCore::MCF* caller)
	: MCFCore::Thread::BaseMCFThread(std::min<uint32>(num, 4), caller, "SaveMCF Thread")
	, m_vWorkerList(createWorkers())
{
}

SMTController::~SMTController()
{
	safe_delete(m_vWorkerList);
}

void SMTController::onPause()
{
	//get thread running again.
	m_WaitCond.notify();
	BaseMCFThread::onPause();
}

void SMTController::onStop()
{
	//get thread running again.
	m_WaitCond.notify();
	BaseMCFThread::onStop();
}

void SMTController::run()
{
	gcAssert(m_uiNumber);
	gcAssert(m_szFile);

	fillFileList();
	m_pUPThread->start();

	if (!initWorkers())
		return;

	for (auto worker : m_vWorkerList)
		worker->workThread->start();

	while (true)
	{
		doPause();

		if (isStopped())
			break;

		//wait here as we have nothing else to do
		m_WaitCond.wait(2);

		if (m_iRunningWorkers == 0)
			break;
	}

	m_pUPThread->stop();

	for (auto worker : m_vWorkerList)
		worker->workThread->stop();

	if (!isStopped())
		postProcessing();
}

std::vector<SMTWorkerInfo*> SMTController::createWorkers()
{
	std::vector<SMTWorkerInfo*> vWorkerList;

	for (uint32 x = 0; x < m_uiNumber; x++)
		vWorkerList.push_back(new SMTWorkerInfo(x));

	return vWorkerList;
}

bool SMTController::initWorkers()
{
	for (uint32 x = 0; x<m_uiNumber; x++)
	{
		gcString file("{0}", m_szFile);

		UTIL::FS::FileHandle* fh = new UTIL::FS::FileHandle();

		if (x != 0)
			file += gcString(".part_{0}", x);

		try
		{
			fh->open(file.c_str(), UTIL::FS::FILE_WRITE);

			//due to the first thread being the proper MCF we have to allow for the header.
			if (x == 0)
				fh->seek(MCFCore::MCFHeader::getSizeS());
		}
		catch (gcException &except)
		{
			if (x == 0)
			{
				safe_delete(fh);
				onErrorEvent(except);
				return false;
			}
		}

		m_vWorkerList[x]->init(this, fh, file);
		m_iRunningWorkers++;
	}

	return true;
}

void SMTController::postProcessing()
{
	if (m_uiNumber == 1)
		return;

	gcTrace("");

	UTIL::FS::FileHandle fhSource;
	UTIL::FS::FileHandle fhSink;

	UTIL::FS::Path path(m_szFile, "", true);

	uint64 sinkSize = UTIL::FS::getFileSize(path);

	try
	{
		fhSink.open(path, UTIL::FS::FILE_APPEND);
	}
	catch (gcException &)
	{
		return;
	}

	char buff[BLOCKSIZE];

	auto bFirst = true;

	for (auto worker : m_vWorkerList)
	{
		if (bFirst)
		{
			bFirst = false;
			continue;
		}

		uint64 fileSize = UTIL::FS::getFileSize(UTIL::FS::PathWithFile(worker->file));
		uint64 done = 0;

		uint32 readSize = BLOCKSIZE;

		try
		{
			fhSource.open(worker->file.c_str(), UTIL::FS::FILE_READ);
			while (fileSize > done)
			{
				if ((fileSize - done) < (uint64)readSize)
					readSize = (uint32)(fileSize - done);

				fhSource.read(buff, readSize);
				fhSink.write(buff, readSize);

				done += readSize;
			}
			fhSource.close();
		}
		catch (gcException &)
		{
		}

		for (auto index : worker->vFileList)
		{
			auto temp = m_rvFileList[index];

			if (!temp)
				continue;

			temp->setOffSet(temp->getOffSet() + sinkSize);
		}

		sinkSize += fileSize;
		UTIL::FS::delFile(UTIL::FS::PathWithFile(worker->file));
	}

	if (m_bCreateDiff == false)
		return;
}

void SMTController::fillFileList()
{
	gcTrace("");

	uint64 sumSize = 0;

	std::vector<tFile*> vList;

	for (size_t x = 0; x<m_rvFileList.size(); x++)
	{
		if (!m_rvFileList[x]->isSaved())
			continue;

		if (m_rvFileList[x]->getSize() == 0)
		{
			m_rvFileList[x]->addFlag(MCFCore::MCFFileI::FLAG_ZEROSIZE);
			continue;
		}

		sumSize += m_rvFileList[x]->getSize();


		if (m_bCompress && m_rvFileList[x]->shouldCompress())
			m_rvFileList[x]->addFlag(MCFCore::MCFFileI::FLAG_COMPRESSED);
		else
			m_rvFileList[x]->delFlag(MCFCore::MCFFileI::FLAG_COMPRESSED);

		vList.push_back(new tFile(m_rvFileList[x]->getSize(), (uint32)x));
	}


	std::sort(vList.begin(), vList.end(), file_sortkey());

	for (size_t x = 0; x<vList.size(); x++)
	{
		m_vFileList.push_back(vList[x]->pos);
	}

	safe_delete(vList);

	m_pUPThread->setTotal(sumSize);
}

MCFThreadStatus SMTController::getStatus(uint32 id)
{
	SMTWorkerInfo* worker = findWorker(id);
	gcAssert(worker);

	if (isPaused())
		return MCFThreadStatus::SF_STATUS_PAUSE;

	if (isStopped())
		return MCFThreadStatus::SF_STATUS_STOP;

	return worker->status;
}

std::shared_ptr<MCFCore::MCFFile> SMTController::newTask(uint32 id)
{
	gcTrace("Id: {0}", id);

	SMTWorkerInfo* worker = findWorker(id);
	gcAssert(worker);

	if (worker->status != MCFThreadStatus::SF_STATUS_NULL)
		return nullptr;

	m_pFileMutex.lock();
	size_t listSize = m_vFileList.size();
	m_pFileMutex.unlock();

	if (listSize == 0)
	{
		m_pUPThread->stopThread(id);
		worker->status = MCFThreadStatus::SF_STATUS_STOP;

		m_iRunningWorkers--;
		//wake thread up
		m_WaitCond.notify();
		return nullptr;
	}

	m_pFileMutex.lock();
	int index = m_vFileList.back();
	m_vFileList.pop_back();
	m_pFileMutex.unlock();

	auto temp = m_rvFileList[index];

	if (!temp)
		return newTask(id);

	worker->vFileList.push_back(index);
	worker->curFile = temp;
	worker->status = MCFThreadStatus::SF_STATUS_CONTINUE;
	return temp;
}

void SMTController::endTask(uint32 id)
{
	gcTrace("Id: {0}", id);

	SMTWorkerInfo* worker = findWorker(id);
	gcAssert(worker);

	worker->status = MCFThreadStatus::SF_STATUS_NULL;

	worker->ammountDone += worker->curFile->getSize();
}

SMTWorkerInfo* SMTController::findWorker(uint32 id)
{
	for (auto worker : m_vWorkerList)
	{
		if (worker->id == id)
			return worker;
	}

	return nullptr;
}

void SMTController::reportError(uint32 id, gcException &e)
{
	gcTrace("Id: {0}, E: {1}", id, e);

#ifdef WIN32
	SMTWorkerInfo* worker = findWorker(id);
	gcAssert(worker);
#endif
	Warning("SMTControler {0} Error: {1}.\n", id, e);
	onErrorEvent(e);
}

void SMTController::reportProgress(uint32 id, uint64 ammount)
{
	SMTWorkerInfo* worker = findWorker(id);
	gcAssert(worker);
	gcAssert(m_pUPThread);

	m_pUPThread->reportProg(id, worker->ammountDone + ammount);
}
