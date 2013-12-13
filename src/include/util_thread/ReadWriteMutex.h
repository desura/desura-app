/*
Desura is the leading indie game distribution platform
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#ifndef DESURA_READWRITE_MUTEX_H
#define DESURA_READWRITE_MUTEX_H
#ifdef _WIN32
#pragma once
#endif

#include <mutex>
#include <condition_variable>

namespace Thread
{
namespace Internal
{

class ReadWriteMutex
{
public:
    ReadWriteMutex() :
        m_readers(0),
        m_pendingWriters(0),
        m_currentWriter(false)
    {
	}

	ReadWriteMutex(const ReadWriteMutex&) = delete;

    // local class has access to ReadWriteMutex private members, as required
    class ScopedReadLock
    {
    public:
        ScopedReadLock(ReadWriteMutex& rwLock) : m_rwLock(rwLock)
        {
            m_rwLock.acquireReadLock();
        }

		ScopedReadLock(const ScopedReadLock&) = delete;

        ~ScopedReadLock()
        {
            m_rwLock.releaseReadLock();
        }

    private:
        ReadWriteMutex& m_rwLock;
    };

    class ScopedWriteLock
    {
    public:
        ScopedWriteLock(ReadWriteMutex& rwLock) :
            m_rwLock(rwLock)
        {
            m_rwLock.acquireWriteLock();
        }

		ScopedWriteLock(const ScopedWriteLock&) = delete;

        ~ScopedWriteLock()
        {
            m_rwLock.releaseWriteLock();
        }

    private:
        ReadWriteMutex& m_rwLock;
    };


private: // data
    std::mutex m_mutex;

    unsigned int m_readers;
    std::condition_variable m_noReaders;

    unsigned int m_pendingWriters;
    bool m_currentWriter;
    std::condition_variable m_writerFinished;


public: // internal locking functions
    void acquireReadLock()
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        // require a while loop here, since when the writerFinished condition is notified
        // we should not allow readers to lock if there is a writer waiting
        // if there is a writer waiting, we continue waiting
        while(m_pendingWriters != 0 || m_currentWriter)
        {
            m_writerFinished.wait(lock);
        }
        ++m_readers;
    }

    void releaseReadLock()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        --m_readers;

        if(m_readers == 0)
        {
            // must notify_all here, since if there are multiple waiting writers
            // they should all be woken (they continue to acquire the lock exclusively though)
            m_noReaders.notify_all();
        }
    }

    // this function is currently not exception-safe:
    // if the wait calls throw, m_pendingWriter can be left in an inconsistent state
    void acquireWriteLock()
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        // ensure subsequent readers block
        ++m_pendingWriters;

        // ensure all reader locks are released
        while(m_readers > 0)
        {
            m_noReaders.wait(lock);
        }

        // only continue when the current writer has finished
        // and another writer has not been woken first
        while(m_currentWriter)
        {
            m_writerFinished.wait(lock);
        }
        --m_pendingWriters;
        m_currentWriter = true;
    }

    void releaseWriteLock()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_currentWriter = false;
        m_writerFinished.notify_all();
    }
};

}
}


#endif