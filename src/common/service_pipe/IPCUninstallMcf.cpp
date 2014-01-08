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
#include "IPCUninstallMcf.h"
#include "IPCManager.h"

#ifndef DESURA_CLIENT
	#include "UnInstallProcess.h"
#endif

REG_IPC_CLASS( IPCUninstallMcf );

IPCUninstallMcf::IPCUninstallMcf(IPC::IPCManager* mang, uint32 id, DesuraId itemId) : IPC::IPCClass(mang, id, itemId)
{
	registerFunctions();
	m_pThread = nullptr;
}

IPCUninstallMcf::~IPCUninstallMcf()
{
#ifndef DESURA_CLIENT
	safe_delete(m_pThread);
#endif
}


void IPCUninstallMcf::registerFunctions()
{
#ifndef DESURA_CLIENT
	REG_FUNCTION_VOID( IPCUninstallMcf, pause );
	REG_FUNCTION_VOID( IPCUninstallMcf, unpause );
	REG_FUNCTION_VOID( IPCUninstallMcf, stop );

	REG_FUNCTION_VOID( IPCUninstallMcf, start );

	LINK_EVENT( onProgressIPCEvent, uint64 );
	LINK_EVENT( onErrorEvent, gcException );
	LINK_EVENT_VOID( onCompleteEvent );
#else
	REG_EVENT( onProgressIPCEvent );
	REG_EVENT( onErrorEvent );
	REG_EVENT_VOID( onCompleteEvent );

	onProgressIPCEvent += delegate(this, &IPCUninstallMcf::onProgress);
#endif
}


#ifdef DESURA_CLIENT
void IPCUninstallMcf::start(const char* mcfpath, const char* inspath, const char* installScript)
{
	IPC::functionCallV(this, "start", mcfpath, inspath, installScript);
}

void IPCUninstallMcf::pause()
{
	IPC::functionCallV(this, "pause");
}

void IPCUninstallMcf::unpause()
{
	IPC::functionCallV(this, "unpause");
}

void IPCUninstallMcf::stop()
{
	IPC::functionCallV(this, "stop");
}

void IPCUninstallMcf::onProgress(uint64& prog)
{
	MCFCore::Misc::ProgressInfo pi(prog);
	onProgressEvent(pi);
}

#else

void IPCUninstallMcf::start(const char* mcfpath, const char* inspath, const char* installScript)
{
	if (m_pThread)
		return;

	m_pThread = new UninstallProcess(mcfpath, inspath, installScript);
	m_pThread->onErrorEvent += delegate(&onErrorEvent);
	m_pThread->onCompleteEvent += delegate(&onCompleteEvent);
	m_pThread->onProgressEvent += delegate(&onProgressIPCEvent);

	m_pThread->start();
}

void IPCUninstallMcf::pause()
{
	if (m_pThread)
		m_pThread->pause();
}

void IPCUninstallMcf::unpause()
{
	if (m_pThread)
		m_pThread->unpause();
}

void IPCUninstallMcf::stop()
{
	if (m_pThread)
		m_pThread->nonBlockStop();
}


#endif
