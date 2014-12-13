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

#ifndef DESURA_IPCMAIN_H
#define DESURA_IPCMAIN_H
#ifdef _WIN32
#pragma once
#endif

#include "IPCClass.h"
#include "IPCParameter.h"

#include "ServiceMainI.h"

class IPCUpdateApp;
class IPCUninstallMcf;
class IPCInstallMcf;
class IPCComplexLaunch;
class IPCUninstallBranch;
class ServiceMainThread;

class IPCServiceMain : public IPC::IPCClass, public IPC::ServiceMainI
{
public:
	IPCServiceMain(IPC::IPCManager* mang, uint32 id, DesuraId itemId);
	~IPCServiceMain();

	void message(int type, const char* msg, uint64 col, std::map<std::string, std::string> mArgs);
	void updateRegKey(const char* key, const char* value);

#ifdef DESURA_CLIENT
	void updateBinaryRegKey(const char* key, const char* value, size_t size) override;
#else
	void updateBinaryRegKeyBlob(const char* key, IPC::PBlob blob);
#endif

	void setUninstallRegKey(uint64 id, uint64 installSize) override;

#ifdef WIN32
	void removeUninstallRegKey(uint64 id) override;

	void addDesuraToGameExplorer() override;
	void addItemGameToGameExplorer(const char* name, const char* dllPath) override;
	void removeGameFromGameExplorer(const char* dllPath, bool deleteDll = true) override;
#endif

	void updateShortCuts() override;

	void runInstallScript(const char* file, const char* installPath, const char* function) override;
	IPC::PBlob getSpecialPath(int32 key);

#ifdef DESURA_CLIENT
	std::shared_ptr<IPCUpdateApp> newUpdateApp() override;
	std::shared_ptr<IPCUninstallMcf> newUninstallMcf() override;
	std::shared_ptr<IPCInstallMcf> newInstallMcf() override;
	std::shared_ptr<IPCComplexLaunch> newComplexLaunch() override;
	std::shared_ptr<IPCUninstallBranch> newUninstallBranch() override;
#endif

	void setAppDataPath(const char* path);
	void setCrashSettings(const char* user, bool upload);
	void dispVersion();

	void fixFolderPermissions(const char* dir) override;

	void killProcessesAtPath(const char* szPath) override;
	uint32 findProcessId(const char* szProcessName) override;

	gc_IMPLEMENT_REFCOUNTING(IPCServiceMain);

private:
	void registerFunctions();

#ifndef DESURA_CLIENT
	void startThread();
	ServiceMainThread* m_pServiceThread;
#endif
};

#endif //DESURA_IPCMAIN_H
