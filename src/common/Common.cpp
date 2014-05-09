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

//	This compiles the precompile header common.h
//
//
//

#include "Common.h"

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

	m_mStackTraces[pObj] = UTIL::OS::getStackTraceString(2, 10);
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
		Debug(" -- 0x{0:x}: {1}\n", (size_t)p.first, p.second);
}
#endif