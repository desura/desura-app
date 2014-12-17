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
#include "Log.h"
#include "LogCallback.h"

class Color;

LogCallback* g_pLogCallBack = nullptr;

std::mutex g_RegDllLock;
std::vector<RegDLLCB_MCF> g_pRegDlls;


void InitLogging(RegDLLCB_MCF cb)
{
	if (g_pLogCallBack)
	{
		if (cb)
		{
			std::lock_guard<std::mutex> guard(g_RegDllLock);
			cb(g_pLogCallBack);
			g_pRegDlls.push_back(cb);
		}

		return;
	}


	LogCallback::MessageFn messageFn = [](MSG_TYPE type, const char* msg, Color* col, std::map<std::string, std::string> *mpArgs)
	{
		LogMsg(type, msg, col, mpArgs);
	};

	g_pLogCallBack = new LogCallback();
	g_pLogCallBack->RegMsg(messageFn);

	if (cb)
	{
		g_pRegDlls.push_back(cb);
		cb(g_pLogCallBack);
	}

	gcTraceS("");
}

void DestroyLogging()
{
	gcTraceS("");

	{
		std::lock_guard<std::mutex> guard(g_RegDllLock);

		for (auto cb : g_pRegDlls)
			cb(nullptr);
	}

	safe_delete(g_pLogCallBack);
}

#include "DesuraPrintFRedirect.h"
