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
#include "WGTController.h"
#include "WGTWorker.h"

#include "mcf/MCFFile.h"
#include "mcf/MCF.h"

#include "ProviderManager.h"

#define MAX_BLOCK_SIZE (50*1024*1024)

#ifndef min
template <typename A>
A min(A a, A b)
{
	return a<b?a:b;
}

#endif


namespace
{
	struct block_sortkey
	{
		bool operator()(MCFCore::Thread::Misc::WGTBlock *lhs, MCFCore::Thread::Misc::WGTBlock *rhs)
		{
			return (lhs->webOffset < rhs->webOffset);
		}
	};
}

namespace MCFCore
{
	namespace Thread
	{

		class WGTWorkerInfo
		{
		public:
			WGTWorkerInfo(WGTControllerI* con, uint32 i, MCFCore::Misc::ProviderManager &provMng)
				: id(i)
				, m_pWorkThread(new WGTWorker(con, i, provMng))
			{
				m_pWorkThread->setPriority(::Thread::BaseThread::BELOW_NORMAL);
			}

			~WGTWorkerInfo()
			{
				safe_delete(m_pWorkThread);
				safe_delete(vBuffer);
			}

			const uint32 id;
			uint64 ammountDone = 0;
					
			void setCurrentBlock(Misc::WGTSuperBlock* pBlock)
			{
				std::lock_guard<std::mutex> guard(mutex);

				curBlock = pBlock;
				status = MCFThreadStatus::SF_STATUS_CONTINUE;

				gcTrace("");
			}

			Misc::WGTSuperBlock* resetCurrentBlock()
			{
				std::lock_guard<std::mutex> guard(mutex);

				gcTrace("");

				auto block = curBlock;
				curBlock = nullptr;
				status = MCFThreadStatus::SF_STATUS_CONTINUE;

				return block;
			}

			void addBlock(Misc::WGTBlock* pBlock)
			{
				std::lock_guard<std::mutex> guard(mutex);
				vBuffer.push_back(pBlock);
			}

			Misc::WGTBlock* popBlock()
			{
				std::lock_guard<std::mutex> guard(mutex);

				if (vBuffer.size() == 0)
					return nullptr;

				Misc::WGTBlock *block = vBuffer.front();
				vBuffer.pop_front();

				return block;
			}

			size_t getBufferSize()
			{
				std::lock_guard<std::mutex> guard(mutex);
				return vBuffer.size();
			}

			size_t getCurrentBlockSize()
			{
				std::lock_guard<std::mutex> al(mutex);

				if (!curBlock)
					return 0;

				return curBlock->vBlockList.size();
			}

			Misc::WGTSuperBlock* getCurrentBlock()
			{
				std::lock_guard<std::mutex> al(mutex);

				gcTrace("");

				return curBlock;
			}

			Misc::WGTSuperBlock* splitCurrentBlock(size_t halfWay)
			{
				std::lock_guard<std::mutex> al(mutex);

				gcTrace("");

				if (!curBlock)
					return nullptr;

				if (halfWay > curBlock->vBlockList.size())
					return nullptr;

				Misc::WGTSuperBlock* superBlock = new Misc::WGTSuperBlock();

				std::lock_guard<std::mutex> cg(curBlock->m_Lock);

				auto firstHalf = std::deque<Misc::WGTBlock*>(curBlock->vBlockList.begin(), curBlock->vBlockList.begin() + halfWay);
				auto secondHalf = std::deque<Misc::WGTBlock*>(curBlock->vBlockList.begin() + halfWay, curBlock->vBlockList.end());

				size_t totSize = 0;

				for (auto b : secondHalf)
					totSize += b->size;

				curBlock->size -= totSize;
				curBlock->vBlockList = firstHalf;

				superBlock->size = totSize;
				superBlock->vBlockList = secondHalf;
				superBlock->offset = curBlock->offset + curBlock->size;

				return superBlock;
			}

			MCFThreadStatus getStatus()
			{
				std::lock_guard<std::mutex> al(mutex);
				return status;
			}

			void setStatus(MCFThreadStatus newStatus)
			{
				std::lock_guard<std::mutex> al(mutex);
				status = newStatus;
			}

			void stop()
			{
				m_pWorkThread->stop();
			}

