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
void safe_delete(std::shared_ptr<T> &pShared)
{
	pShared.reset();
}


template <typename T>
class AutoDelete
{
public:
	AutoDelete()
	{
		m_pPointer = nullptr;
	}

	AutoDelete(T* t)
	{
		m_pPointer = t;
	}

	~AutoDelete()
	{
		safe_delete(m_pPointer);
	}

	T* handle()
	{
		return m_pPointer;
	}

	AutoDelete<T>& operator=(AutoDelete<T>& o)
	{
		if (this != &o)
		{
			safe_delete(m_pPointer);
			m_pPointer = o.m_pPointer;
			o.m_pPointer = nullptr;
		}

		return *this;
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
	T* m_pPointer;
};

#ifdef WIN32
template <typename T>
class AutoDeleteLocal : public AutoDelete<T>
{
public:
	AutoDeleteLocal(T* t) : AutoDelete<T>(t)
	{
	}

	~AutoDeleteLocal()
	{
		LocalFree(*AutoDelete<T>::m_pPointer);
		AutoDelete<T>::m_pPointer = nullptr;
	}

	AutoDeleteLocal<T>& operator=(AutoDeleteLocal<T>& o)
	{
		if (this != &o)
		{
			LocalFree(*AutoDelete<T>::m_pPointer);
			AutoDelete<T>::m_pPointer = o.m_pPointer;
			o.m_pPointer = nullptr;
		}

		return *this;
	}
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