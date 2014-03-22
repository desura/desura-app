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

	void warning(const char* msg);
	void message(const char* msg);
	void debug(const char* msg);

	void updateRegKey(const char* key, const char* value) override;

#ifdef DESURA_CLIENT
	void updateBinaryRegKey(const char* key, const char* value, size_t size) override;
#else
	void updateBinaryRegKeyBlob(const char* key, IPC::PBlob blob);
#endif

#ifdef WIN32
	void removeUninstallRegKey(uint64 id) override;
	void setUninstallRegKey(uint64 id, uint64 installSize) override;

	void addDesuraToGameExplorer() override;
	void addItemGameToGameExplorer(const char* name, const char* dllPath) override;
	void removeGameFromGameExplorer(const char* dllPath, bool deleteDll = true) override;
#endif

	void updateShortCuts() override;

	void runInstallScript(const char* file, const char* installPath, const char* function) override;
	IPC::PBlob getSpecialPath(int32 key);

#ifdef DESURA_CLIENT
	IPCUpdateApp* newUpdateApp() override;
	IPCUninstallMcf* newUninstallMcf() override;
	IPCInstallMcf* newInstallMcf() override;
	IPCComplexLaunch* newComplexLaunch() override;
	IPCUninstallBranch* newUninstallBranch() override;
#endif

	void setAppDataPath(const char* path);
	void setCrashSettings(const char* user, bool upload);
	void dispVersion();

	void fixFolderPermissions(const char* dir) override;

private:
	void registerFunctions();

#ifndef DESURA_CLIENT
	void startThread();
	ServiceMainThread* m_pServiceThread;
#endif
};

#endif //DESURA_IPCMAIN_H
