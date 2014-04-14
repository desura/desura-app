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
#include "WGTWorker.h"
#include "WGTController.h"
#include "ProviderManager.h"


using namespace MCFCore::Thread;


WGTWorker::WGTWorker(WGTControllerI* controller, uint16 id, MCFCore::Misc::ProviderManager &provMng) 
    : BaseThread( "WebGet Worker Thread" )
	, m_ProvMng(provMng)
	, m_uiId(id)
	, m_pCT(controller)
{
}

WGTWorker::~WGTWorker()
{
	m_DeleteMutex.lock();
	safe_delete(m_pMcfCon);
	m_DeleteMutex.unlock();
}

void WGTWorker::reportError(gcException &e, gcString provider)
{
	if (m_ProvMng.getName(m_uiId) != provider)
		return;

	m_ErrorMutex.lock();
	m_bError = true;
	m_Error = e;
	m_ErrorMutex.unlock();
}

void WGTWorker::run()
{
	m_DownloadProvider = m_ProvMng.getUrl(m_uiId);

	if (!m_DownloadProvider || !m_DownloadProvider->isValidAndNotExpired())
	{
		Warning("Mcf Download Thread [{0}] failed to get valid url for download.\n", m_uiId);
		return;
	}

	gcString name = m_ProvMng.getName(m_uiId);

	m_pMcfCon = new MCFCore::Misc::MCFServerCon();
	m_pMcfCon->setDPInformation(name.c_str());
	m_pMcfCon->onProgressEvent += delegate(this, &WGTWorker::onProgress);

    while (!isThreadStopped())
	{
		MCFThreadStatus status = m_pCT->getStatus(m_uiId);

		uint32 pauseCount = 0;

		bool isPaused= (status == MCFThreadStatus::SF_STATUS_PAUSE);

		if (isPaused)
			m_pMcfCon->onPause();

		while (status == MCFThreadStatus::SF_STATUS_PAUSE)
		{
			if (pauseCount > 30000 && m_pMcfCon->isConnected())
				m_pMcfCon->disconnect();

			gcSleep(500);
			pauseCount += 500;

			status = m_pCT->getStatus(m_uiId);
		}

		if (isPaused)
		{
			gcString name = m_ProvMng.getName(m_uiId);
			m_pMcfCon->setDPInformation(name.c_str());
		}

		if (status == MCFThreadStatus::SF_STATUS_STOP)
			break;

		m_ErrorMutex.lock();
		if (m_bError)
		{
			requestNewUrl(m_Error);
			m_bError = false;
		}
		m_ErrorMutex.unlock();

		if (status == MCFThreadStatus::SF_STATUS_CONTINUE)
			doDownload();
	}

	m_ProvMng.removeAgent(m_uiId);

	m_DeleteMutex.lock();
	safe_delete(m_pMcfCon);
	m_DeleteMutex.unlock();

	//need to do this incase we are the last thread and the controller is stuck on the wait mutex
	m_pCT->pokeThread();
}

void WGTWorker::reset()
{
}

bool WGTWorker::isThreadStopped()
{
    return BaseThread::isStopped();
}

bool WGTWorker::writeData(char* data, uint32 size)
{
    if (isThreadStopped())
		return false;

	if (m_bError)
		return false;

	if (size == 0)
		return true;

	if (!m_pCurBlock)
		return false;

	MCFCore::Thread::Misc::WGTBlock* block = nullptr;

	m_pCurBlock->m_Lock.lock();

	if (m_pCurBlock->vBlockList.size() != 0)
		block = m_pCurBlock->vBlockList[0];

	m_pCurBlock->m_Lock.unlock();

	if (!block)
		return false;

    size_t done = m_pCurBlock->done;

	if (done == 0)
	{
		safe_delete(block->buff);
		block->buff =  new char[block->size];
	}

	size_t ds = block->size - done;

	if (size >= ds)
	{
		block->dlsize = block->size;
		block->provider = m_ProvMng.getName(m_uiId);

		memcpy(block->buff+done, data, ds);

#ifdef DEBUG
		//checkBlock(block);
#endif

		m_pCT->workerFinishedBlock(m_uiId, block);

		m_pCurBlock->m_Lock.lock();

			m_pCurBlock->vBlockList.pop_front();
			m_pCurBlock->done = 0;

			m_pCurBlock->offset += block->size;
			m_pCurBlock->size -= block->size;

		m_pCurBlock->m_Lock.unlock();
		
		writeData(data+ds, size-ds);
	}
	else
	{
		memcpy(block->buff+done, data, size);
		m_pCurBlock->done += size;
	}

	if (m_pCT->getStatus(m_uiId) == MCFThreadStatus::SF_STATUS_PAUSE)
		return false;

	return true;
}


