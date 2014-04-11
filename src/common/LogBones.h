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

class LogCallback;
class Color;

enum MSG_TYPE
{
	MT_MSG,
	MT_WARN,
	MT_DEBUG,
	MT_TRACE,
};

void LogMsg(MSG_TYPE type, std::string msg, Color *col = nullptr, std::map<std::string, std::string> *pmArgs = nullptr);


template <typename T>
std::string TraceClassInfo(T *pClass)
{
	return "";
}

template<typename CT>
void PrintToStream(const DesuraId& t, std::basic_stringstream<CT> &oss)
{
	oss << t.toInt64();
}



#ifdef WITH_TRACING

#ifdef WIN32
#ifndef _delayimp_h
extern "C" IMAGE_DOS_HEADER __ImageBase;
#endif
#endif

template <typename ... Args>
void TraceS(const char* szFunction, const char* szClassInfo, const char* szFormat, Args ... args)
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

	mArgs["function"] = gcString(szFunction);
	mArgs["classinfo"] = gcString(szClassInfo);
	mArgs["thread"] = gcString("{0}", getCurrentThreadId());
	mArgs["time"] = gcTime().to_js_string();

#ifdef WIN32
	char szModuleName[255];
	GetModuleFileNameA(reinterpret_cast<HMODULE>(&__ImageBase), szModuleName, 255);

	auto t = std::string(szModuleName, 255);
	mArgs["module"] = t.substr(t.find_last_of('\\') + 1);
#endif

	LogMsg(MT_TRACE, gcString(szFormat, args...), nullptr, &mArgs);
}

template <typename T, typename ... Args>
void TraceT(const char* szFunction, T *pClass, const char* szFormat, Args ... args)
{
	auto ci = TraceClassInfo(pClass);
	TraceS(szFunction, ci.c_str(), szFormat, args...);
}

template <typename T, typename ... Args>
void WarningT(const char* szFunction, T *pClass, const char* szFormat, Args ... args)
{
	gcString msg(szFormat, args...);
	LogMsg(MT_WARN, msg);

	msg = "Warning: " + msg;

	auto ci = TraceClassInfo(pClass);
	TraceS(szFunction, ci.c_str(), msg.c_str());
}

namespace
{
	class FakeTracerClass
	{
	};
}

#define Msg( ... ) LogMsg(MT_MSG, gcString(__VA_ARGS__))
#define Debug( ... ) LogMsg(MT_DEBUG, gcString(__VA_ARGS__))
#define Warning( ... ) WarningT(__FUNCTION__, this, __VA_ARGS__)
#define WarningS( ... ) WarningT(__FUNCTION__, (FakeTracerClass*)nullptr, __VA_ARGS__)
#define gcTrace( ... ) TraceT(__FUNCTION__, this, __VA_ARGS__)
#define gcTraceS( ... ) TraceT(__FUNCTION__, (FakeTracerClass*)nullptr, __VA_ARGS__)


#else

template <typename ... Args>
void TraceS(const char* szFunction, const char* szClassInfo, const char* szFormat, Args ... args)
{
}

template <typename T, typename ... Args>
void TraceT(const char* szFunction, T *pClass, const char* szFormat, Args ... args)
{
}

#define Msg( ... ) LogMsg(MT_MSG, gcString(__VA_ARGS__))
#define Debug( ... ) LogMsg(MT_DEBUG, gcString(__VA_ARGS__))
#define Warning( ... ) LogMsg(MT_WARN, gcString(__VA_ARGS__))
#define WarningS( ... ) LogMsg(MT_WARN, gcString(__VA_ARGS__))
#define gcTrace( ... ) {}
#define gcTraceS( ... ) {}

#endif



#ifdef WIN32
#define printf DesuraPrintFRedirect
void DesuraPrintFRedirect(const char* format, ...);
#endif

class TracerI
{
public:
	virtual void trace(const std::string &strTrace, std::map<std::string, std::string> *mpArgs) = 0;

protected:
	virtual ~TracerI(){}
};


#endif
