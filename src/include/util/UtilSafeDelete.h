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

#ifndef DESURA_UTILSAFEDELETE_H
#define DESURA_UTILSAFEDELETE_H


#include <memory>


template <typename T>
void safe_deleteA(T*& ptr)
{
	if (ptr)
		delete[] ptr;
	ptr = nullptr;
}

inline void safe_delete(char*& ptr)
{
	safe_deleteA(ptr);
}

inline void safe_delete(wchar_t*& ptr)
{
	safe_deleteA(ptr);
}

inline void safe_delete(unsigned char*& ptr)
{
	safe_deleteA(ptr);
}

inline void safe_delete(const char*& ptr)
{
	safe_deleteA(ptr);
}

inline void safe_delete(const wchar_t*& ptr)
{
	safe_deleteA(ptr);
}

inline void safe_delete(const unsigned char*& ptr)
{
	safe_deleteA(ptr);
}

template <typename T>
void safe_delete(T*& ptr)
{
	if (ptr)
		delete ptr;
	ptr = nullptr;
}

template <typename T>
void safe_delete(std::shared_ptr<T> &pShared)
{
    pShared.reset();
}

template <typename T>
void safe_delete(std::vector<T>& vec)
{
	for (size_t x = 0; x < vec.size(); x++)
		safe_delete(vec[x]);

	vec.clear();
}

template <typename T, typename A>
void safe_delete(const std::vector<T, A>& vec)
{
	for (auto p : vec)
		safe_delete(p);
}

template <typename T, typename A>
void safe_delete(std::vector<T, A>& vec)
{
	for (size_t x = 0; x < vec.size(); x++)
		safe_delete(vec[x]);

	vec.clear();
}

template <typename T>
void safe_delete(std::deque<T>& deq)
{
	for (size_t x = 0; x < deq.size(); x++)
		safe_delete(deq[x]);

	deq.clear();
}

template <typename T, typename S>
void safe_delete(std::map<T, S*>& map)
{
	for (auto i : map)
		safe_delete(i.second);

	map.clear();
}

template <typename T>
void safe_delete(size_t argc, T **argv)
{
	if (!argv)
		return;

	for (size_t n = 0; n < argc; n++)
		free(argv[n]);

	delete[] argv;
}

template <typename T>
void safe_delete(gcRefPtr<T> &pShared)
{
#if defined(DEBUG) && defined(WIN32)
	if (pShared.getRefCt() > 1)
		pShared.get()->dumpStackTraces();
#endif

	gcAssert(pShared.getRefCt() <= 1);
	pShared = gcRefPtr<T>();
}


template <typename T>
class AutoDelete
{
public:
	AutoDelete(T* &t)
		: m_pPointer(t)
	{
	}

	~AutoDelete()
	{
		safe_delete(m_pPointer);
	}

	T* handle()
	{
		return m_pPointer;
	}

	T* operator->()
	{
		return m_pPointer;
	}

	operator T*()
	{
		return m_pPointer;
	}

protected:
	T* &m_pPointer;
};

#ifdef WIN32
template <typename T>
class AutoDeleteLocal
{
public:
	AutoDeleteLocal(T t)
		: m_pPointer(t)
	{
	}

	~AutoDeleteLocal()
	{
		LocalFree(m_pPointer);
	}

protected:
	T m_pPointer;
};
#endif

template <typename T>
class AutoDeleteV
{
public:
	AutoDeleteV(T& t) : m_pPointer(t)
	{
	}

	~AutoDeleteV()
	{
		safe_delete(m_pPointer);
	}

private:
	T& m_pPointer;
};

#endif