			void start()
			{
				m_pWorkThread->start();
			}

			void reportError(gcException &e, gcString &strProv)
			{
				m_pWorkThread->reportError(e, strProv);
			}

			std::string toTracerString()
			{
				return gcString("CurBlock: {0}, Status: {1}", (uint64)curBlock, (uint32)status);
			}

		private:
			WGTWorker* m_pWorkThread;

			MCFThreadStatus status = MCFThreadStatus::SF_STATUS_CONTINUE;
			Misc::WGTSuperBlock* curBlock = nullptr;

			std::mutex mutex;
			std::deque<Misc::WGTBlock*> vBuffer;
		};

		class WGTWorkerList
		{
		public:
			WGTWorkerList()
			{

			}

			~WGTWorkerList()
			{
				safe_delete(m_vWorkerList);
			}

			void createWorkers(WGTControllerI *pController, uint32 nCount, MCFCore::Misc::ProviderManager &pProvManager)
			{
				for (uint16 x = 0; x<nCount; x++)
					m_vWorkerList.push_back(new WGTWorkerInfo(pController, x, pProvManager));
			}

			operator const std::vector<WGTWorkerInfo*>& ()
			{
				return m_vWorkerList;
			}

		private:
			std::vector<WGTWorkerInfo*> m_vWorkerList;
		};
	}
}

template <>
std::string TraceClassInfo<MCFCore::Thread::WGTWorkerInfo>(MCFCore::Thread::WGTWorkerInfo *pClass)
{
	return pClass->toTracerString();
}


using namespace MCFCore::Thread;




WGTController::WGTController(std::shared_ptr<MCFCore::Misc::DownloadProvidersI> pDownloadProviders, uint16 numWorkers, MCFCore::MCF* caller, bool checkMcf) 
	: MCFCore::Thread::BaseMCFThread(numWorkers, caller, "WebGet Controller Thread")
	, m_ProvManager(pDownloadProviders)
	, m_bCheckMcf(checkMcf)
	, m_pWorkerList(std::make_unique<WGTWorkerList>())
	, m_vWorkerList(*m_pWorkerList.get())
{
	if (m_uiNumber > pDownloadProviders->size())
		m_uiNumber = pDownloadProviders->size();

	m_ProvManager.onProviderEvent += delegate(&onProviderEvent);
	setPriority(BELOW_NORMAL);
}

WGTController::~WGTController()
{
	stop();

	if (m_bDoingStop)
		gcSleep(500);

	m_pWorkerList.reset();

	m_pFileMutex.lock();
	safe_delete(m_vSuperBlockList);
	m_pFileMutex.unlock();
}

void WGTController::pokeThread()
{
	m_WaitCondition.notify();
}


void WGTController::run()
{
	UTIL::FS::FileHandle fh;

	if (!fillBlockList())
		return;

	try
	{
		//header should be saved all ready so append to it
		fh.open(m_szFile, UTIL::FS::FILE_APPEND);
	}
	catch (gcException &except)
	{
		onErrorEvent(except);
		return;
	}

	m_pUPThread->start();
	createWorkers();

	//notify that init is done. :P
	MCFCore::Misc::ProgressInfo pi;
	pi.flag = MCFCore::Misc::ProgressInfo::FLAG_INITFINISHED;
	pi.percent = 0;
	onProgressEvent(pi);

	while (!isStopped())
	{
		doPause();
		saveBuffers(fh);

		if (isQueuedBlocks())
			continue;

		if (m_iRunningWorkers == 0)
			break;

		if (!m_ProvManager.hasValidAgents())
		{
			saveBuffers(fh, true);
			break;
		}

		if (!isStopped())
			m_WaitCondition.wait(5);
	}

	m_pUPThread->stop();

	for (auto worker : m_vWorkerList)
		worker->stop();

	if (m_iAvailbleWork == 0)
	{
		//notify that download is done. :P
		MCFCore::Misc::ProgressInfo pi;
		pi.flag = MCFCore::Misc::ProgressInfo::FLAG_FINALIZING;
		pi.percent = 100;
		onProgressEvent(pi);

		for (size_t x=0; x<m_vDlFiles.size(); x++)
		{
			m_rvFileList[m_vDlFiles[x]]->addFlag(MCFCore::MCFFileI::FLAG_COMPLETE);
			m_rvFileList[m_vDlFiles[x]]->delFlag(MCFCore::MCFFileI::FLAG_STARTEDDL);
		}
	}
	else
	{
		if (!isStopped())
		{
			gcException newE(ERR_SOCKET, "All server connections failed. \n\n"
								"Please check you are connected to the internet\n"
								"and try again or check console for more info.");
			onErrorEvent(newE);
		}
	}
}

