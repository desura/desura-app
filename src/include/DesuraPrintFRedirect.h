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

#ifdef WIN32

#include <stdio.h>
#include <stdarg.h>


void DesuraPrintFRedirect(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vprintf(format, args);

	std::string strOut("\0");
	strOut.reserve(1024);

	vsnprintf(const_cast<char*>(strOut.c_str()), 1024, format, args);

	LogMsg((MSG_TYPE)0, strOut, nullptr, nullptr);

#ifdef DEBUG
	OutputDebugStringA(strOut.c_str());
#endif

	va_end(args);
}

#endif