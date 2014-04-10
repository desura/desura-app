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
#include "Managers.h"
#include "MainApp.h"

class CIMPORT uploadMng;

inline std::basic_string<char> LANGFOLDER(void)
{
	return gcString("{0}{1}language{1}", UTIL::OS::getDataPath(), DIRS_STR);
}

inline std::basic_string<char> THEMEFOLDER(void)
{
	return gcString("{0}{1}themes{1}", UTIL::OS::getDataPath(), DIRS_STR);
}


bool LangChanged(CVar* var, const char* val)
{
	gcString lan("{0}{1}.xml", LANGFOLDER(), val);

	if (GetLanguageManager().loadFromFile(lan.c_str()))
	{
		Msg(gcString("Loaded Language file: {0}\n", val));
		return true;
	}

	WarningS("Failed to Load Language file: {0}\n", val);
	return false;

}

bool ThemeChanged(CVar* var, const char* val)
{
	if (!val)
		return false;

	UTIL::FS::Path path(THEMEFOLDER(), "theme.xml", false);
	path += val;

	bool isValid = UTIL::FS::isValidFile(path);

	if (!isValid)
		WarningS("Theme {0} is not a valid theme file.\n", val);

	return isValid;
}

CVar gc_language("gc_language", "english", CFLAG_SAVEONEXIT, (CVarCallBackFn)&LangChanged);
CVar gc_theme("gc_theme", "default", CFLAG_SAVEONEXIT, (CVarCallBackFn)&ThemeChanged);


CONCOMMAND(cc_reloadlanguage, "reload_language")
{
	gcString lan("{0}{1}.xml", LANGFOLDER(), gc_language.getString());
	GetLanguageManager().loadFromFile(lan.c_str());
}


UserCore::UserThreadManagerI* GetThreadManager()
{
	if (GetUserCore())
		return GetUserCore()->getThreadManager();

	return nullptr;	
}

UserCore::UploadManagerI* GetUploadMng()
{
	if (GetUserCore())
		return GetUserCore()->getUploadManager();

	return nullptr;
}

void InitLocalManagers()
{
	GetGCThemeManager()->loadFromFolder(THEMEFOLDER().c_str());
	GetGCThemeManager()->loadTheme(gc_theme.getString());
}

wxWindow* GetMainWindow()
{
	return g_pMainApp->getMainWindow();
}

void ExitApp()
{
	g_pMainApp->Close();
}

