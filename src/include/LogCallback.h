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

#ifndef DESURA_LOG_CALLBACK_H
#define DESURA_LOG_CALLBACK_H
#ifdef _WIN32
#pragma once
#endif

#include <functional>
#include "LogBones.h"

class Color;


class LogCallback
{
public:
	typedef std::function<void(MSG_TYPE, const char*, Color*, std::map<std::string, std::string>*)> MessageFn;

	void Message(MSG_TYPE type, const char* msg, Color* col = nullptr, std::map<std::string, std::string>* mpArgs = nullptr)
	{
		if (m_cbMsg)
			m_cbMsg(type, msg, col, mpArgs);
	}

	void RegMsg(MessageFn &cb)
	{
		m_cbMsg = cb;
	}

	void Reset()
	{
		m_cbMsg = MessageFn();
	}

private:
	MessageFn m_cbMsg;
};

#endif //DESURA_LOG_CALLBACK_H
