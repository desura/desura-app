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
