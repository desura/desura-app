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

#ifndef DESURA_PIPE_H
#define DESURA_PIPE_H
#ifdef _WIN32
#pragma once
#endif

#include "IPCPipeBase.h"
#include "IPCManager.h"

namespace IPC
{

//! Class for interprocess communication that handles the client connections
//!
class PipeClient : public PipeBase, public IPCManager
{
public:
	//! Constructor
	//!
	//! @param name Pipename
	//!
	PipeClient(const char* name, LoopbackProcessor* loopbackProcessor = nullptr, uint32 managerId = 0);

	//! Desructor
	//!
	~PipeClient();

#ifdef WIN32
	void setUpPipes();
#endif
	
protected:
	IPCManager* getManager(uint32 index);
	
	//inherited from IPCPipeBase
	void disconnectAndReconnect(uint32 i);

#ifdef WIN32
	uint32 getNumEvents(){return 2;}
	
	PipeData* getData(uint32 index);
	
	void cleanUp();
#endif
	

private:
#ifdef WIN32
	bool m_bSetUped;	
	
	PipeData m_pdSend;
	PipeData m_pdRecv;
#endif
};


}


#endif //DESURA_PIPE_H
