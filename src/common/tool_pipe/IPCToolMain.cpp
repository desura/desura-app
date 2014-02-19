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
#include "IPCToolMain.h"
#include "IPCManager.h"

#ifndef DESURA_CLIENT
#include "ToolInstallThread.h"
#include "LogCallback.h"
#else
#include "ToolInfo.h"
#endif

IPCToolMain* g_pToolMain = nullptr;
REG_IPC_CLASS( IPCToolMain );

extern LogCallback* g_pLogCallback;


#ifndef DESURA_CLIENT

bool g_bLogEnabled = true;

void IPCToolPipe_LogMsg(int type, std::string msg, Color* col)
{
	OutputDebugStringA(msg.c_str());

	if (!g_pToolMain || !g_bLogEnabled)
		return;

	switch (type)
	{
	case MT_MSG:
	case MT_MSG_COL:
		g_pToolMain->message(msg.c_str());
		break;

	case MT_WARN:
		g_pToolMain->warning(msg.c_str());
		break;
	};
}

void IPCToolPipe_LogMsg(int type, std::wstring msg, Color* col)
{
	OutputDebugStringW(msg.c_str());

	if (!g_pToolMain || !g_bLogEnabled)
		return;

	switch (type)
	{
	case MT_MSG:
	case MT_MSG_COL:
		g_pToolMain->message(gcString(msg).c_str());
		break;

	case MT_WARN:
		g_pToolMain->warning(gcString(msg).c_str());
		break;
	};
}




void IPCToolPipe_Msg(const char* msg, Color *col = nullptr)
{
	if (!msg)
		return;

	IPCToolPipe_LogMsg(MT_MSG, msg, col);
}

void IPCToolPipe_Msg_W(const wchar_t* msg, Color *col = nullptr)
{
	if (!msg)
		return;

	IPCToolPipe_LogMsg(MT_MSG, msg, col);
}

void IPCToolPipe_Warn(const char* msg)
{
	if (!msg)
		return;

	IPCToolPipe_LogMsg(MT_WARN, msg, nullptr);
}

void IPCToolPipe_Warn_W(const wchar_t* msg)
{
	if (!msg)
		return;

	IPCToolPipe_LogMsg(MT_WARN, msg, nullptr);
}

void IPCToolPipe_Debug(const char* msg)
{
#ifdef DEBUG
	if (!msg)
		return;

	IPCToolPipe_LogMsg(MT_MSG, msg, nullptr);
#endif
}

void IPCToolPipe_Debug_W(const wchar_t* msg)
{
#ifdef DEBUG
	if (!msg)
		return;

	IPCToolPipe_LogMsg(MT_MSG, msg, nullptr);
#endif
}

#endif


IPCToolMain::IPCToolMain(IPC::IPCManager* mang, uint32 id, DesuraId itemId) : IPC::IPCClass(mang, id, itemId)
{
	registerFunctions();
	g_pToolMain = this;

#ifndef DESURA_CLIENT
	m_pThread = nullptr;
#endif
}

IPCToolMain::~IPCToolMain()
{
#ifndef DESURA_CLIENT
	safe_delete(m_pThread);
#endif
}

#ifndef DESURA_CLIENT
void IPCToolMain::SetupLogging()
{
	safe_delete(g_pLogCallback);
	g_pLogCallback = new LogCallback();

	g_pLogCallback->RegMsg(&IPCToolPipe_Msg);
	g_pLogCallback->RegMsg(&IPCToolPipe_Msg_W);
	g_pLogCallback->RegWarn(&IPCToolPipe_Warn);
	g_pLogCallback->RegWarn(&IPCToolPipe_Warn_W);
	g_pLogCallback->RegDebug(&IPCToolPipe_Debug);
	g_pLogCallback->RegDebug(&IPCToolPipe_Debug_W);
}

#endif

void IPCToolMain::registerFunctions()
{
#ifndef DESURA_CLIENT
	REG_FUNCTION_VOID(IPCToolMain, setCrashSettings);
	REG_FUNCTION(IPCToolMain, installTool);

	LINK_EVENT(onErrorEvent, gcException);
	LINK_EVENT(onCompleteEvent, int32);
#else
	REG_EVENT(onErrorEvent);
	REG_EVENT(onCompleteEvent);

	REG_FUNCTION_VOID(IPCToolMain, warning);
	REG_FUNCTION_VOID(IPCToolMain, message);
	REG_FUNCTION_VOID(IPCToolMain, debug);
#endif
}


#ifdef DESURA_CLIENT

void IPCToolMain::warning(const char* msg)
{
	Warning(gcString("DesuraService: {0}", msg));
}

void IPCToolMain::message(const char* msg)
{
	Msg(gcString("DesuraService: {0}", msg));
}

void IPCToolMain::debug(const char* msg)
{
	Debug(gcString("DesuraService: {0}", msg));
}	

gcException IPCToolMain::installTool(UserCore::ToolInfo* info)
{
	return installTool(info->getExe(), info->getArgs());
}

gcException IPCToolMain::installTool(const char* exe, const char* args)
{
	try
	{
		return IPC::functionCall<gcException>(this, "installTool", exe, args);
	}
	catch (gcException &e)
	{
		return e;
	}
}

void IPCToolMain::setCrashSettings(const char* user, bool upload)
{
	IPC::functionCallAsync(this, "setCrashSettings", user, upload);
}

#else

void IPCToolMain::warning(const char* msg)
{
	IPC::functionCallAsync(this, "warning", msg);
}

void IPCToolMain::message(const char* msg)
{
	IPC::functionCallAsync(this, "message", msg);
}

void IPCToolMain::debug(const char* msg)
{
	IPC::functionCallAsync(this, "debug", msg);
}

gcException IPCToolMain::installTool(const char* exe, const char* args)
{
	if (!m_pThread)
	{
		m_pThread = new ToolInstallThread();
		m_pThread->start();

		m_pThread->onCompleteEvent += delegate(&onCompleteEvent);
		m_pThread->onErrorEvent += delegate(&onErrorEvent);
	}

	return m_pThread->installTool(exe, args);
}

//void SetCrashSettings(const wchar_t* user, bool upload);
void IPCToolMain::setCrashSettings(const char* user, bool upload)
{
	//gcWString wUser(user);
	//SetCrashSettings(wUser.c_str(), upload);
}

#endif