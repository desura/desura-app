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

#ifndef DESURA_LOG_BONES_H
#define DESURA_LOG_BONES_H
#ifdef _WIN32
#pragma once
#endif

#ifdef WIN32
#define printf PrintfMsg
#endif
void PrintfMsg(const char* format, ...);

class LogCallback;
class Color;

enum MSG_TYPE
{
	MT_MSG,
	MT_MSG_COL,
	MT_WARN,
	MT_DEBUG,
	MT_TRACE,
};

void LogMsg(MSG_TYPE type, std::string msg, Color *col = nullptr, std::map<std::string, std::string> *pmArgs = nullptr);

template<typename CT>
void Msg(const CT* message)
{
	LogMsg(MT_MSG, gcBaseString<CT>(message));
}

template<typename CT>
void Msg(gcBaseString<CT> message)
{
	LogMsg(MT_MSG, message);
}


template<typename CT>
void MsgCol(Color* col, const CT* message)
{
	LogMsg(MT_MSG_COL, gcBaseString<CT>(message), col);
}

template<typename CT>
void MsgCol(Color* col, gcBaseString<CT> message)
{
	LogMsg(MT_MSG_COL, message, col);
}

template<typename CT>
void Warning(const CT* message)
{
	LogMsg(MT_WARN, gcBaseString<CT>(message));
}

template<typename CT>
void Warning(gcBaseString<CT> message)
{
	gcString msg(message);
	LogMsg(MT_WARN, msg);
}

template<typename CT>
void Debug(const CT* message)
{
	LogMsg(MT_DEBUG, gcBaseString<CT>(message));
}

template<typename CT>
void Debug(gcBaseString<CT> message)
{
	LogMsg(MT_DEBUG, message);
}

template <typename T>
gcString TraceClassInfo(T *pClass)
{
	return "";
}

template <typename T, typename ... Args>
void TraceT(const char* szFunction, const char* szFile, int nLine, T *pClass, const char* szFormat, Args ... args)
{
	static auto getCurrentThreadId = []()
	{
#ifdef WIN32
		return ::GetCurrentThreadId();
#else
		return (uint64)pthread_self();
#endif
	};

	std::map<std::string, std::string> mArgs;

	mArgs["function"] = szFunction;
	mArgs["file"] = szFile;
	mArgs["line"] = gcString("{0}", nLine);
	mArgs["classinfo"] = TraceClassInfo(pClass);
	mArgs["thread"] = gcString("{0}", getCurrentThreadId());
	mArgs["time"] = gcTime().to_iso_string();

	LogMsg(MT_TRACE, gcString(szFormat, args...), nullptr, &mArgs);
}


template<typename CT>
void PrintToStream(const DesuraId& t, std::basic_stringstream<CT> &oss)
{
	oss << t.toInt64();
}

#define gcTrace( ... ) TraceT(__FUNCTION__, __FILE__, __LINE__, this, __VA_ARGS__);


#endif
