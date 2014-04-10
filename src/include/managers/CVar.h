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

#ifndef DESURA_CVAR_H
#define DESURA_CVAR_H

#include "BaseManager.h"

enum
{
	CFLAG_NOFLAGS = 0<<0,	//!< No value
	CFLAG_NOSAVE = 1<<0,	//!< Dont save the value in the database (temp)
	CFLAG_USER = 1<<1,		//!< CVar only applies to the current logged in desura user, each user gets their own value
	CFLAG_WINUSER = 1<<2,	//!< CVar only applies to the current logged in windows user
	CFLAG_ADMIN = 1<<3,		//!< CVar is admin only
	CFLAG_SAVEONEXIT = 1<<4,	//!< CVar value is only updated on exit

	CVAR_LINUX_ONLY = 1<<5,		//!< CVar is linux only
	CVAR_WINDOWS_ONLY = 1<<6,	//!< CVar is windows only
	CFLAG_NOCALLBACKONLOAD = 1<<7, //!< Dont hit the callback on load from db
};

class CVar;

typedef bool (*CVarCallBackFn)(const CVar* cvar, const char* value);

class CVarRegTargetI;
extern CVarRegTargetI* g_pCVarRegTarget;

namespace UnitTest
{
	class CVarTestFixture;
}

//! CVar is a variable that is saved into a db and can be changed via console
class CVar : public BaseItem
{
public:
	//! @param name CVar display name
	//! @param defVal Default value
	//! @param flags Flags as outlined above
	//! @param callBack Function to call when value changes. Return true to accept, false to reject
	//! @param userCallBack Function to call when value changes with user data. Return true to accept, false to reject
	CVar(const char* name, const char* defVal, int32 flags = CFLAG_NOFLAGS, CVarCallBackFn callBack = nullptr, CVarRegTargetI *pManager = g_pCVarRegTarget);
	CVar(const char* szName, const char* szDefVal, int32 nFlags, std::function<bool(const CVar*, const char*)> callback, CVarRegTargetI *pManager = g_pCVarRegTarget);

	~CVar();

	bool	getBool() const;
	int32	getInt() const;
	float	getFloat() const;
	const char* getString() const;
	uint32 getFlags() const {return m_iFlags;}

	void setValue(const char* s);
	void setValue(bool b);
	void setValue(int32 i);
	void setValue(float f);

	//! Processes a console command string updating the value if needed
	void parseCommand(std::vector<gcString> &vArgList);

	//! Sets the value back to the default value
	void setDefault();
	
	//! Gets the default value
	const char* getDefault() const;

	//! Set the user data for the user data callback
	void setUserData(void* data);
	void* getUserData();

	void deregister();

protected:
	void setValueOveride(const char* s, bool bLoadFromDb = false);
	const char* getExitString();

	void reg(const char* name);
	void checkOsValid() const;

private:
	gcString m_szData;
	gcString m_szDefault;
	gcString m_szExitData;

	const std::function<bool(const CVar*, const char*)> m_fnCallback;
	CVarRegTargetI *m_pCVarManager;

	const uint32 m_iFlags;

	bool m_bReg = false;
	bool m_bInCallback = false;

	friend class CVarManager;
	friend class DesuraJSSettings;
	friend class UnitTest::CVarTestFixture;
};

#endif