bool WGTWorker::checkBlock(Misc::WGTBlock *block)
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

	return false;
}


void WGTWorker::doDownload()
{
	gcAssert(m_pMcfCon);

	if (!m_pCurBlock)
	{
		MCFThreadStatus status = MCFThreadStatus::SF_STATUS_NULL;
		m_pCurBlock = m_pCT->newTask(m_uiId, status);

		if (!m_pCurBlock)
		{
			if (status != MCFThreadStatus::SF_STATUS_STOP)
				Warning("The block was nullptr for Mcf Download thread {0}\n", m_uiId);

			return;
		}
	}

	try
	{
		if (!m_DownloadProvider || !m_DownloadProvider->isValidAndNotExpired())
			m_DownloadProvider = m_ProvMng.getUrl(m_uiId);

		if (!m_DownloadProvider || !m_DownloadProvider->isValidAndNotExpired())
			throw gcException(ERR_MCFSERVER, "No more download servers to use.");

		if (!m_pMcfCon->isConnected())
		{
			auto authToken = m_ProvMng.getDownloadAuth();
			m_pMcfCon->connect(*m_DownloadProvider, *authToken);
		}

		m_pMcfCon->downloadRange(m_pCurBlock->offset + m_pCurBlock->done, m_pCurBlock->size - m_pCurBlock->done, this);
	}
	catch (gcException &excep)
	{
		if (excep.getErrId() == ERR_MCFSERVER && excep.getSecErrId() == ERR_USERCANCELED)
		{
			//do nothing. Block errored out before or client paused.
		}
        else if (!isThreadStopped())
		{
			if (m_DownloadProvider)
				Warning("Mcf Server error: {0} [{1}]\n", excep, m_DownloadProvider->getUrl());
			else
				Warning("Mcf Server error: {0} [NULL PROVIDER]\n", excep);

			if (excep.getErrId() == ERR_LIBCURL)
			{
				//dont do any thing, just retry unless this is our third time
				if (isGoodSocketError(excep.getSecErrId()) && m_iAttempt < 3)
					m_iAttempt++;
				else
					requestNewUrl(excep);
			}
			else if (excep.getErrId() == ERR_MCFSERVER)
			{
				requestNewUrl(excep);
			}
			else
			{
				m_pCT->reportError(m_uiId, excep);
			}
		}

		takeProgressOff();
		return;
	}

	if (m_pCurBlock->size == 0)
	{
		m_pCT->workerFinishedSuperBlock(m_uiId);
		m_pCurBlock = nullptr;
	}
}


bool WGTWorker::isGoodSocketError(uint32 errId)
{
	switch (errId)
	{
	case 28: //CURL_TIMEOUT
	case 42: //CURLE_ABORTED_BY_CALLBACK:
	case 81: //CURLE_AGAIN:
		return true;
	};

	return false;
}

void WGTWorker::onStop()
{
	m_DeleteMutex.lock();

	if (m_pMcfCon)
		m_pMcfCon->disconnect();

	m_DeleteMutex.unlock();

	join();
}

void WGTWorker::takeProgressOff()
{
	m_pCT->reportNegProgress(m_uiId, m_pCurBlock->done);
	m_pCurBlock->done = 0;

	m_pCT->workerFinishedSuperBlock(m_uiId);
	m_pCurBlock = nullptr;
}

void WGTWorker::onProgress(uint32& prog)
{
	m_pCT->reportProgress(m_uiId, prog);
}

void WGTWorker::requestNewUrl(gcException& e)
{
	m_DownloadProvider = m_ProvMng.requestNewUrl(m_uiId, e.getSecErrId(), e.getErrMsg());

	if (m_DownloadProvider && m_DownloadProvider->isValidAndNotExpired())
	{
		m_iAttempt = 0;
		m_pMcfCon->disconnect();

		gcString name = m_ProvMng.getName(m_uiId);
		m_pMcfCon->setDPInformation(name.c_str());
	}
	else
	{
		gcException e(ERR_MCFSERVER, "No more download servers to use.");
		m_pCT->reportError(m_uiId, e);
		m_pCT->pokeThread();
	}
}


