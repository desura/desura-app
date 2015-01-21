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

#ifndef DESURA_UTIL_CALLBACK_H
#define DESURA_UTIL_CALLBACK_H
#ifdef _WIN32
#pragma once
#endif

namespace UTIL
{
namespace CB
{
	class CallbackI
	{
	public:
		//! Return false to stop
		//!
		virtual bool onData(const unsigned char* buff, uint32 size)=0;
		virtual ~CallbackI() {}
	};


	template <typename T>
	class TemplateCallback : public UTIL::CB::CallbackI
	{
	public:
		TemplateCallback(T &t) : m_T(t)
		{
		}

		virtual bool onData(const unsigned char* buff, uint32 size)
		{
			return m_T(buff, size);
		}

	private:
		T &m_T;
	};


}
}

#endif //DESURA_UTIL_CALLBACK_H
