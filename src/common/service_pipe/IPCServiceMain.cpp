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
#include "IPCServiceMain.h"
#include "IPCManager.h"

#include "IPCUninstallBranch.h"
#include "IPCInstallMcf.h"
#include "IPCUninstallMcf.h"
#include "IPCComplexLaunch.h"
#include "IPCUpdateApp.h"
#include <branding/servicecore_version.h>

#include "Color.h"

#ifndef DESURA_CLIENT
#include "InstallScriptRunTime.h"
#endif

#include "SharedObjectLoader.h"

#ifdef WIN32
typedef BOOL (WINAPI* SetDllDirectoryFunc)(LPCTSTR);
#endif

IPCServiceMain* g_pServicemain = nullptr;

#ifndef DESURA_CLIENT

#include "ServiceMainThread.h"

bool g_bLogEnabled = true;

void StopLogging()
{
	g_bLogEnabled = false;
}

gcString GetSpecialPath(int32 key)
{
	if (g_pServicemain)
	{
		IPC::PBlob str = g_pServicemain->getSpecialPath(key);
		return std::string(str.getData(), str.getSize());
	}

	return "SERVICE CORE IS NULL";
}

TracerI *g_pTracer = nullptr;

void SetTracer(TracerI *pTracer)
{
	g_pTracer = pTracer;
}

void LogMsg(MSG_TYPE type, std::string msg, Color* col, std::map<std::string, std::string> *mpArgs)
{
	if (!g_pServicemain || !g_bLogEnabled)
		return;

#ifndef DEBUG
	if (type == MT_DEBUG)
		return;
#endif

	uint64 nCol = -1;

	if (col)
		nCol = col->getColor();

	if (mpArgs)
	{
		g_pServicemain->message((int)type, msg.c_str(), nCol, *mpArgs);
	}
	else
	{
		std::map<std::string, std::string> mEmpty;
		g_pServicemain->message((int)type, msg.c_str(), nCol, mEmpty);
	}

	if (g_pTracer && type == MT_TRACE)
		g_pTracer->trace(msg, mpArgs);
}

#endif


REG_IPC_CLASS( IPCServiceMain );


IPCServiceMain::IPCServiceMain(IPC::IPCManager* mang, uint32 id, DesuraId itemId) : IPC::IPCClass(mang, id, itemId)
{
	registerFunctions();
	g_pServicemain = this;

#ifndef DESURA_CLIENT
	m_pServiceThread = nullptr;
#endif
}

IPCServiceMain::~IPCServiceMain()
{
	g_pServicemain = nullptr;

#ifndef DESURA_CLIENT
	if (m_pServiceThread)
		m_pServiceThread->stop();

	safe_delete(m_pServiceThread);
#endif
}

void IPCServiceMain::registerFunctions()
{
#ifndef DESURA_CLIENT
	REG_FUNCTION_VOID( IPCServiceMain, updateRegKey );
	REG_FUNCTION_VOID( IPCServiceMain, updateBinaryRegKeyBlob);

	REG_FUNCTION_VOID( IPCServiceMain, dispVersion );
	
	REG_FUNCTION_VOID( IPCServiceMain, setAppDataPath );
	REG_FUNCTION_VOID( IPCServiceMain, setCrashSettings );

#ifdef WIN32
	REG_FUNCTION_VOID( IPCServiceMain, removeUninstallRegKey);
	REG_FUNCTION_VOID( IPCServiceMain, setUninstallRegKey);
	REG_FUNCTION_VOID( IPCServiceMain, addDesuraToGameExplorer);
	REG_FUNCTION_VOID( IPCServiceMain, addItemGameToGameExplorer);
	REG_FUNCTION_VOID( IPCServiceMain, removeGameFromGameExplorer);
#endif

	REG_FUNCTION_VOID( IPCServiceMain, updateShortCuts);
	REG_FUNCTION_VOID( IPCServiceMain, fixFolderPermissions);
	REG_FUNCTION_VOID( IPCServiceMain, runInstallScript);
#else
	REG_FUNCTION_VOID_T( IPCServiceMain, message, false );
	REG_FUNCTION( IPCServiceMain, getSpecialPath );
#endif
}



#ifdef DESURA_CLIENT

#ifdef WIN32
#include <shlobj.h>
#endif

#ifdef NIX
#define MAX_PATH 255
#endif

IPC::PBlob IPCServiceMain::getSpecialPath(int32 key)
{
	char path[MAX_PATH] = "NOT SUPPORTED";

#ifdef WIN32
	switch (key)
	{
	case 1:
		SHGetFolderPath(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, path);
	case 2:
		SHGetFolderPath(nullptr, CSIDL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, path);
	};

#endif

	return IPC::PBlob(path, strlen(path));
}

