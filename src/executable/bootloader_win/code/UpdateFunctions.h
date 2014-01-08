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

#ifndef DESURA_UPDATEFUNCTIONS_H
#define DESURA_UPDATEFUNCTIONS_H
#ifdef _WIN32
#pragma once
#endif

enum
{
	UPDATE_NONE,
	UPDATE_MCF,
	UPDATE_FILES,
	UPDATE_XML,
	UPDATE_SERVICE,
	UPDATE_CERT,
	UPDATE_FORCED,
	UPDATE_SERVICE_PATH,		//path is wrong but current service should launch
	UPDATE_SERVICE_LOCATION,	//path is worng and current service wont launch
	UPDATE_DATAPATH,			//not used
	UPDATE_SERVICE_HASH,
	UPDATE_SERVICE_DISABLED,
};

bool FileExists(const wchar_t* fileName);
int NeedUpdate();

void McfUpdate();
void FullUpdate();

bool CheckUpdate(const wchar_t* path);
bool CheckInstall();

bool MoveDataFolder();
bool ServiceUpdate(bool validService);
bool FixServiceDisabled();

#endif //DESURA_UPDATEFUNCTIONS_H
