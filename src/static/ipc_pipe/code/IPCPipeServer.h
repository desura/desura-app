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

#ifndef DESURA_IPCPIPESERVER_H
#define DESURA_IPCPIPESERVER_H
#ifdef _WIN32
#pragma once
#endif

#include "IPCPipeBase.h"
#include "IPCPipeAuth.h"

namespace IPC
{

//! Class for interprocess communication that handles the server connections
//!
class PipeServer : public PipeBase
{
public:
	//! Constuctor
	//!
	//! @param name PipeName
	//! @param numPipes Number of pipes avaliable to connect to
	//!
	PipeServer(const char* name, uint8 numPipes = 4, bool changeAccess = false);

	//! Desructor
	//!
	~PipeServer();

	//! Event that gets triggered when authentication is needed for pipe connect
	//!
	Event<PipeItemAuth> onNeedAuthEvent;

	//! Event that gets triggered when one pipe disconects
	//! Passes in pipe number
	//!
	Event<uint32> onDisconnectEvent;

	//! Event that gets triggered when one pipe gets connected two
	//! Passes in pipe number
	//!
	Event<uint32> onConnectEvent;


	IPCManager* getManager(uint32 index);

protected:

#ifdef WIN32	
	virtual void run();

	//! Init a new client pipe
	//!
	//! @param p Pipe Instance
	//! @param e Send event handler
	//!
	void initNewClient(uint32 index, PipeInst *p, HANDLE e);

	//! Connect to a named pipe instance
	//!
	//! @param data Pipe Data
	//!
	void connectNamedPipe(uint32 index, PipeData* data);
	
#endif
	//inherited from IPCPipeBase
	void setUpPipes();
#ifdef WIN32
	void disconnectAndReconnect(uint32 i);

	uint32 getNumEvents(){return 2*m_uiNumPipes;}
	PipeData* getData(uint32 index);	
	void createAccessRights(PACL pNewAcl, SECURITY_ATTRIBUTES &sa, SECURITY_DESCRIPTOR &sd);
#endif
	
private:
#ifdef WIN32
	bool m_bChangeAccess;
	uint8 m_uiNumPipes;
	std::vector<PipeInst*> m_vPipeInst;

	gcString m_szName;
#else
	PipeInst* m_pPipe;
#endif
};


}

#endif //DESURA_IPCPIPESERVER_H
