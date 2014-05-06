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

#ifndef DESURA_IPCLOCKABLE_H
#define DESURA_IPCLOCKABLE_H
#ifdef _WIN32
#pragma once
#endif

#include "util_thread/BaseThread.h"
#include "IPCParameter.h"

#include <atomic>

namespace IPC
{

	//! Represents a basic lock for function calls across IPC
	//!
	class IPCLock
	{
	public:
		IPCLock(uint32 i);
		~IPCLock();

		void wait();
		bool wait(int sec, int milli);

		void timedWait();
		
		void trigger(IPCParameterI* pParameter);
		IPCParameterI* popResult();

		const uint32 id;

	private:
		IPCParameterI* m_pResult = nullptr;

		std::atomic<bool> m_bTriggered;
		Thread::WaitCondition m_WaitCond;
		std::mutex m_InternalLock;
	};


	template <typename T>
	class IPCScopedLock
	{
	public:
		IPCScopedLock(T* pParent, std::shared_ptr<IPCLock> pLock)
			: m_pLock(pLock)
			, m_pParent(pParent)
		{
		}

		~IPCScopedLock()
		{
			if (m_pParent && m_pLock)
			{
				//avoid assert
				auto id = m_pLock->id;
				m_pLock.reset();
				m_pParent->delLock(id);
			}
		}

		std::shared_ptr<IPCLock> operator ->()
		{
			return m_pLock;
		}

	private:
		std::shared_ptr<IPCLock> m_pLock;
		T* m_pParent;
	};

	//! Class holds locks for function calls
	//!
	class IPCLockable
	{
	public:
		IPCLockable();
		virtual ~IPCLockable();

		void cancelLocks(gcException &reason);

	protected:
		void delLock(uint32 id);
		std::shared_ptr<IPCLock> newLock();
		std::shared_ptr<IPCLock> findLock(uint32 id);

	private:
		template <class T> friend class IPCScopedLock;

		std::atomic<uint32> m_uiIdCount;
		std::vector<std::shared_ptr<IPCLock>> m_vLockList;
		std::mutex m_lockMutex;
	};
}

#endif //DESURA_IPCLOCKABLE_H
