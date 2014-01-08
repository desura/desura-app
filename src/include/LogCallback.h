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

#ifndef DESURA_LOG_CALLBACK_H
#define DESURA_LOG_CALLBACK_H
#ifdef _WIN32
#pragma once
#endif

class Color;

typedef void (* MsgCallBackFn)(const char*, Color*);
typedef void (* MsgCallBackWFn)(const wchar_t*, Color*);

typedef void (* SpecialCallBackFn)(const char*);
typedef void (* SpecialCallBackWFn)(const wchar_t*);



class LogCallback
{
public:
	LogCallback()
	{
		m_cbMsg = nullptr;
		m_cbMsgW = nullptr;
		m_cbWarn = nullptr;
		m_cbWarnW = nullptr;
		m_cbDebug = nullptr;
		m_cbDebugW = nullptr;
	}

	void Msg(const char* msg, Color* col = nullptr)
	{
		if (m_cbMsg)
			m_cbMsg(msg, col);
	}

	void Msg_W(const wchar_t* msg, Color* col = nullptr)
	{
		if (m_cbMsgW)
			m_cbMsgW(msg, col);
	}

	void Warn(const char* msg)
	{
		if (m_cbWarn)
			m_cbWarn(msg);
	}

	void Warn_W(const wchar_t* msg)
	{
		if (m_cbWarnW)
			m_cbWarnW(msg);
	}

	void Debug(const char* msg)
	{
		if (m_cbDebug)
			m_cbDebug(msg);
	}

	void Debug_W(const wchar_t* msg)
	{
		if (m_cbDebugW)
			m_cbDebugW(msg);
	}

	void RegMsg(MsgCallBackFn cb)
	{
		m_cbMsg = cb;
	}

	void RegMsg(MsgCallBackWFn cb)
	{
		m_cbMsgW = cb;
	}

	void RegWarn(SpecialCallBackFn cb)
	{
		m_cbWarn = cb;
	}

	void RegWarn(SpecialCallBackWFn cb)
	{
		m_cbWarnW = cb;
	}

	void RegDebug(SpecialCallBackFn cb)
	{
		m_cbDebug = cb;
	}	

	void RegDebug(SpecialCallBackWFn cb)
	{
		m_cbDebugW = cb;
	}

private:
	MsgCallBackFn		m_cbMsg;
	MsgCallBackWFn		m_cbMsgW;
	
	SpecialCallBackFn	m_cbWarn;
	SpecialCallBackWFn	m_cbWarnW;

	SpecialCallBackFn	m_cbDebug;
	SpecialCallBackWFn	m_cbDebugW;
};

#endif //DESURA_LOG_CALLBACK_H
