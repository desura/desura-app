/*
Desura is the leading indie game distribution platform
Copyright (C) 2014 Mark Chandler (LindenLab)

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