#ifdef WITH_GTEST

#include <gtest/gtest.h>

namespace UnitTest
{
	class StubMCFServerCon : public MCFCore::Misc::MCFServerConI
	{
	public:
		StubMCFServerCon(int nDownloadSize, int nMaxRequestSize)
			: m_nDownloadSize(nDownloadSize)
			, m_nMaxRequestSize(nMaxRequestSize)
		{
		}
		
		void connect(const MCFCore::Misc::DownloadProvider &provider, const MCFCore::Misc::GetFile_s& fileAuth) override
		{
		}

		void disconnect() override
		{
		}

		void setDPInformation(const char* name) override
		{
		}

		void downloadRange(uint64 offset, uint32 size, MCFCore::Misc::OutBufferI* buff) override
		{
			gcBuff temp(m_nDownloadSize);
			char* szBuff = temp.c_ptr();

			if (size > m_nMaxRequestSize)
				size = m_nMaxRequestSize;

			while (size > 0)
			{
				uint32 todo = m_nDownloadSize;

				if (todo > size)
					todo = size;

				for (uint32 x = 0; x < todo; ++x)
					szBuff[x] = (char)(offset + x);

				buff->writeData(szBuff, todo);

				size -= todo;
				offset += todo;
			}
		}

		void onPause() override
		{
		}

		bool isConnected() override
		{
			return true;
		}

		const uint32 m_nDownloadSize;
		const uint32 m_nMaxRequestSize;
	};

	class StubWGTController : public MCFCore::Thread::WGTControllerI
	{
	public:
		StubWGTController()
			: m_bSuperCompleted(false)
		{
		}

		MCFCore::Thread::Misc::WGTSuperBlock* newTask(uint32 id, MCFThreadStatus &status) override
		{
			return &m_SuperBlock;
		}

		MCFThreadStatus getStatus(uint32 id) override
		{
			return MCFThreadStatus::SF_STATUS_CONTINUE;
		}

		void reportError(uint32 id, gcException &e) override
		{
		}

		void reportProgress(uint32 id, uint64 ammount) override
		{
		}

		void reportNegProgress(uint32 id, uint64 ammount) override
		{
		}

		void workerFinishedBlock(uint32 id, MCFCore::Thread::Misc::WGTBlock* block) override
		{
			m_vCompletedBlocks.push_back(block);
		}

		void workerFinishedSuperBlock(uint32 id) override
		{
			m_bSuperCompleted = true;
		}

		void pokeThread() override
		{
		}

		bool m_bSuperCompleted;
		std::vector<MCFCore::Thread::Misc::WGTBlock*> m_vCompletedBlocks;
		MCFCore::Thread::Misc::WGTSuperBlock m_SuperBlock;
	};


	class TestWGTWorker : public MCFCore::Thread::WGTWorker
	{
	public:
		TestWGTWorker(MCFCore::Thread::WGTControllerI* controller, uint16 id, MCFCore::Misc::ProviderManager &provMng)
			: MCFCore::Thread::WGTWorker(controller, id, provMng)
		{
		}

		void download(StubMCFServerCon *pMcfCon, int nRunCount)
		{
			safe_delete(m_pMcfCon);
			m_pMcfCon = pMcfCon;

			for (int x = 0; x < nRunCount; x++)
				doDownload();
		}

        bool isThreadStopped() override
        {
            return false;
        }
	};

	class TestDownloadProviders : public MCFCore::Misc::DownloadProvidersI
	{
	public:
		TestDownloadProviders(std::vector<std::shared_ptr<const MCFCore::Misc::DownloadProvider>> &vProviders)
			: m_vProviders(vProviders)
		{
		}

		void setInfo(DesuraId id, MCFBranch branch, MCFBuild build) override
		{

		}

		bool getDownloadProviders(std::vector<std::shared_ptr<const MCFCore::Misc::DownloadProvider>> &vDownloadProviders) override
		{
			vDownloadProviders = m_vProviders;
			return true;
		}

		std::shared_ptr<const MCFCore::Misc::GetFile_s> getDownloadAuth()
		{
			return std::make_shared<const MCFCore::Misc::GetFile_s>();
		}

		size_t size()
		{
			return m_vProviders.size();
		}

		std::vector<std::shared_ptr<const MCFCore::Misc::DownloadProvider>> &m_vProviders;
	};

