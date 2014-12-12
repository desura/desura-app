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

#ifndef DESURA_SAVEFILESTHREAD_H
#define DESURA_SAVEFILESTHREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "Common.h"
#include "BaseMCFThread.h"


namespace MCFCore
{
	namespace Thread
	{
		class SFTWorkerInfo;

		//! Save file thread worker buffer. Used to store the data from the mcf
		//!
		class SFTWorkerBuffer
		{
		public:
			SFTWorkerBuffer(uint32 s, char* b)
			{
				buff = b;
				size = s;
			}

			~SFTWorkerBuffer()
			{
				safe_delete(buff);
			}

			char* buff;
			uint32 size;
		};

		//! Save file thread controller. Used to exact mcf files and save into local filesystem
		//!
		class SFTController : public MCFCore::Thread::BaseMCFThread
		{
		public:
			//! Constructor
			//!
			//! @param num Number of worker threads
			//! @param caller Parent mcf
			//! @param path Path to save files to
			//!
			SFTController(uint16 num, MCFCore::MCF* caller, const char* path);
			~SFTController();

			//! Gets the status of a worker thread
			//!
			//! @parama id Worker id
			//! @return Worker status
			//!
			MCFThreadStatus getStatus(uint32 id);

			//! Gets a block from the queue
			//!
			//! @param id Worker id
			//! @param[out] status Worker status
			//! @return Block
			//!
			std::shared_ptr<SFTWorkerBuffer> getBlock(uint32 id, MCFThreadStatus &status);

			//! Creates a new save file task
			//!
			//! @param id Worker thread id
			//! @return MCFFile to save
			//!
			std::shared_ptr<MCFCore::MCFFile> newTask(uint32 id);

			//! Ends the current task when file save is complete
			//!
			//! @param id Worker id
			//! @param status Completion status
			//!
			void endTask(uint32 id, MCFThreadStatus status, gcException e = gcException());

			//! Reports an error from a worker thread
			//!
			//! @param id Worker id
			//! @param e Exception that occurred
			//!
			void reportError(uint32 id, gcException &e);

			//! Report progress from a worker
			//!
			//! @param id Worker id
			//! @param ammount amount completed
			//!
			void reportProgress(uint32 id, uint64 amount);

			//! Wake up this thread
			//!
			void pokeThread();

		protected:
			void run();

			//! Finds a Worker given a worker id
			//!
			//! @param id worker id
			//! @return Worker
			//!
			SFTWorkerInfo* findWorker(uint32 id);

			//! Fills up the worker buffers from the mcf
			//!
			//! @param fileHandle Mcf handle
			//! @return True if read one or more buffers, else false
			//!
			bool fillBuffers(UTIL::FS::FileHandle& fileHandle);

			//! Are all workers compelted
			//!
			//! @return True if all completed, false if not
			//!
			bool workersDone();

			//! Fills the list of files needed to be saved
			//!
			void fillFileList();

		private:
			gcString m_szPath;
			std::vector<SFTWorkerInfo*> m_vWorkerList;

			::Thread::WaitCondition m_WaitCond;
		};
	}
}

#endif
