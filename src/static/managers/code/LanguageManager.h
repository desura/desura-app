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



#ifndef LANG_MANAGER_H
#define LANG_MANAGER_H

#include "BaseManager.h"
#include "managers/Managers.h"

class LanguageString : public BaseItem
{
public:
	LanguageString(const char* name) 
		: BaseItem(name)
	{
	}

	gcString str;
	gcWString ustr;
};


enum
{
	LM_OK=0,
	LM_ERR_NOLANG_NODE,
};

class LanguageManager : public BaseManager<LanguageString>, public LanguageManagerI
{
public:
	LanguageManager();
	~LanguageManager();

	const char* getString(const char* name) override;
	const wchar_t* getString(const wchar_t* name) override;

	bool loadFromFile(const char* file) override;

	void unloadAll() override;
};











#endif
