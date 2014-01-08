
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

#ifndef DLB_FILES_H
#define DLB_FILES_H

#include <string> // std::wstring
#include <sys/types.h> // _stat64i32
#include <sys/stat.h> // stat()
#include <limits.h> // MAX_PATH
#include <cerrno> // errno
#include <vector>

#include "Common.h" // ERROR_OUTPUT(), define NIX
#include "util/UtilString.h" // tokenize

#define STR_APPDATA "/AppData"

std::string GetAppPath(std::string extra = "");
std::string GetAppDataPath(std::string extra = "");
bool ChangeToAppDir();
bool FileExists(const char* file = nullptr);
bool DeleteFile(const char* file = nullptr);
bool DeleteFile(const wchar_t* file = nullptr);

#endif
