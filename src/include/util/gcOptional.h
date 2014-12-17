/*
Copyright (C) 2014 Mark Chandler (LindenLab)
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

#ifndef DESURA_GCOPTIONAL_H
#define DESURA_GCOPTIONAL_H

//todo: replace with std::optional in c++14
template <typename T>
class gcOptional
{
public:
	gcOptional()
	{
	}

	gcOptional(const gcOptional<T> &t)
		: m_bIsSet(t.m_bIsSet)
		, m_tVal(t.m_tVal)
	{
	}

	gcOptional(gcOptional<T> &&t)
		: m_bIsSet(t.m_bIsSet)
		, m_tVal(std::move(t.m_tVal))
	{
	}

	gcOptional(const T &t)
		: m_bIsSet(true)
		, m_tVal(t)
	{
	}

	gcOptional(T &&t)
		: m_bIsSet(true)
		, m_tVal(std::move(t))
	{
	}

	T& operator*()
	{
		return value();
	}

	const T& operator*() const
	{
		return value();
	}

	T* operator->()
	{
		return &value();
	}

	const T* operator->() const
	{
		return &value();
	}

	operator bool() const
	{
		return m_bIsSet;
	}

	T& value()
	{
		gcAssert(m_bIsSet);
		return m_tVal;
	}

	const T& value() const
	{
		gcAssert(m_bIsSet);
		return m_tVal;
	}

	gcOptional<T>& operator=(const gcOptional<T>& t)
	{
		if (this == &t)
			return *this;

		m_bIsSet = t.m_bIsSet;
		m_tVal = t.m_tVal;

		return *this;
	}

	gcOptional<T>& operator=(gcOptional<T>&& t)
	{
		if (this == &t)
			return *this;

		m_bIsSet = t.m_bIsSet;
		m_tVal = std::move(t.m_tVal);

		return *this;
	}

private:
	bool m_bIsSet = false;
	T m_tVal;
};

#endif