void WGTController::createWorkers()
{
	m_pWorkerList->createWorkers(this, m_uiNumber, m_ProvManager);

	for (auto w : m_vWorkerList)
	{
		w->start();
		++m_iRunningWorkers;
	}
}

bool WGTController::isQueuedBlocks()
{
	for (auto worker : m_vWorkerList)
	{
		if (worker->getBufferSize() > 0)
			return true;
	}

	return false;
}

void WGTController::saveBuffers(UTIL::FS::FileHandle& fileHandle, bool allBlocks)
{
	//dont eat all the cpu speed
	if (m_vWorkerList.size() == 0)
		return;

	for (auto worker : m_vWorkerList)
	{
		do
		{
			auto block = worker->popBlock();

			if (!block)
				break;

			if (!checkBlock(block, worker->id))
				continue;

			try
			{
				fileHandle.seek(block->fileOffset);
				fileHandle.write(block->buff, block->size);

#ifdef DEBUG
				m_uiSaved += block->size;
#endif
			}
			catch (gcException &e)
			{
				onErrorEvent(e);
				safe_delete(block);
				break;
			}

			safe_delete(block);
		}
		while (allBlocks); //if all blocks is true it will keep looping until buff size is zero else it will run once
	}
}

bool WGTController::checkBlock(Misc::WGTBlock *block, uint32 workerId)
{
	if (!block)
		return true;

	bool crcFail = false;
	bool sizeFail = block->dlsize != block->size;

	uint32 crc = 0;

	if (!sizeFail)
	{
		crc = UTIL::MISC::CRC32((unsigned char*)block->buff, block->size);
		crcFail = (block->crc != 0 && block->crc != crc);
	}

	if (!sizeFail && !crcFail)
		return true;

	gcTrace("Id: {0}", workerId);

	reportNegProgress(workerId, block->dlsize);

	gcString appData = UTIL::OS::getAppDataPath(L"crc");

	UTIL::FS::Path logFile(appData, "crc_log.txt", false);
	UTIL::FS::Path outFile(appData, gcString("block_{0}_{1}.bin", block->fileOffset, time(nullptr)), false);

	outFile += m_pHeader->getDesuraId().toString();

	gcString log("Crc of block didnt match expected.\nFile: {0}\nWebOffset: {1}\nFileOffset: {2}\nExpected Crc: {3}\nCrc: {4}\nOut File: {5}\n", block->file->getName(), block->webOffset, block->fileOffset, block->crc, crc, outFile.getFullPath());
	log += gcString("Size: {3}\nMcf: {0}\nBuild: {1}\nBranch: {2}\nProvider: {4}\n\n", m_pHeader->getId(), m_pHeader->getBuild(), m_pHeader->getBranch(), block->size, block->provider);

	UTIL::FS::recMakeFolder(logFile);
	UTIL::FS::recMakeFolder(outFile);

	try
	{
		UTIL::FS::FileHandle fh(outFile, UTIL::FS::FILE_WRITE);
		fh.write(block->buff, block->size);
		fh.close();

		fh.open(logFile, UTIL::FS::FILE_APPEND);
		fh.write(log.c_str(), log.size());
		fh.close();

		Warning("Failed crc check. Writen to log file [{0}], block saved to file [{1}].\n", logFile.getFullPath(), outFile.getFullPath());
	}
	catch (gcException)
	{
	}

	Misc::WGTSuperBlock* super = new Misc::WGTSuperBlock();

	super->vBlockList.push_back(block);
	super->offset = block->webOffset;
	super->size = block->size;

	m_pFileMutex.lock();
	m_vSuperBlockList.push_back(super);
	m_iAvailbleWork++;
	m_pFileMutex.unlock();

	//get thread running again.
	m_WaitCondition.notify();

	gcException e(ERR_INVALIDDATA);

	if (sizeFail)
		e = gcException(ERR_INVALIDDATA, "Size of the download chunk didnt match what was expected");
	else if (crcFail)
		e = gcException(ERR_INVALIDDATA, "Crc of the download chunk didnt match what was expected");

	auto w = findWorker(workerId);

	if (w)
		w->reportError(e, block->provider);

	return false;
}

