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

#ifndef DESURA_WGTCONTROLLERI_H
#define DESURA_WGTCONTROLLERI_H
#ifdef _WIN32
#pragma once
#endif

#include "WGTExtras.h"

namespace MCFCore
{
	namespace Thread
	{
		class WGTControllerI
		{
		public:
			//! Request a new task (Worker thread)
			//!
			//! @param id Worker id
			//! @param[out] status Current worker status
			//! @return New worker task
			//!
			virtual bool newTask(uint32 id, MCFThreadStatus &status, std::shared_ptr<Misc::WGTSuperBlock> &pSuperBlock) = 0;

			//! Gets the status of a worker
			//!
			//! @param id Worker id
			//! @return Worker status
			//!
			virtual MCFThreadStatus getStatus(uint32 id) = 0;

			//! Reports an error from a worker thread
			//!
			//! @param id Worker id
			//! @param e Exception that occurred
			//!
			virtual void reportError(uint32 id, gcException &e, std::shared_ptr<Misc::WGTSuperBlock> &pSuperBlock) = 0;

			//! Report progress from a worker
			//!
			//! @param id Worker id
			//! @param ammount amount completed
			//!
			virtual void reportProgress(uint32 id, uint64 amount) = 0;
			virtual void reportNegProgress(uint32 id, uint64 amount) = 0;

			//! Report that a worker has completed a block
			//!
			//! @param id Worker id
			//!
			virtual void workerFinishedBlock(uint32 id, std::shared_ptr<Misc::WGTBlock> &block) = 0;

			//! Report that a worker has completed a super block
			//!
			//! @param id Worker id
			//!
			virtual void workerFinishedSuperBlock(uint32 id, std::shared_ptr<Misc::WGTSuperBlock> &pSuperBlock) = 0;

			//! Wake up the controller
			//!
			virtual void pokeThread() = 0;
		};
	}
}

#endif //DESURA_WGTCONTROLLERI_H
