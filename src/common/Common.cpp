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

//	This compiles the precompile header common.h
//
//
//

#include "Common.h"
#include <thread>

bool CheckVerify(bool bCon, const char* szFunction, const char* szCheck)
{
	if (bCon)
		return true;

	WarningS("VERIFY HIT IN {0}: {1}", szFunction, szCheck);
	PAUSE_DEBUGGER();

	return false;
}


#if defined(DEBUG) && defined(WIN32)
void gcRefCount::addStackTrace(void* pObj)
{
	std::lock_guard<std::mutex> guard(m_Lock);

	auto it = m_mStackTraces.find(pObj);
	gcAssert(it == end(m_mStackTraces));
	m_mStackTraces[pObj] = UTIL::OS::getStackTrace(2, 10);
}

void gcRefCount::delStackTrace(void* pObj)
{
	std::lock_guard<std::mutex> guard(m_Lock);

	auto it = m_mStackTraces.find(pObj);
	gcAssert(it != end(m_mStackTraces));
	m_mStackTraces.erase(it);
}

void gcRefCount::dumpStackTraces()
{
	std::lock_guard<std::mutex> guard(m_Lock);

	Debug("Dumping stacktraces for {0}\n", (size_t)this);

	for (auto p : m_mStackTraces)
	{
		auto trace = UTIL::OS::getStackTraceString(p.second);
		Debug(" -- 0x{0:x} [TID: {2}]: {1}\n", (size_t)p.first, trace, p.second->m_ThreadId);
	}

}
#endif

static std::thread::id g_UIThreadId;

void SetUIThreadId()
{
	g_UIThreadId = std::this_thread::get_id();
}

bool IsUIThread()
{
	return std::this_thread::get_id() == g_UIThreadId;
}