static bool WGTBlockSort(Misc::WGTBlock* a, Misc::WGTBlock* b)
{
	return a->webOffset < b->webOffset;
}

static bool SortByOffset(const std::shared_ptr<MCFCore::MCFFile>& a, const std::shared_ptr<MCFCore::MCFFile>& b)
{
	return a->getOffSet() < b->getOffSet();
}

bool WGTController::fillBlockList()
{
	MCFCore::Misc::ProgressInfo pi;
	MCFCore::MCF webMcf(m_ProvManager.getDownloadProviders());

	try
	{
		{
			std::lock_guard<std::mutex> guard(m_McfLock);
			m_pCurMcf = &webMcf;
		}

		webMcf.dlHeaderFromWeb();

		{
			std::lock_guard<std::mutex> guard(m_McfLock);
			m_pCurMcf = nullptr;
		}

	}
	catch (gcException &e)
	{
		onErrorEvent(e);
		return false;
	}

	pi.percent = 5;
	onProgressEvent(pi);

	webMcf.sortFileList();

	uint64 mcfOffset = m_pHeader->getSize();
	uint64 downloadSize = 0;
	uint64 done = 0;

	size_t fsSize = m_rvFileList.size();

	std::vector<std::shared_ptr<MCFFile>> tempFileList;
	tempFileList.reserve(fsSize);

	//find the last files offset
	for (size_t x=0; x< fsSize; x++)
	{
		auto file = m_rvFileList[x];

		if (!file->isSaved())
			continue;

		tempFileList.push_back(file);

		if ((file->isComplete() || file->hasStartedDL()) && file->getOffSet() > mcfOffset)
		{
			mcfOffset = file->getOffSet() + file->getCurSize();
		}
	}

	fsSize = tempFileList.size();

	UTIL::FS::FileHandle fh;
	try
	{
		fh.open(m_szFile, UTIL::FS::FILE_READ);
	}
	catch (gcException &)
	{
	}

	std::deque<Misc::WGTBlock*> vBlockList;
	std::sort(tempFileList.begin(), tempFileList.end(), SortByOffset);

	for (size_t x=0; x<tempFileList.size(); ++x)
	{
		auto file = tempFileList[x];

		if (isStopped())
		{
			safe_delete(vBlockList);
			return false;
		}

		uint32 p = 5 + x*95/fsSize;

		//dont go crazy on sending progress events
		if (pi.percent != p)
		{
			pi.percent = p;
			onProgressEvent(pi);
		}

		//dont download all ready downloaded items
		if (file->isComplete())
		{
			done += file->getCurSize();
			continue;
		}

		//skip files that arnt "saved" in the MCF
		if (!file->isSaved())
			continue;

		if (file->isZeroSize())
		{
			file->setOffSet(0);
			continue;
		}

		uint64 size = file->getCurSize();
		bool started = file->hasStartedDL();

		uint32 index = webMcf.findFileIndexByHash(file->getHash());
		auto webFile = webMcf.getFile(index);

		if (index == UNKNOWN_ITEM || !webFile || !webFile->isSaved())
		{
			Warning("File {0} is not in web MCF. Skipping download.\n", file->getName());
			if (!started)
				file->delFlag(MCFCore::MCFFileI::FLAG_SAVE);
			continue;
		}

		file->copyBorkedSettings(webFile);

		m_vDlFiles.push_back(x);
		file->addFlag(MCFCore::MCFFileI::FLAG_STARTEDDL);

		if (!started && file->getOffSet() != 0)
		{
			bool check1 = false;
			bool check2 = false;

			if (fsSize > 1 && x != (fsSize-1))
				check1 = (file->getOffSet() + file->getCurSize()) > tempFileList[x+1]->getOffSet();

			if (fsSize > 1 && x != 0 )
				check2 = file->getOffSet() < (tempFileList[x-1]->getOffSet() + tempFileList[x-1]->getCurSize());

			if (!check1 && !check2)
				started = true;
		}

		if (!started)
		{
			file->setOffSet(mcfOffset);
			mcfOffset += size;
		}

		uint32 blocksize = file->getBlockSize();
		uint32 offset = 0;
		uint32 y = 0;

		while (offset < size)
		{
			Misc::WGTBlock* temp = new Misc::WGTBlock;

			temp->file =  file;
			temp->index = y;

			temp->webOffset = webFile->getOffSet() + offset;
			temp->fileOffset = file->getOffSet() + offset;

			if (webFile->getCRCCount() > y)
				temp->crc = webFile->getCRC(y);

			//make sure we dont read past end of the file
			if (size-offset < blocksize)
				temp->size = (uint32)(size-offset);
			else
				temp->size = blocksize;

			if (started && m_bCheckMcf && fh.isValidFile() && file->crcCheck(y, fh))
			{
				done += temp->size;
				safe_delete(temp);
			}
			else
			{
				vBlockList.push_back(temp);
				downloadSize += temp->size;
			}

			offset+=blocksize;
			y++;
		}
	}

	m_pUPThread->setDone(done);
	m_pUPThread->setTotal(downloadSize+done);

	std::sort(vBlockList.begin(), vBlockList.end(), &WGTBlockSort);
	uint64 maxSize = min((uint32)(downloadSize / m_uiNumber), (uint32)(256*1024*1024));

	while (vBlockList.size() > 0)
	{
		Misc::WGTSuperBlock* sb = new Misc::WGTSuperBlock();
		sb->offset = vBlockList[0]->webOffset;

		bool size = false;
		bool offset = false;
		bool maxsize = false;

		do
		{
			Misc::WGTBlock* block = vBlockList.front();

			uint64 t = (uint64)block->size + (uint64)sb->size;

			//if we go over the 32 bit size limit break
			if (t >> 32)
				break;

			vBlockList.pop_front();

			sb->size += block->size;
			sb->vBlockList.push_back(block);

			size = (vBlockList.size() > 0);
			offset = size && (vBlockList[0]->webOffset == (sb->offset + sb->size));
			maxsize = (sb->size < maxSize);
		}
		while (size && offset && maxsize);

		m_vSuperBlockList.push_back(sb);
	}

	m_iAvailbleWork = m_vSuperBlockList.size();

	//make sure we hit 100
	pi.percent = 100;
	onProgressEvent(pi);
	return true;
}

