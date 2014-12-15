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

#ifndef DESURA_BASEMANAGER_H
#define DESURA_BASEMANAGER_H
#ifdef _WIN32
#pragma once
#endif

//this disables this warning:
//warning C4275: non dll-interface class 'BaseManager' used as base for dll-interface class 'wildcardMng'
#pragma warning( disable : 4275)
#include <map>

class BaseItem : public gcRefBase
{
public:
	BaseItem()
	{
		m_uiHash = 0;
	}

	BaseItem(const char* name)
	{
		m_szName =  gcString(name);
		m_uiHash = UTIL::MISC::RSHash_CSTR(name);
	}

	virtual ~BaseItem(){}

	uint64 getHash(){return m_uiHash;}
	const char* getName(){return m_szName.c_str();}

protected:
	gcString m_szName;
	uint64 m_uiHash;

	gc_IMPLEMENT_REFCOUNTING(BaseItem)
};



template <class T>
class BaseManager
{
public:
	BaseManager<T>()
	{
	}

	virtual ~BaseManager<T>()
	{
		removeAll();
	}

#if defined WIN32 || MACOS || GCC_VERSION > 40500 || ( __clang_major__ > 2 && __clang_minor__ > 0 )
	template <typename F>
	void for_each(F &f)
	{
		for (const auto &p : m_mItemMap)
			f(p.second);
	}

	template <typename F>
	void for_each(const F &f)
	{
		for (const auto &p : m_mItemMap)
			f(p.second);
	}
#endif


	gcRefPtr<T> findItem(uint64 hash)
	{
		if (m_mItemMap.size() == 0)
			return nullptr;

		typename std::map<uint64, gcRefPtr<T>>::iterator it = m_mItemMap.find(hash);

		if (it == m_mItemMap.end())
			return nullptr;

		return it->second;
	}

	gcRefPtr<T> findItem(const char* name)
	{
		uint64 hash = UTIL::MISC::RSHash_CSTR(name);
		return findItem(hash);
	}

	gcRefPtr<T> getItem(uint32 index)
	{
		typename std::map<uint64, gcRefPtr<T>>::iterator it;
		typename std::map<uint64, gcRefPtr<T>>::iterator endit = m_mItemMap.end();

		uint32 count = 0;

		for (it = m_mItemMap.begin(); it != endit; ++it)
		{
			if (count == index)
				return it->second;

			count++;
		}

		return nullptr;
	}

	uint32 getCount(){return (uint32)m_mItemMap.size();}

	void removeAll()
	{
		m_mItemMap.clear();
	}

	std::vector<gcRefPtr<T>> dumpAndClear()
	{
		std::vector<gcRefPtr<T>> ret;

		for (auto p : m_mItemMap)
			ret.push_back(p.second);

		m_mItemMap.clear();
		return ret;
	}

protected:
	void addItem(const gcRefPtr<T> &pItem)
	{
		if (pItem)
			addItem(pItem->getHash(), pItem);
	}

	void addItem(uint64 index, const gcRefPtr<T> &pItem)
	{
		if (pItem)
			m_mItemMap.insert(std::pair<uint64, gcRefPtr<T>>(index, pItem));
	}

	void removeItem(const char* name)
	{
		uint64 hash = UTIL::MISC::RSHash_CSTR(name);
		removeItem(hash);
	}

	void removeItem(uint64 hash)
	{
		if (m_mItemMap.size() == 0)
			return;

		typename std::map<uint64, gcRefPtr<T>>::iterator it = m_mItemMap.find(hash);

		if (it == m_mItemMap.end())
			return;

		m_mItemMap.erase(it);
	}

	std::map<uint64, gcRefPtr<T>> m_mItemMap;
};


#endif //DESURA_BASEMANAGER_H
