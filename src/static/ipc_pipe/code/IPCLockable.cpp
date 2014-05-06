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
#include "IPCLockable.h"

using namespace IPC;



IPCLock::IPCLock(uint32 i)
	: id(i)
{
}

IPCLock::~IPCLock()
{
	safe_delete(m_pResult);
}

void IPCLock::wait()
{
	if (!m_bTriggered)
		m_WaitCond.wait();
}

void IPCLock::timedWait()
{
	if (!m_bTriggered)
		m_WaitCond.wait(30); //wait on mutex
}

bool IPCLock::wait(int sec, int milli)
{
	if (!m_bTriggered)
		return m_WaitCond.wait(sec, milli); //wait on mutex

	return false;
}

void IPCLock::trigger(IPCParameterI* pParameter)
{
	std::lock_guard<std::mutex> guard(m_InternalLock);

	gcAssert(!m_pResult);
	m_pResult = pParameter;

	m_bTriggered = true;
	m_WaitCond.notify();
}

IPCParameterI* IPCLock::popResult()
{
	std::lock_guard<std::mutex> guard(m_InternalLock);

	gcAssert(m_pResult);
	auto t = m_pResult;
	m_pResult = nullptr;
	return t;
}







IPCLockable::IPCLockable()
{
}

IPCLockable::~IPCLockable()
{
	std::lock_guard<std::mutex> guard(m_lockMutex);
	gcAssert(m_vLockList.empty());
}

void IPCLockable::cancelLocks(gcException &reason)
{
	std::lock_guard<std::mutex> guard(m_lockMutex);

	for (auto l : m_vLockList)
		l->trigger(newParameterS(reason));
}

std::shared_ptr<IPCLock> IPCLockable::newLock()
{
	auto lock = std::make_shared<IPCLock>(++m_uiIdCount);

	std::lock_guard<std::mutex> guard(m_lockMutex);
	m_vLockList.push_back(lock);

	return lock;
}

void IPCLockable::delLock(uint32 id)
{
	std::lock_guard<std::mutex> guard(m_lockMutex);

	auto it = std::find_if(begin(m_vLockList), end(m_vLockList), [id](std::shared_ptr<IPCLock> &lock){
		return lock->id == id;
	});

	if (it == end(m_vLockList))
		return;

	m_vLockList.erase(it);
}

std::shared_ptr<IPCLock> IPCLockable::findLock(uint32 id)
{
	std::lock_guard<std::mutex> guard(m_lockMutex);

	auto it = std::find_if(begin(m_vLockList), end(m_vLockList), [id](std::shared_ptr<IPCLock> &lock){
		return lock->id == id;
	});

	if (it != end(m_vLockList))
		return *it;

	return nullptr;
}