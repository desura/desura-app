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

#include "Common.h"
#include "User.h"
#include "usercore/UserCoreI.h"
#include <branding/usercore_version.h>

gcString g_szUserCoreVersion("{0}.{1}.{2}.{3}", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILDNO, VERSION_EXTEND);


namespace UserCore
{

	const char* GetUserCoreVersion()
	{
		return g_szUserCoreVersion.c_str();
	}

extern "C"
{

	CEXPORT void* FactoryBuilderUC(const char* name)
	{
		gcString strName(name);

		if (strName == USERCORE)
		{
			UserCore::User* temp = new UserCore::User();
			return temp;
		}
		else if (strName  == USERCORE_VER)
		{
			return (void *)&GetUserCoreVersion;
		}
		else if (strName == USERCORE_GETLOGIN)
		{
			return (void *)&UserCore::User::getLoginInfo;
		}
		else if (strName == USERCORE_GETITEMSTATUS)
		{
			return (void*)&UserCore::Item::ItemHandle::getStatusStr_s;
		}

		return nullptr;
	}

}
}
