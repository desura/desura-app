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
#include "IPCUninstallBranch.h"
#include "IPCManager.h"

#ifndef DESURA_CLIENT
	#include "UnInstallBranchProcess.h"
#endif

REG_IPC_CLASS( IPCUninstallBranch );

IPCUninstallBranch::IPCUninstallBranch(IPC::IPCManager* mang, uint32 id, DesuraId itemId) : IPC::IPCClass(mang, id, itemId)
{
	registerFunctions();
	m_pThread = nullptr;
}

IPCUninstallBranch::~IPCUninstallBranch()
{
#ifndef DESURA_CLIENT
	safe_delete(m_pThread);
#endif
}

void IPCUninstallBranch::registerFunctions()
{
#ifndef DESURA_CLIENT
	REG_FUNCTION_VOID( IPCUninstallBranch, pause );
	REG_FUNCTION_VOID( IPCUninstallBranch, unpause );
	REG_FUNCTION_VOID( IPCUninstallBranch, stop );

	REG_FUNCTION_VOID( IPCUninstallBranch, start );

	LINK_EVENT( onProgressIPCEvent, uint64 );
	LINK_EVENT( onErrorEvent, gcException );
	LINK_EVENT_VOID( onCompleteEvent );
#else
	REG_EVENT( onProgressIPCEvent );
	REG_EVENT( onErrorEvent );
	REG_EVENT_VOID( onCompleteEvent );

	onProgressIPCEvent += delegate(this, &IPCUninstallBranch::onProgress);
#endif
}


#ifdef DESURA_CLIENT

void IPCUninstallBranch::start(const char* oldBranchMcf, const char* newBranchMcf, const char* inspath, const char* installScript)
{
	IPC::functionCallV(this, "start", oldBranchMcf, newBranchMcf, inspath, installScript);
}

void IPCUninstallBranch::pause()
{
	IPC::functionCallAsync(this, "pause");
}

void IPCUninstallBranch::unpause()
{
	IPC::functionCallAsync(this, "unpause");
}

void IPCUninstallBranch::stop()
{
	IPC::functionCallAsync(this, "stop");
}


void IPCUninstallBranch::onProgress(uint64& prog)
{
	MCFCore::Misc::ProgressInfo pi(prog);
	onProgressEvent(pi);
}

#else

void IPCUninstallBranch::start(const char* oldBranchMcf, const char* newBranchMcf, const char* inspath, const char* installScript)
{
	if (m_pThread)
		return;

	m_pThread = new UninstallBranchProcess(oldBranchMcf, newBranchMcf, inspath, installScript);
	m_pThread->onErrorEvent += delegate(&onErrorEvent);
	m_pThread->onCompleteEvent += delegate(&onCompleteEvent);
	m_pThread->onProgressEvent += delegate(&onProgressIPCEvent);
	m_pThread->onFinishEvent += delegate(&onFinishEvent);

	m_pThread->start();
}

void IPCUninstallBranch::pause()
{
	if (m_pThread)
		m_pThread->pause();
}

void IPCUninstallBranch::unpause()
{
	if (m_pThread)
		m_pThread->unpause();
}

void IPCUninstallBranch::stop()
{
	if (m_pThread)
		m_pThread->nonBlockStop();
}


#endif

