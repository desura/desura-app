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

#include "Common.h"
#include "IPCToolMain.h"
#include "IPCManager.h"
#include "Color.h"

#ifndef DESURA_CLIENT
#include "ToolInstallThread.h"
#include "LogCallback.h"
#else
#include "ToolInfo.h"
#endif

REG_IPC_CLASS( IPCToolMain );
extern LogCallback* g_pLogCallback;

static IPCToolMain *g_pToolMain;

IPCToolMain::IPCToolMain(IPC::IPCManager* mang, uint32 id, DesuraId itemId)
	: IPC::IPCClass(mang, id, itemId)
{
	gcAssert(!g_pToolMain);

	registerFunctions();
	g_pToolMain = this;
}

IPCToolMain::~IPCToolMain()
{
	g_pToolMain = nullptr;

#ifndef DESURA_CLIENT
	safe_delete(m_pThread);
	g_pLogCallback->Reset();
#endif
}

#ifndef DESURA_CLIENT
void IPCToolMain::SetupLogging()
{
	LogCallback::MessageFn messageFn = [](MSG_TYPE type, const char* msg, Color* col, std::map<std::string, std::string> *mpArgs)
	{
#ifndef DEBUG
		if (type == MT_DEBUG)
			return;
#endif

		uint64 nCol = -1;

		if (col)
			nCol = col->getColor();

		OutputDebugStringA(msg);
		OutputDebugStringA("\n");

		if (g_pToolMain)
			g_pToolMain->message((int)type, msg, nCol, mpArgs);
	};

	safe_delete(g_pLogCallback);
	g_pLogCallback = new LogCallback();
	g_pLogCallback->RegMsg(messageFn);
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
	REG_FUNCTION_VOID(IPCToolMain, message);
#endif
}


#ifdef DESURA_CLIENT
void IPCToolMain::message(int type, const char* msg, uint64 col, std::map<std::string, std::string> *mpArgs)
{
	Color color;
	Color *pCol = nullptr;

	if (col != -1)
	{
		color = Color((uint32)col);
		pCol = &color;
	}

	if (type == MT_TRACE)
	{
		if (mpArgs)
			(*mpArgs)["app"] = "Tool";

		LogMsg((MSG_TYPE)type, gcString(msg), pCol, mpArgs);
	}
	else
	{
		LogMsg((MSG_TYPE)type, gcString("Tool: {0}", msg), pCol, mpArgs);
	}
}

gcException IPCToolMain::installTool(gcRefPtr<UserCore::ToolInfo> info)
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

void IPCToolMain::message(int type, const char* msg, uint64 col, std::map<std::string, std::string> *mpArgs)
{
	IPC::functionCallAsync(this, "message", type, msg, col, mpArgs);
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
