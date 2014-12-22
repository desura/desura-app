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
#ifndef DLB_UPDATE_H
#define DLB_UPDATE_H

enum
{
	UPDATE_NONE,
	UPDATE_MCF,
	UPDATE_FILES,
	UPDATE_XML,
	UPDATE_SERVICE,
	UPDATE_FORCED,
	UPDATE_SERVICE_PATH,		//path is wrong but current service should launch
	UPDATE_SERVICE_LOCATION,	//path is worng and current service wont launch
	UPDATE_DATAPATH,			//not used
};

bool CheckForUpdates();
int NeedUpdate();

bool CheckUpdate(const char* path);
bool CheckInstall();

int FullUpdate();
int McfUpdate();

int DownloadAndInstallMCF();
int DownloadFilesForTest();
int InstallFilesForTest();

#endif
