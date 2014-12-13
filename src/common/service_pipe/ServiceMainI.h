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


#ifndef DESURA_SERVICEMAINI_H
#define DESURA_SERVICEMAINI_H
#ifdef _WIN32
#pragma once
#endif

class IPCUpdateApp;
class IPCUninstallMcf;
class IPCInstallMcf;
class IPCComplexLaunch;
class IPCUninstallBranch;

namespace IPC
{
	class ServiceMainI : public gcRefBase
	{
	public:
		virtual void updateRegKey(const char* key, const char* value)=0;

#ifdef DESURA_CLIENT
		virtual void updateBinaryRegKey(const char* key, const char* value, size_t size)=0;
#else
		virtual void updateBinaryRegKeyBlob(const char* key, IPC::PBlob blob)=0;
#endif

		virtual void updateShortCuts()=0;
		virtual void runInstallScript(const char* file, const char* installPath, const char* function)=0;

		virtual void fixFolderPermissions(const char* dir)=0;

#ifdef DESURA_CLIENT
		virtual std::shared_ptr<IPCUpdateApp> newUpdateApp()=0;
		virtual std::shared_ptr<IPCUninstallMcf> newUninstallMcf()=0;
		virtual std::shared_ptr<IPCInstallMcf> newInstallMcf()=0;
		virtual std::shared_ptr<IPCComplexLaunch> newComplexLaunch()=0;
		virtual std::shared_ptr<IPCUninstallBranch> newUninstallBranch()=0;
#endif

#ifdef WIN32
		virtual void removeUninstallRegKey(uint64 id)=0;
		virtual void setUninstallRegKey(uint64 id, uint64 installSize)=0;

		virtual void addDesuraToGameExplorer()=0;
		virtual void addItemGameToGameExplorer(const char* name, const char* dllPath)=0;
		virtual void removeGameFromGameExplorer(const char* dllPath, bool deleteDll = true)=0;
#endif

		virtual void killProcessesAtPath(const char* szPath) = 0;
		virtual uint32 findProcessId(const char* szProcessName) = 0;

	protected:
		virtual ~ServiceMainI(){}
	};

#ifdef LINK_WITH_GMOCK
	class ServiceMainMock : public ServiceMainI
	{
	public:
		MOCK_METHOD2(updateRegKey, void(const char*, const char*));

#ifdef DESURA_CLIENT
		MOCK_METHOD3(updateBinaryRegKey, void(const char*, const char*, size_t));
#else
		MOCK_METHOD2(updateBinaryRegKeyBlob, void(const char*, IPC::PBlob));
#endif

		MOCK_METHOD0(updateShortCuts, void());
		MOCK_METHOD3(runInstallScript, void(const char*, const char*, const char*));

		MOCK_METHOD1(fixFolderPermissions, void(const char*));

#ifdef DESURA_CLIENT
		MOCK_METHOD0(newUpdateApp, std::shared_ptr<IPCUpdateApp>());
		MOCK_METHOD0(newUninstallMcf, std::shared_ptr<IPCUninstallMcf>());
		MOCK_METHOD0(newInstallMcf, std::shared_ptr<IPCInstallMcf>());
		MOCK_METHOD0(newComplexLaunch, std::shared_ptr<IPCComplexLaunch>());
		MOCK_METHOD0(newUninstallBranch, std::shared_ptr<IPCUninstallBranch>());
#endif

#ifdef WIN32
		MOCK_METHOD1(removeUninstallRegKey, void(uint64));
		MOCK_METHOD2(setUninstallRegKey, void(uint64, uint64));

		MOCK_METHOD0(addDesuraToGameExplorer, void());
		MOCK_METHOD2(addItemGameToGameExplorer, void(const char*, const char*));
		MOCK_METHOD2(removeGameFromGameExplorer, void(const char*, bool));
#endif

		MOCK_METHOD1(killProcessesAtPath, void(const char*));
		MOCK_METHOD1(findProcessId, uint32(const char*));

		gc_IMPLEMENT_REFCOUNTING(ServiceMainMock);
	};
#endif
}


#endif
