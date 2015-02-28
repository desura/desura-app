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

#include "stdafx.h"

#define UTILWEB_INCLUDE

#include "Common.h"
#include "AppUpdateInstall.h"
#include "IPCPipeClient.h"
#include "IPCManager.h"
#include "IPCUpdateApp.h"

#include "util/UtilWindows.h"
#include <branding/branding.h>

class AppUpdateInstall::Privates
{
public:
	gcString m_szMcfPath;
	gcString m_szInsPath;

	ProgressReportI* m_pProgressReport;

	int m_iResult;
	bool m_bTestMode;
};

AppUpdateInstall::AppUpdateInstall(ProgressReportI* progressReport, bool testMode)
{
	m_pPrivates = new Privates();

	m_pPrivates->m_bTestMode = testMode;
	m_pPrivates->m_iResult = -4;

	m_pPrivates->m_pProgressReport = progressReport;

	char temp[255] = {0};
	GetCurrentDirectory(255, temp);

	m_pPrivates->m_szInsPath = temp;

	if (m_pPrivates->m_bTestMode)
		m_pPrivates->m_szMcfPath = "desura_update.mcf";
	else
		m_pPrivates->m_szMcfPath = UTIL::OS::getAppDataPath(UPDATEFILE_W);

#ifdef DEBUG
	m_pPrivates->m_szMcfPath = "desura_update.mcf";
	m_pPrivates->m_szInsPath = ".\\test_install\\";
#endif

	m_pEvent = CreateEvent(nullptr, true, false, nullptr);
}

AppUpdateInstall::~AppUpdateInstall()
{
	SetEvent(m_pEvent);
	CloseHandle(m_pEvent);

	try
	{
		stopService();
	}
	catch (gcException)
	{
	}

	safe_delete(m_pPrivates);
}

void AppUpdateInstall::startService()
{
	gcTrace("");

	uint32 res = UTIL::WIN::queryService(SERVICE_NAME);

	if (res != SERVICE_STATUS_STOPPED)
	{
		try
		{
			UTIL::WIN::stopService(SERVICE_NAME);
			gcSleep(500);
		}
		catch (gcException &)
		{
		}
	}

	std::vector<std::string> args;

	args.push_back("-wdir");
	args.push_back(gcString(UTIL::OS::getCurrentDir()));

	UTIL::WIN::startService(SERVICE_NAME, args);

	uint32 count = 0;
	while (UTIL::WIN::queryService(SERVICE_NAME) != SERVICE_STATUS_RUNNING)
	{
		//wait 10 seconds
		if (count > 40)
			throw gcException(ERR_SERVICE, "Failed to start desura Service (PipeManager).");

		gcSleep(250);
		count++;
	}
}

void AppUpdateInstall::stopService()
{
	gcTrace("");

	UTIL::WIN::stopService(SERVICE_NAME);
}

void AppUpdateInstall::onIPCDisconnect()
{
	gcTrace("");

	m_pPrivates->m_iResult = 4;
	SetEvent(m_pEvent);
}

int AppUpdateInstall::run()
{
	m_pPrivates->m_iResult = -1;

	try
	{
		startService();

		IPC::PipeClient pc("DesuraIS");
		pc.onDisconnectEvent += delegate(this, &AppUpdateInstall::onIPCDisconnect);

		pc.setUpPipes();
		pc.start();

		std::shared_ptr<IPCUpdateApp> ipua = IPC::CreateIPCClass< IPCUpdateApp >(&pc, "IPCUpdateApp");

		if (!ipua)
		{
			m_pPrivates->m_iResult = 2;
			throw gcException(ERR_NULLHANDLE, "Failed to create UpdateApp service.");
		}

		ipua->onCompleteEvent += delegate(this, &AppUpdateInstall::onComplete);
		ipua->onErrorEvent += delegate(this, &AppUpdateInstall::onError);
		ipua->onProgressEvent += delegate(this, &AppUpdateInstall::onProgress);

		ipua->start(m_pPrivates->m_szMcfPath.c_str(), m_pPrivates->m_szInsPath.c_str(), m_pPrivates->m_bTestMode);
		WaitForSingleObject(m_pEvent, INFINITE);

		ipua->onCompleteEvent -= delegate(this, &AppUpdateInstall::onComplete);
		ipua->onErrorEvent -= delegate(this, &AppUpdateInstall::onError);
		ipua->onProgressEvent -= delegate(this, &AppUpdateInstall::onProgress);

		pc.onDisconnectEvent -= delegate(this, &AppUpdateInstall::onIPCDisconnect);

		uint32 p = 100;

		if (m_pPrivates->m_pProgressReport)
			m_pPrivates->m_pProgressReport->reportProgress(p);
	}
	catch (gcException &e)
	{
		m_pPrivates->m_iResult = 3;
		onError(e);
	}

	try
	{
		stopService();
	}
	catch(...)
	{
	}

	return m_pPrivates->m_iResult;
}


void AppUpdateInstall::onError(gcException& e)
{
	gcTrace("Error: {0}", e);

	if (m_pPrivates->m_bTestMode)
	{
		m_pPrivates->m_iResult = e.getErrId();
		return;
	}

	unsigned int err = e.getErrId();
	if ( err != ERR_INVALIDFILE && err != ERR_FAILEDSEEK && err != ERR_INVALID )
	{
		char mbmsg[ 255 ];
		Safe::snprintf( mbmsg, 255, PRODUCT_NAME " has had a critical error while updating.\n\n%s [%d.%d]", e.getErrMsg(), err, e.getSecErrId() );
		::MessageBox( nullptr, mbmsg, PRODUCT_NAME " Critical Update Error", MB_OK );
	}
	else
	{
		if ( err != ERR_INVALID )
			m_pPrivates->m_iResult = 0;
	}
}

void AppUpdateInstall::onProgress(uint32& p)
{
	if (m_pPrivates->m_pProgressReport)
		m_pPrivates->m_pProgressReport->reportProgress(p);
}

void AppUpdateInstall::onComplete()
{
	gcTrace("");

	SetEvent(m_pEvent);
	m_pPrivates->m_iResult = 0;
}
