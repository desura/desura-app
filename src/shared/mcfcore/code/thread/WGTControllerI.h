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
	virtual Misc::WGTSuperBlock* newTask(uint32 id, uint32 &status)=0;

	//! Gets the status of a worker
	//!
	//! @param id Worker id
	//! @return Worker status
	//!
	virtual uint32 getStatus(uint32 id)=0;

	//! Reports an error from a worker thread
	//!
	//! @param id Worker id
	//! @param e Exception that occured
	//!
	virtual void reportError(uint32 id, gcException &e)=0;

	//! Report progress from a worker
	//!
	//! @param id Worker id
	//! @param ammount Ammount completed
	//!
	virtual void reportProgress(uint32 id, uint64 ammount)=0;
	virtual void reportNegProgress(uint32 id, uint64 ammount)=0;
	
	//! Report that a worker has completed a block
	//!
	//! @param id Worker id
	//!
	virtual void workerFinishedBlock(uint32 id, Misc::WGTBlock* block)=0;

	//! Report that a worker has completed a super block
	//!
	//! @param id Worker id
	//!
	virtual void workerFinishedSuperBlock(uint32 id)=0;

	//! Wake up the controller
	//!
	virtual void pokeThread()=0;
};

}
}

#endif //DESURA_WGTCONTROLLERI_H
