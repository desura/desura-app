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
