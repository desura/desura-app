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
