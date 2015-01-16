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


#include "UtilFile.h"

std::string GetAppPath(std::string extra)
{
	ERROR_OUTPUT(__func__);
	char result[PATH_MAX] = {0};
	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);

	if (count <= 0)
	{
		ERROR_OUTPUT("Could not read /proc/self/exe!");
		return "";
	}

	std::string exePath(result);

	// Cut off the filename.
	for(size_t i = count - 1; i > 0; i--)
	{
		if(result[i] == '/')
		{
			result[i] = '\0';
			break;
		}
	}

	return result;
}

std::string GetAppDataPath(std::string extra)
{
	ERROR_OUTPUT(__func__);

	std::string appDataPath = GetAppPath();
	appDataPath += STR_APPDATA;

	if (extra.size() > 0)
	{
		appDataPath += "/";
		appDataPath += extra;
	}

	return appDataPath;
}

bool ChangeToAppDir()
{
	ERROR_OUTPUT(__func__);
	std::string appPath = GetAppPath();
	ERROR_OUTPUT(appPath.c_str());
	if(chdir(appPath.c_str()) != 0)
	{
		ERROR_OUTPUT("Failed to change working directory!");
		return false;
	}

	return true;
}

bool FileExists(const char* file)
{
	char buffer[PATH_MAX];
	snprintf(buffer, PATH_MAX, "%s (%s)", __func__, file);
	ERROR_OUTPUT(buffer);

	if (!file)
		return false;

	struct stat stFileInfo;
	int intStat;

	// Attempt to get the file attributes
	intStat = stat(file, &stFileInfo);

	if (intStat == 0)
		return true;
	else
		return false;
}

bool DeleteFile(const char* file)
{
	char buffer[PATH_MAX];
	snprintf(buffer, PATH_MAX, "%s (%s)", __func__, file);
	ERROR_OUTPUT(buffer);

	if (!file)
		return false;

	return (remove(file) == 0);
}

bool DeleteFile(const wchar_t* file)
{
	char buffer[PATH_MAX];
	snprintf(buffer, PATH_MAX, "%s (%ls)", __func__, file);
	ERROR_OUTPUT(buffer);

	char path[PATH_MAX];
	if (wcstombs(path, file, PATH_MAX) == UINT_MAX)
	{
		ERROR_OUTPUT("Couldn't convert multibyte string to wide-character array!");
		return false;
	}

	DeleteFile(path);

	return true;
}

void UpdateIcons(bool updateDesktop)
{
	ERROR_OUTPUT(__func__);

	gcString curPath = UTIL::OS::getCurrentDir();
	gcString savedPath;

	try
	{
		savedPath = UTIL::OS::getConfigValue("InstallPath");
	}
	catch (std::exception &e)
	{
	}

	if (savedPath == curPath)
		return;

	gcString out;

	if (FileExists("desura.desktop"))
	{
#ifndef DEBUG
		gcString cmd("xdg/xdg-desktop-menu uninstall desura.desktop");
		system(cmd.c_str());
#endif
	}

	out.reserve(2000);

	out += "#!/usr/bin/env xdg-open\n";
	out += "[Desktop Entry]\n";
	out += "Version=1.0\n";
	out += "Type=Application\n";
	out += "Name=Desura\n";
	out += "GenericName=Game Manager\n";
	out += "Comment=Download and play the best games\n";
	out += gcString("Exec={0}/../desura {1}U\n", curPath, '%');
	out += gcString("TryExec={0}/../desura\n", curPath);
	out += gcString("Path={0}/..\n", curPath);
	out += "MimeType=x-scheme-handler/desura;\n";
	out += gcString("Icon={0}/../desura.png\n", curPath);
	out += "Terminal=false\n";
	out += "Categories=Game;\n";
	out += "StartupNotify=false\n";

	FILE *fh = fopen("../desura.desktop", "w");
	if (fh)
	{
		unsigned int size = fprintf(fh, out.c_str());
		if (size != out.size())
			fprintf(stderr, "Failed to update menu icon. Error writing to file.");
		fclose(fh);
		system("chmod a+x desura.desktop 2>/dev/null 1>/dev/null");

	#ifndef DEBUG
		system("xdg/xdg-desktop-menu install --novendor --mode user desura.desktop 2>/dev/null 1>/dev/null");

		system(gcString("gconftool-2 -s /desktop/gnome/url-handlers/desura/enabled --type bool true 2>/dev/null 1>/dev/null").c_str());
		system(gcString("gconftool-2 -s /desktop/gnome/url-handlers/desura/command --type string \"{0}/desura {1}s\" 2>/dev/null 1>/dev/null", curPath, '%').c_str());
		system(gcString("gconftool-2 -s /desktop/gnome/url-handlers/desura/needs_terminal --type bool false 2>/dev/null 1>/dev/null").c_str());

		// Update menus if we can
		system("update-menus 2>/dev/null 1>/dev/null");
	#endif
		try
		{
			UTIL::OS::setConfigValue("InstallPath", curPath);
		}
		catch (std::exception &e)
		{
		}
	}
	else
	{
		fprintf(stderr, "Failed to update desktop icon. Could not open file for writing");
	}

	char *env = getenv("XDG_DESKTOP_DIR");
	std::string desktopFile;

	if (env)
		desktopFile = env;

	if (desktopFile.size() == 0)
	{
		env = getenv("HOME");
		if (env)
		{
			desktopFile = env;
			desktopFile += "/Desktop";
		}
		else
		{
			return;
		}
	}

	desktopFile += "/desura.desktop";

	if(FileExists(desktopFile.c_str()) || updateDesktop)
	{
		FILE *fh = fopen(desktopFile.c_str(), "w");
		if (fh)
		{
			unsigned int size = fprintf(fh, "%s", out.c_str());
			if (size != out.size())
				fprintf(stderr, "Failed to update menu icon. Error writing to file.");
			fclose(fh);
			system(gcString("chmod a+x {0} 2>/dev/null 1>/dev/null", desktopFile.c_str()).c_str());
		}
		else
		{
			fprintf(stderr, "Failed to update desktop icon. Could not open file for writing");
		}
	}
}

