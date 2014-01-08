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

#ifndef DESURA_DLLVERSION_H
#define DESURA_DLLVERSION_H
#ifdef _WIN32
#pragma once
#endif

namespace WebCore
{
namespace Misc
{

//! Class holds the version numbers of all the modules
class DLLVersion
{
public:
	//! Default constuctor
	//!
	DLLVersion()
	{
	}

	//! Constuctor
	//!
	//! @param d Desura Exe Version
	//! @param ui UICore Version
	//! @param w WebCore Version
	//! @param m MCFCore Version
	//! @param u UserCore Version
	//!
	DLLVersion(const char* d, const char* ui, const char* w, const char* m, const char* u)
	{
		szMcfVer = gcString(m);
		szDEVer = gcString(d);
		szUIVer = gcString(ui);
		szWebVer = gcString(w);
		szUserVer = gcString(u);
	}

	//! Copy Constuctor
	//!
	//! @param dv object to copy from
	//!
	DLLVersion(DLLVersion* dv)
	{
		if (dv)
		{
			szMcfVer = dv->szMcfVer;
			szDEVer = dv->szDEVer;
			szUIVer = dv->szUIVer;
			szWebVer = dv->szWebVer;
			szUserVer = dv->szUserVer;
		}
	}

	gcString szMcfVer;
	gcString szDEVer;
	gcString szUIVer;
	gcString szWebVer;
	gcString szUserVer;
};

}
}

#endif //DESURA_DLLVERSION_H