	class WGTWorkerFixture : public ::testing::TestWithParam<std::pair<int, int>>
	{
	public:
		WGTWorkerFixture()
			: m_nParamOne(GetParam().first)
			, m_nParamTwo(GetParam().second)
			, Provider(std::make_shared<MCFCore::Misc::DownloadProvider>("a1", "a2", "a3", "a4"))
			, DownloadProviders(std::make_shared<TestDownloadProviders>(getProviderVector()))
			, ProviderManager(DownloadProviders)
			, Worker(&Controller, 1, ProviderManager)
		{
		}

		void AddBlock(int nSize)
        {
			std::shared_ptr<MCFCore::Thread::Misc::WGTBlock> a(new MCFCore::Thread::Misc::WGTBlock());
			a->size = nSize;
			a->fileOffset = Controller.m_SuperBlock.size;
			a->webOffset = Controller.m_SuperBlock.size;

			unsigned char* szTemp = new unsigned char[nSize];

			for (int x = 0; x < nSize; x++)
				szTemp[x] = (unsigned char)(Controller.m_SuperBlock.size + x);

			a->crc = UTIL::MISC::CRC32(szTemp, nSize);
			a->index = m_vBlocks.size();

			safe_delete(szTemp);

			Controller.m_SuperBlock.size += nSize;
			Controller.m_SuperBlock.vBlockList.push_back(a.get());
			m_vBlocks.push_back(a);
		}

		void DoDownload(int nDownloadSize, int nMaxRequestSize=-1)
		{
			int nRunCount = (Controller.m_SuperBlock.size / nMaxRequestSize) + 1;

			Worker.download(new StubMCFServerCon(nDownloadSize, nMaxRequestSize), nRunCount);
		}

		void CheckCompletedBlocks()
		{
			ASSERT_TRUE(Controller.m_bSuperCompleted);
			ASSERT_EQ(m_vBlocks.size(), Controller.m_vCompletedBlocks.size());

			for (size_t x = 0; x < m_vBlocks.size(); x++)
			{
				unsigned long crc = UTIL::MISC::CRC32((unsigned char*)m_vBlocks[x]->buff, m_vBlocks[x]->size);
				ASSERT_EQ(m_vBlocks[x]->crc, crc);
			}
		}

		int m_nParamOne;
		int m_nParamTwo;

		std::vector<std::shared_ptr<const MCFCore::Misc::DownloadProvider>>& getProviderVector()
		{
			if (Providers.empty())
				Providers.push_back(Provider);
			return Providers;
		}

		std::vector<std::shared_ptr<const MCFCore::Misc::DownloadProvider>> Providers;
		std::shared_ptr<MCFCore::Misc::DownloadProvider> Provider;
		std::shared_ptr<MCFCore::Misc::DownloadProvidersI> DownloadProviders;
		MCFCore::Misc::ProviderManager ProviderManager;
		
		std::vector<std::shared_ptr<MCFCore::Thread::Misc::WGTBlock>> m_vBlocks;
        TestWGTWorker Worker;
		StubWGTController Controller;

		MCFCore::Misc::GetFile_s FileAuth;	
	};


	INSTANTIATE_TEST_CASE_P(DownloadBufferSize,
		WGTWorkerFixture,
		::testing::Values(std::make_pair(1, 1), std::make_pair(5, 1), std::make_pair(10, 1),
				std::make_pair(1, 5), std::make_pair(5, 5), std::make_pair(10, 1),
				std::make_pair(1, 10), std::make_pair(5, 10), std::make_pair(10, 10)));

	TEST_P(WGTWorkerFixture, Download_SuperBlock_VarDownloadSize)
	{
		for (int x = 0; x < 5; x++)
			AddBlock(5);

		DoDownload(GetParam().first, GetParam().second);
		CheckCompletedBlocks();
	}

	TEST_P(WGTWorkerFixture, Download_SuperBlock_VarBlockSize)
	{
		for (int x = 0; x < 5; x++)
			AddBlock(GetParam().first);

		DoDownload(5, GetParam().second);
		CheckCompletedBlocks();
	}

	TEST_P(WGTWorkerFixture, Download_SuperBlock_VarBlockCount)
	{
		for (int x = 0; x < GetParam().first; x++)
			AddBlock(5);

		DoDownload(5, GetParam().second);
		CheckCompletedBlocks();
	}
}

#endif
