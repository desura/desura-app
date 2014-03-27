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
#include "ServiceCore.h"
#include "IPCPipeServer.h"

ServiceCore* g_pServiceCore = nullptr;
void StopLogging();

void SetCrashSettings(const wchar_t* user, bool upload)
{
	if (g_pServiceCore)
		g_pServiceCore->setCrashSettings(user, upload);
}

extern void SetTracer(TracerI *pTracer);


ServiceCore::ServiceCore()
{
	m_pIPCServer = nullptr;
	m_pDisconnect = nullptr;

	g_pServiceCore = this;
}

ServiceCore::~ServiceCore()
{
	g_pServiceCore = nullptr;
	stopPipe();
}

void ServiceCore::startPipe()
{
	if (m_pIPCServer)
		return;

	m_pIPCServer = new IPC::PipeServer("DesuraIS", 1, true);
	m_pIPCServer->onDisconnectEvent += delegate(this, &ServiceCore::onDisconnect);
	m_pIPCServer->start();
}

void ServiceCore::stopPipe()
{
	if (m_pIPCServer)
		m_pIPCServer->stop();

	safe_delete(m_pIPCServer);
}

bool ServiceCore::stopService(const char* serviceName)
{
#ifdef WIN32
	try
	{
		UTIL::WIN::stopService(SERVICE_NAME);
	}
	catch (gcException &)
	{
		return false;
	}
#endif
	return true;
}

void ServiceCore::destroy()
{
	delete this;
}

void ServiceCore::setCrashSettingCallback(CrashSettingFn crashSettingFn)
{
	m_pCrashSetting = crashSettingFn;
}

void ServiceCore::setDisconnectCallback(DisconnectFn disconnectFn)
{
	m_pDisconnect = disconnectFn;
}

void ServiceCore::setCrashSettings(const wchar_t* user, bool upload)
{
	if (m_pCrashSetting)
		m_pCrashSetting(user, upload);
}

void ServiceCore::onDisconnect(uint32 &)
{
	StopLogging();

	if (m_pDisconnect)
		m_pDisconnect();
}

void ServiceCore::setTracer(TracerI *pTracer)
{
	::SetTracer(pTracer);
}