Misc::WGTSuperBlock* WGTController::stealBlocks()
{
	gcTrace("");

	WGTWorkerInfo* largestWorker = nullptr;
	size_t largestCount = 0;

	for (auto w : m_vWorkerList)
	{
		size_t count = w->getCurrentBlockSize();

		if (count > largestCount)
		{
			largestCount = count;
			largestWorker = w;
		}
	}

	if (largestCount < 3 || !largestWorker)
		return nullptr;

	size_t halfWay = largestCount / 2;
	return largestWorker->splitCurrentBlock(halfWay);
}

bool WGTController::newTask(uint32 id, MCFThreadStatus &status, Misc::WGTSuperBlock* &pSuperBlock)
{
	gcAssert(!pSuperBlock);

	gcTrace("Id: {0}", id);

	WGTWorkerInfo* worker = findWorker(id);
	gcAssert(worker);

	status = worker->getStatus();

	if (status != MCFThreadStatus::SF_STATUS_CONTINUE)
		return false;

	pSuperBlock = worker->getCurrentBlock();

	if (pSuperBlock)
		return true;

	auto bNoMoreWork = false;
	
	{
		std::lock_guard<std::mutex> guard(m_pFileMutex);
		size_t listSize = m_vSuperBlockList.size();

		if (listSize == 0)
		{
			auto pStolenBlock = stealBlocks();

			if (!pStolenBlock)
			{
				bNoMoreWork = true;
			}
			else
			{
				pSuperBlock = pStolenBlock;
				m_iAvailbleWork++;
			}
		}
		else
		{
			pSuperBlock = m_vSuperBlockList.front();
			m_vSuperBlockList.pop_front();
		}
	}

	if (bNoMoreWork)
	{
		m_pUPThread->stopThread(id);
		worker->setStatus(MCFThreadStatus::SF_STATUS_STOP);
		status = MCFThreadStatus::SF_STATUS_STOP;

		m_iRunningWorkers--;
		//get thread running again.
		m_WaitCondition.notify();

		return false;
	}

	if (!pSuperBlock)
		return newTask(id, status, pSuperBlock);

	worker->setCurrentBlock(pSuperBlock);
	status = MCFThreadStatus::SF_STATUS_CONTINUE;

	return true;
}

