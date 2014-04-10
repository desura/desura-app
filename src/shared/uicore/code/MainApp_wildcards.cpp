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
#include "MainApp.h"

#include "SteamUser.h"
#include "managers/WildcardManager.h"
#include "managers/CVar.h"

CVar gc_steamuser("gc_steamuser", "", CFLAG_USER|CVAR_WINDOWS_ONLY);


void MainApp::processWildCards(WCSpecialInfo &info, wxWindow* parent)
{
	if (info.handled)
		return;

	if (Safe::stricmp("STEAMUSER", info.name.c_str()) == 0)
		getSteamUser(&info, parent);
}

void MainApp::getSteamUser(WCSpecialInfo *info, wxWindow *parent)
{
	if (gc_steamuser.getString() && strcmp(gc_steamuser.getString(), "") != 0)
	{
		info->result = gc_steamuser.getString();
		info->handled = true;
	}
	else
	{
		SteamUserDialog dlg(parent?parent:this);
		dlg.ShowModal();

		const char *steamuser = dlg.getSteamUser();

		if (steamuser)
		{
			gc_steamuser.setValue(steamuser);
			info->result = steamuser;
			info->handled = true;
		}
	}
}
