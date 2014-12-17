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

#ifndef DESURA_APPUPDATEINSTALL_H
#define DESURA_APPUPDATEINSTALL_H
#ifdef _WIN32
#pragma once
#endif

#ifndef uint64
#define uint64 unsigned long long
#endif

#include "gcError.h"

class UpdateForm;

class ProgressReportI
{
public:
	virtual void reportProgress(uint32 prog)=0;
};

class AppUpdateInstall
{
public:
	AppUpdateInstall(ProgressReportI* progressReport, bool testMode = false);
	~AppUpdateInstall();

	virtual int run();

protected:
	void onError(gcException& e);
	void onProgress(uint32& p);
	void onComplete();

	void startService();
	void stopService();

	void onIPCDisconnect();

	HANDLE m_pEvent;

private:
	class Privates;
	Privates *m_pPrivates;
};

#endif //DESURA_APPUPDATEINSTALL_H
