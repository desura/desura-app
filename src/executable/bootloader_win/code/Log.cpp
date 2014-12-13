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

#include "StdAfx.h"

#include <map>
#ifdef __MINGW32__
#include <string>
#else
#include <xstring>
#endif

#include "Tracer.h"

void LogMsg(MSG_TYPE type, std::string msg, Color* col, std::map<std::string, std::string> *mpArgs);
#include "DesuraPrintFRedirect.h"


void LogMsg(MSG_TYPE type, std::string msg, Color* col, std::map<std::string, std::string> *mpArgs)
{
	if (type == MT_TRACE)
		g_Tracer.trace(msg, mpArgs);
	else
		fprintf(stdout, "%s", msg.c_str());
}