void IPCServiceMain::message(int type, const char* msg, uint64 col, std::map<std::string, std::string> mArgs)
{
	Color color;
	Color *pCol = &color;

	if (col != -1)
		color = Color((uint32)col);
	else if (type == MT_DEBUG)
		color = Color(0x65, 0x33, 0x66);
	else if (type == MT_WARN)
		color = Color(0xFF, 0x99, 0x33);
	else
		color = Color(65, 209, 7);

	if (type == MT_TRACE)
	{
		mArgs["app"] = "Service";
		LogMsg((MSG_TYPE)type, gcString(msg), pCol, &mArgs);
	}
	else
	{
		LogMsg((MSG_TYPE)type, gcString("Service: {0}", msg), pCol, &mArgs);
	}
}

void IPCServiceMain::updateRegKey(const char* key, const char* value)
{
	IPC::functionCallV(this, "updateRegKey", key, value);
}

void IPCServiceMain::updateBinaryRegKey(const char* key, const char* value, size_t size)
{
	IPC::PBlob blob(value, size);
	IPC::functionCallV(this, "updateBinaryRegKeyBlob", key, blob);
}

std::shared_ptr<IPCUpdateApp> IPCServiceMain::newUpdateApp()
{
	return IPC::CreateIPCClass< IPCUpdateApp >(m_pManager, "IPCUpdateApp");
}

std::shared_ptr<IPCUninstallMcf> IPCServiceMain::newUninstallMcf()
{
	return IPC::CreateIPCClass< IPCUninstallMcf >(m_pManager, "IPCUninstallMcf");
}

std::shared_ptr<IPCInstallMcf> IPCServiceMain::newInstallMcf()
{
	return IPC::CreateIPCClass< IPCInstallMcf >(m_pManager, "IPCInstallMcf");
}

std::shared_ptr<IPCComplexLaunch> IPCServiceMain::newComplexLaunch()
{
	return IPC::CreateIPCClass< IPCComplexLaunch >(m_pManager, "IPCComplexLaunch");
}

std::shared_ptr<IPCUninstallBranch> IPCServiceMain::newUninstallBranch()
{
	return IPC::CreateIPCClass< IPCUninstallBranch >(m_pManager, "IPCUninstallBranch");
}

void IPCServiceMain::dispVersion()
{
	IPC::functionCallAsync(this, "dispVersion");
}

void IPCServiceMain::setAppDataPath(const char* path)
{
	IPC::functionCallAsync(this, "setAppDataPath", path);
}

void IPCServiceMain::setCrashSettings(const char* user, bool upload)
{
	IPC::functionCallAsync(this, "setCrashSettings", user, upload);
}

#ifdef WIN32
void IPCServiceMain::removeUninstallRegKey(uint64 id)
{
	IPC::functionCallAsync(this, "removeUninstallRegKey", id);
}

void IPCServiceMain::setUninstallRegKey(uint64 id, uint64 installSize)
{
	IPC::functionCallAsync(this, "setUninstallRegKey", id, installSize);
}

void IPCServiceMain::addDesuraToGameExplorer()
{
	IPC::functionCallAsync(this, "addDesuraToGameExplorer");
}

void IPCServiceMain::addItemGameToGameExplorer(const char* name, const char* dllPath)
{
	IPC::functionCallAsync(this, "addItemGameToGameExplorer", name, dllPath);
}

void IPCServiceMain::removeGameFromGameExplorer(const char* dllPath, bool deleteDll)
{
	IPC::functionCallAsync(this, "removeGameFromGameExplorer", dllPath, deleteDll);
}
#endif

void IPCServiceMain::updateShortCuts()
{
	IPC::functionCallAsync(this, "updateShortCuts");
}

void IPCServiceMain::fixFolderPermissions(const char* dir)
{
	IPC::functionCallV(this, "fixFolderPermissions", dir);
}

void IPCServiceMain::runInstallScript(const char* file, const char* installpath, const char* function)
{
	IPC::functionCallAsync(this, "runInstallScript", file, installpath, function);
}

#else

IPC::PBlob IPCServiceMain::getSpecialPath(int32 key)
{
	return IPC::functionCall<IPC::PBlob, int32>(this, "getSpecialPath", key);
}

void IPCServiceMain::message(int type, const char* msg, uint64 col, std::map<std::string, std::string> mArgs)
{
	IPC::functionCallAsync(this, "message", type, msg, col, mArgs);
}

void IPCServiceMain::startThread()
{
	if (!m_pServiceThread)
	{
		m_pServiceThread = new ServiceMainThread();
		m_pServiceThread->start();
	}
}

void SetCrashSettings(const wchar_t* user, bool upload);

#ifdef WIN32
void UnInstallRegKey_SetAppDataPath(const char* path);
#endif

void IPCServiceMain::setAppDataPath(const char* path)
{
#ifdef WIN32
	UnInstallRegKey_SetAppDataPath(path);
#endif
}

void IPCServiceMain::setCrashSettings(const char* user, bool upload)
{
	gcWString wUser(user);
	SetCrashSettings(wUser.c_str(), upload);
}