void WGTController::workerFinishedSuperBlock(uint32 id, Misc::WGTSuperBlock* &pSuperBlock)
{
	gcTrace("Id: {0}", id);

	WGTWorkerInfo* worker = findWorker(id);
	gcAssert(worker);

	Misc::WGTSuperBlock* block = worker->resetCurrentBlock();

	gcAssert(block == pSuperBlock);
	pSuperBlock = nullptr;

	if (!block)
	{
		m_WaitCondition.notify();
		return;
	}

	if (block->vBlockList.size() == 0)
	{
		if (block->size != 0)
			Warning("WGControler: Block still has data to download but all blocks consumed. Expect trouble ahead\n");

		safe_delete(block);

		std::lock_guard<std::mutex> guard(m_pFileMutex);
		m_iAvailbleWork--;
	}
	else
	{
		std::lock_guard<std::mutex> guard(m_pFileMutex);
		m_vSuperBlockList.push_back(block);
	}

	//get thread running again.
	m_WaitCondition.notify();
}

void WGTController::workerFinishedBlock(uint32 id, Misc::WGTBlock* block)
{
	WGTWorkerInfo* worker = findWorker(id);
	gcAssert(worker);

	if (!block || !worker)
		return;

	worker->addBlock(block);

	//get thread running again.
	m_WaitCondition.notify();
}

MCFThreadStatus WGTController::getStatus(uint32 id)
{
	WGTWorkerInfo* worker = findWorker(id);
	gcAssert(worker);

	if (isPaused())
		return MCFThreadStatus::SF_STATUS_PAUSE;

	if (isStopped())
		return MCFThreadStatus::SF_STATUS_STOP;

	return worker->getStatus();
}

void WGTController::reportError(uint32 id, gcException &e, Misc::WGTSuperBlock* &pSuperBlock)
{
	gcTrace("Id: {0}, E: {1}", id, e);

	workerFinishedSuperBlock(id, pSuperBlock);

	WGTWorkerInfo* worker = findWorker(id);
	gcAssert(worker);

	Warning("WebGet: {0} Error: {1}.\n", id, e);

	m_pUPThread->stopThread(id);
	worker->setStatus(MCFThreadStatus::SF_STATUS_STOP);

	m_iRunningWorkers--;

	//get thread running again.
	m_WaitCondition.notify();
}

void WGTController::reportProgress(uint32 id, uint64 ammount)
{
	WGTWorkerInfo* worker = findWorker(id);
	gcAssert(worker);
	gcAssert(m_pUPThread);

	worker->ammountDone += ammount;
	m_pUPThread->reportProg(id, worker->ammountDone);
}

void WGTController::reportNegProgress(uint32 id, uint64 ammount)
{
	WGTWorkerInfo* worker = findWorker(id);
	gcAssert(worker);
	gcAssert(m_pUPThread);

	worker->ammountDone -= ammount;
	m_pUPThread->reportProg(id, worker->ammountDone);
}

WGTWorkerInfo* WGTController::findWorker(uint32 id)
{
	auto it = std::find_if(begin(m_vWorkerList), end(m_vWorkerList), [id](WGTWorkerInfo* w){
		return w->id == id;
	});

	if (it != end(m_vWorkerList))
		return *it;

	return nullptr;
}

void WGTController::onStop()
{
	m_bDoingStop = true;

	{
		std::lock_guard<std::mutex> guard(m_McfLock);
		if (m_pCurMcf)
			m_pCurMcf->stop();
	}

	BaseMCFThread::onStop();

	for (auto w : m_vWorkerList)
		w->stop();

	//get thread running again.
	m_WaitCondition.notify();

	m_bDoingStop = false;
}
