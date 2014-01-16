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

#ifndef DESURA_GCTIME_H
#define DESURA_GCTIME_H
#ifdef _WIN32
#pragma once
#endif

#include <chrono>

class gcDuration
{
public:
	gcDuration()
	{
	}

	gcDuration(const std::chrono::system_clock::duration &duration)
		: m_Duration(duration)
	{
	}

	uint64 seconds() const
	{
		return std::chrono::duration_cast<std::chrono::seconds>(m_Duration).count();
	}

	uint64 minutes() const
	{
		return std::chrono::duration_cast<std::chrono::minutes>(m_Duration).count();
	}

	uint64 hours() const
	{
		return std::chrono::duration_cast<std::chrono::hours>(m_Duration).count();
	}

	gcDuration& operator-=(const gcDuration &duration)
	{
		m_Duration -= duration.m_Duration;
		return *this;
	}

	gcDuration& operator+=(const gcDuration &duration)
	{
		m_Duration += duration.m_Duration;
		return *this;
	}

	gcDuration& operator-=(const std::chrono::system_clock::duration &duration)
	{
		m_Duration -= duration;
		return *this;
	}

	gcDuration& operator+=(const std::chrono::system_clock::duration &duration)
	{
		m_Duration += duration;
		return *this;
	}


private:
	std::chrono::system_clock::duration m_Duration;
};

#define IOS_TIME_STR_FORMAT "%Y%m%dT%H%M%S"

class gcTime
{
public:
	gcTime()
		: m_TimePoint(std::chrono::system_clock::now())
	{
	}

	gcTime(const std::chrono::system_clock::time_point& time)
		: m_TimePoint(time)
	{
	}

	gcTime(const time_t &t)
		: m_TimePoint(std::chrono::system_clock::from_time_t(t))
	{
	}

	int hours() const
	{
		auto t = std::chrono::system_clock::to_time_t(m_TimePoint);
		tm source = *localtime(&t);
		return source.tm_hour;
	}

	int minutes() const
	{
		auto t = std::chrono::system_clock::to_time_t(m_TimePoint);
		tm source = *localtime(&t);
		return source.tm_min;
	}

	int seconds() const
	{
		auto t = std::chrono::system_clock::to_time_t(m_TimePoint);
		tm source = *localtime(&t);
		return source.tm_sec;
	}

	gcDuration operator-(const gcTime& t)
	{
		return gcDuration(m_TimePoint - t.m_TimePoint);
	}

	bool operator> (const gcTime& t) const
	{
		return m_TimePoint > t.m_TimePoint;
	}

	bool operator< (const gcTime& t) const
	{
		return m_TimePoint < t.m_TimePoint;
	}

	bool operator>= (const gcTime& t) const
	{
		return m_TimePoint >= t.m_TimePoint;
	}

	bool operator<= (const gcTime& t) const
	{
		return m_TimePoint <= t.m_TimePoint;
	}

	bool operator!= (const gcTime& t) const
	{
		return m_TimePoint != t.m_TimePoint;
	}

	bool operator== (const gcTime& t) const
	{
		return m_TimePoint == t.m_TimePoint;
	}

	gcTime& operator-=(const std::chrono::system_clock::duration &duration)
	{
		m_TimePoint -= duration;
		return *this;
	}

	gcTime& operator+=(const std::chrono::system_clock::duration &duration)
	{
		m_TimePoint += duration;
		return *this;
	}

	gcTime operator+(const std::chrono::system_clock::duration &duration)
	{
		return gcTime(m_TimePoint + duration);
	}

	std::string to_iso_string() const
	{
		auto t = std::chrono::system_clock::to_time_t(m_TimePoint);
		return to_iso_string(t);
	}

	time_t to_time_t() const
	{
		return std::chrono::system_clock::to_time_t(m_TimePoint);
	}

	static std::string to_iso_string(const time_t &t)
	{
#ifdef NIX
		assert(false);
		return "";
#else
		tm source = *localtime(&t);

		std::ostringstream stream;
		stream << std::put_time(&source, IOS_TIME_STR_FORMAT);
		return stream.str();
#endif
	}

	static gcTime from_iso_string(const std::string &str)
	{
#ifdef NIX
		assert(false);
		return gcTime();
#else
		tm source = {0};

		std::istringstream stream(str);
		stream >> std::get_time(&source, IOS_TIME_STR_FORMAT);

		return gcTime(std::chrono::system_clock::from_time_t(mktime(&source)));
#endif
	}

private:
	std::chrono::system_clock::time_point m_TimePoint;
};


#endif //DESURA_GCTIME_H
