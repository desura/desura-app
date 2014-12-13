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

        std::atomic<bool> m_bTriggered = {false};
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
