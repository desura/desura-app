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

#ifndef DESURA_SERVICECORE_H
#define DESURA_SERVICECORE_H
#ifdef _WIN32
#pragma once
#endif

#include "ServiceCoreI.h"
#include "IPCPipeServer.h"

namespace IPC
{
	class PipeServer;
}

class ServiceCore : public ServiceCoreI
{
public:
	ServiceCore();
	~ServiceCore();


	void startPipe() override;
	void stopPipe() override;

	bool stopService(const char* serviceName) override;

	void destroy() override;

	void setCrashSettingCallback(CrashSettingFn crashSettingFn) override;
	void setDisconnectCallback(DisconnectFn disconnectFn) override;

	void setCrashSettings(const wchar_t* user, bool upload);
	void setTracer(TracerI *pTracer) override;

protected:
	void onDisconnect(uint32 &);

private:
	IPC::PipeServer *m_pIPCServer;

	CrashSettingFn m_pCrashSetting;
	DisconnectFn m_pDisconnect;

};

#endif //DESURA_SERVICECORE_H
