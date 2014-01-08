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

#ifndef DESURA_TOOLINSTALLTHREAD_H
#define DESURA_TOOLINSTALLTHREAD_H
#ifdef _WIN32
#pragma once
#endif


#include "util_thread\BaseThread.h"

class ToolInstall;

class ToolInstallThread : public Thread::BaseThread
{
public:
	ToolInstallThread();
	~ToolInstallThread();

	gcException installTool(const char* exe, const char* args);

	Event<int32> onCompleteEvent;
	Event<gcException> onErrorEvent;

protected:
	void run();
	void onStop();

	void doInstall();
	void killProcess();

private:
	gcString m_szExe;
	gcString m_szArgs;

	volatile bool m_bActiveInstall;

	::Thread::WaitCondition m_WaitCond;

	STARTUPINFOA siStartupInfo; 
	PROCESS_INFORMATION piProcessInfo; 
};

#endif //DESURA_TOOLINSTALLTHREAD_H