void IPCServiceMain::dispVersion()
{
	Msg(gcString("Version: {0}.{1}.{2}.{3}\n", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILDNO, VERSION_EXTEND));
}

void IPCServiceMain::fixFolderPermissions(const char* dir)
{
	try
	{
		UTIL::FS::recMakeFolder(dir);
#ifdef WIN32
		UTIL::WIN::changeFolderPermissions(gcWString(dir));
#endif // LINUX TODO
	}
	catch (gcException &e)
	{
		Warning("Failed to fix folder permissions: {0}\n", e);
	}
}


void UpdateShortCuts();

#ifdef WIN32
void RemoveUninstallRegKey(DesuraId id);
bool SetUninstallRegKey(DesuraId id, uint64 installSize);

void AddDesuraToWIndowsGameExplorer();
void AddGameToWindowsGameExplorer(const char* name, const char* dllPath);
void RemoveGameFromWindowsGameExplorer(const char* dllPath, bool deleteDll);

class RemoveUninstallTask : public TaskI
{
public:
	RemoveUninstallTask(uint64 id)
	{
		this->id = id;
	}

	void doTask()
	{
		RemoveUninstallRegKey(DesuraId(id));
	}

	void destroy()
	{
		delete this;
	}

	uint64 id;
};

class SetUninstallTask : public TaskI
{
public:
	SetUninstallTask(uint64 id, uint64 installSize)
	{
		this->id = id;
		this->installSize = installSize;
	}

	void doTask()
	{
		SetUninstallRegKey(DesuraId(id), installSize);
	}

	void destroy()
	{
		delete this;
	}

	uint64 id;
	uint64 installSize;
};

class AddDesuraToWGETask : public TaskI
{
public:
	void doTask()
	{
		AddDesuraToWIndowsGameExplorer();
	}

	void destroy()
	{
		delete this;
	}
};

class AddToWGETask : public TaskI
{
public:
	AddToWGETask(const char* name, const char* dllPath)
	{
		m_szDllPath = dllPath;
		m_szName = name;
	}

	void doTask()
	{
		AddGameToWindowsGameExplorer(m_szName.c_str(), m_szDllPath.c_str());
	}

	void destroy()
	{
		delete this;
	}

	gcString m_szDllPath;
	gcString m_szName;
};

class RemoveFromWGETask : public TaskI
{
public:
	RemoveFromWGETask(const char* dllPath, bool deleteDll)
	{
		m_szDllPath = dllPath;
		m_bDelete = deleteDll;
	}

	void doTask()
	{
		RemoveGameFromWindowsGameExplorer(m_szDllPath.c_str(), m_bDelete);
	}

	void destroy()
	{
		delete this;
	}

	gcString m_szDllPath;
	bool m_bDelete;
};

#endif


class UpdateShortcutTask : public TaskI
{
public:
	void doTask()
	{
		UpdateShortCuts();
	}

	void destroy()
	{
		delete this;
	}
};




void IPCServiceMain::updateRegKey(const char* key, const char* value)
{
	if (!key || !value)
		return;

	try
	{
		UTIL::OS::setConfigValue(key, value);
	}
	catch (gcException &e)
	{
		Warning("Failed to set reg key: {0}\n", e);
	}
}

void IPCServiceMain::updateBinaryRegKeyBlob(const char* key, IPC::PBlob blob)
{
	if (!key || !blob.getSize())
		return;

	try
	{
		UTIL::OS::setConfigBinaryValue(key, blob.getData(), blob.getSize(), false);
	}
	catch (gcException &e)
	{
		Warning("Failed to set reg key: {0}\n", e);
	}
}

#ifdef WIN32
void IPCServiceMain::removeUninstallRegKey(uint64 id)
{
	startThread();
	m_pServiceThread->addTask(new RemoveUninstallTask(id));
}

void IPCServiceMain::setUninstallRegKey(uint64 id, uint64 installSize)
{
	startThread();
	m_pServiceThread->addTask(new SetUninstallTask(id, installSize));	
}

void IPCServiceMain::addDesuraToGameExplorer()
{
	startThread();
	m_pServiceThread->addTask(new AddDesuraToWGETask());	
}

void IPCServiceMain::addItemGameToGameExplorer(const char* name, const char* dllPath)
{
	startThread();
	m_pServiceThread->addTask(new AddToWGETask(name, dllPath));		
}

void IPCServiceMain::removeGameFromGameExplorer(const char* dllPath, bool deleteDll)
{
	startThread();
	m_pServiceThread->addTask(new RemoveFromWGETask(dllPath, deleteDll));		
}

#endif


void IPCServiceMain::updateShortCuts()
{
	startThread();
	m_pServiceThread->addTask(new UpdateShortcutTask());	
}

void IPCServiceMain::runInstallScript(const char* file, const char* installpath, const char* function)
{
	try
	{
		InstallScriptRunTime isr(file, installpath);
		isr.run(function);
	}
	catch (gcException &e)
	{
		Warning("Failed to execute script {0}: {1}\n", file, e);
	}
}


#endif
