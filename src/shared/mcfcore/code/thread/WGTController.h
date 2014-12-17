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

#ifndef DESURA_WEBGETTHREAD_H
#define DESURA_WEBGETTHREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "BaseMCFThread.h"
#include "WGTControllerI.h"

#include "WGTExtras.h"
#include "mcfcore/DownloadProvider.h"
#include "ProviderManager.h"
#include "mcfcore/MCFI.h"

#include <atomic>

namespace MCFCore
{

	namespace Thread
	{
		class WGTWorkerInfo;

		class WGTWorkerList;

		//! Web Get Thread controller. Downloads mcf from mcf servers
		//!
		class WGTController : public MCFCore::Thread::BaseMCFThread, protected WGTControllerI
		{
		public:
			//! Constuctor
			//!
			//! @param source Download provider list
			//! @param numWorkers Number of children to spawn to download the mcf
			//! @param caller Parent Mcf
			//! @param checkMcf Check the Mcf for downloaded chunks before starting
			//!
			WGTController(std::shared_ptr<MCFCore::Misc::DownloadProvidersI> pDownloadProviders, uint16 numWorkers, MCFCore::MCF* caller, bool checkMcf);
			~WGTController();

			//! Provider event
			//!
			Event<MCFCore::Misc::DP_s> onProviderEvent;

		protected:
			//! Has blocks ready to written to the mcf file
			//!
			//! @return True if blocks are ready, false if not
			//!
			bool isQueuedBlocks();

			//! Finds a Worker given a worker id
			//!
			//! @param id worker id
			//! @return Worker
			//!
			std::shared_ptr<WGTWorkerInfo> findWorker(uint32 id);

			//! Fills the block list with all the blocks needed to be downloaded
			//!
			//! @return True if compelted, False if error occured
			//!
			bool fillBlockList();

			//! Creates the workers
			//!
			void createWorkers();

			//! Are all workers done
			//!
			//! @return True if all done, false if not
			//!
			bool workersDone();

			//! Save completed blocks to the mcf file
			//!
			//! @param fileHandle Mcf file to save to
			//! @param allBlocks Save all blocks instead of one
			//!
			void saveBuffers(UTIL::FS::FileHandle& fileHandle, bool allBlocks = false);

			//inhereted from BaseThread
			void run() override;
			void onStop() override;

			//! Checks a block for errors
			//!
			bool checkBlock(const std::shared_ptr<Misc::WGTBlock> &block, uint32 workerId);

			//! Steals blocks from other workes
			//!
			//! @return true if blocks stolen
			//!
			std::shared_ptr<Misc::WGTSuperBlock> stealBlocks();

			bool newTask(uint32 id, MCFThreadStatus &status, std::shared_ptr<Misc::WGTSuperBlock> &pSuperBlock) override;
			MCFThreadStatus getStatus(uint32 id) override;
			void reportError(uint32 id, gcException &e, std::shared_ptr<Misc::WGTSuperBlock> &pSuperBlock) override;
			void reportProgress(uint32 id, uint64 ammount) override;
			void reportNegProgress(uint32 id, uint64 ammount) override;
			void workerFinishedBlock(uint32 id, std::shared_ptr<Misc::WGTBlock> &block) override;
			void workerFinishedSuperBlock(uint32 id, std::shared_ptr<Misc::WGTSuperBlock> &pSuperBlock) override;
			void pokeThread() override;

		private:
			MCFCore::Misc::ProviderManager m_ProvManager;

            std::atomic<uint32> m_iAvailbleWork = {0};
            std::atomic<uint32> m_iRunningWorkers = {0};

			bool m_bCheckMcf = false;
			volatile bool m_bDoingStop = false;

			std::unique_ptr<WGTWorkerList> m_pWorkerList;
			const std::vector<std::shared_ptr<WGTWorkerInfo>>& m_vWorkerList;
			std::deque<std::shared_ptr<Misc::WGTSuperBlock>> m_vSuperBlockList;
			std::vector<uint32> m_vDlFiles;

			::Thread::WaitCondition m_WaitCondition;


			std::mutex m_McfLock;
			MCFCore::MCF* m_pCurMcf = nullptr;

#ifdef DEBUG
			uint64 m_uiSaved = 0;
#endif
		};
	}
}

#endif //DESURA_WEBGETTHREAD_H
