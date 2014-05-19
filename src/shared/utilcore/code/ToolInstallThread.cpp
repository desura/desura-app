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
#include "ToolInstallThread.h"
#include "ToolInstallForm.h"

void PipeIsActive();


ToolInstallThread::ToolInstallThread() : Thread::BaseThread("Install Tool Thread")
{
	m_bActiveInstall = false;
}

ToolInstallThread::~ToolInstallThread()
{
}

gcException ToolInstallThread::installTool(const char* exe, const char* args)
{
	gcTrace("Tool: {0}, Args: {1}", exe, args);

	PipeIsActive();

	if (m_bActiveInstall)
		return gcException(ERR_TOOLINSTALL, gcString("Cant install tool {0} while {1} is installing.", exe, m_szExe));

	if (!exe)
		return gcException(ERR_TOOLINSTALL, "Exe for tool install was nullptr.");

	if (!UTIL::FS::isValidFile(exe))
		return gcException(ERR_TOOLINSTALL, gcString("Exe {0} for tool install is invalid and doesnt exist.\n", exe));

	m_bActiveInstall = true;

	m_szExe = exe;
	m_szArgs = args;

	m_WaitCond.notify();

	return gcException();
}

void ToolInstallThread::run()
{
	while (!isStopped())
	{
		if (m_bActiveInstall)
			doInstall();

		m_WaitCond.wait();
	}
}

void ToolInstallThread::onStop()
{
	gcTrace("");

	killProcess();
	m_WaitCond.notify();
}

void ToolInstallThread::doInstall()
{
	gcTrace("");

	DWORD dwExitCode = 0; 

	UTIL::FS::Path path(m_szExe, "", true);


	gcString exe;
	gcString params;

	if (path.getFile().getFileExt() == "msi")
	{
		char buff[255] = {0};
		GetSystemDirectoryA(buff, 255);

		UTIL::FS::Path msiPath(buff, "", false);
		msiPath += UTIL::FS::File("msiexec.exe");

		exe = gcString("{0}", msiPath.getFullPath());
		params = gcString("msiexec.exe /i \"{0}\" {1}", m_szExe, m_szArgs);
	}
	else
	{
		exe = gcString("{0}", m_szExe);
		params = gcString("{0} {1}", path.getFile().getFile(), m_szArgs);
	}
	

	memset(&siStartupInfo, 0, sizeof(siStartupInfo)); 
	memset(&piProcessInfo, 0, sizeof(piProcessInfo)); 
	siStartupInfo.cb = sizeof(siStartupInfo); 

	std::string workingDir = path.getFolderPath();

	BOOL res = CreateProcessA(const_cast<char*>(exe.c_str()), const_cast<char*>(params.c_str()), 0, 0, false,  CREATE_DEFAULT_ERROR_MODE, 0, workingDir.c_str(),  &siStartupInfo, &piProcessInfo);

	if (res == false)
	{
		DWORD err = GetLastError();
		gcTrace("Failed to start: {0}", err);

		m_bActiveInstall = false;
		gcException gce(ERR_LAUNCH, err, gcString("Failed to start exe {0} for tools install. [P: {1}]", exe, params));
		onErrorEvent(gce);
	}
	else
	{
		WaitForSingleObject(piProcessInfo.hProcess, INFINITE); 

		int32 dwExitCode = 0;
		GetExitCodeProcess(piProcessInfo.hProcess, (DWORD*)&dwExitCode);

		gcTrace("Exit Code: {0}", dwExitCode);

		m_bActiveInstall = false;

		if (!isStopped())
			onCompleteEvent(dwExitCode);
	}

	CloseHandle( piProcessInfo.hProcess );
	CloseHandle( piProcessInfo.hThread );
}

void ToolInstallThread::killProcess()
{
	gcTrace("");

	if (!m_bActiveInstall)
		return;

	::PostThreadMessage(piProcessInfo.dwThreadId, WM_CLOSE, 0, 0);
	::WaitForSingleObject(piProcessInfo.hProcess, 1000);
	  
	// Check exit code
	DWORD dwExitCode = 0;
	::GetExitCodeProcess(piProcessInfo.hProcess, &dwExitCode);

	if(dwExitCode == STILL_ACTIVE)
		::TerminateProcess(piProcessInfo.hProcess, -1);
}