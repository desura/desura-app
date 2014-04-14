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
#include "managers/CVar.h"
#include "CVarManager.h"

namespace
{
	std::function<bool(const CVar*, const char*)> convertToStdFunction(CVarCallBackFn callback)
	{
		std::function<bool(const CVar*, const char*)> fn = [callback](const CVar* cvar, const char* szVal)
		{
			if (callback)
				return callback(cvar, szVal);

			return true;
		};

		return fn;
	}
}

CVar::CVar(const char* name, const char* defVal, int32 flags, CVarCallBackFn callBack, CVarRegTargetI *pManager)
	: CVar(name, defVal, flags, convertToStdFunction(callBack), pManager)
{
}

CVar::CVar(const char* szName, const char* szDefVal, int32 nFlags, std::function<bool(const CVar*, const char*)> callback, CVarRegTargetI *pManager)
	: BaseItem(szName)
	, m_fnCallback(callback)
	, m_szData(szDefVal)
	, m_szDefault(szDefVal)
	, m_iFlags(nFlags)
	, m_pCVarManager(pManager)
{
	if (!m_pCVarManager)
	{
		InitCVarManger();
		m_pCVarManager = g_pCVarRegTarget;
	}

	reg(szName);
}

CVar::~CVar()
{
	deregister();
}

void CVar::reg(const char* name)
{
#ifdef WIN32
	if (m_iFlags&CVAR_LINUX_ONLY)
		return;
#else
	if (m_iFlags&CVAR_WINDOWS_ONLY)
		return;
#endif	

	if (!m_pCVarManager)
		return;

	m_bReg = m_pCVarManager->RegCVar(this);

	if (!m_bReg)
		Warning("Failed to register cvar [{0}] (maybe duplicate)\n", name);		
}

void CVar::deregister()
{
	if (m_pCVarManager && m_bReg)
		m_pCVarManager->UnRegCVar(this);

	m_bReg = false;
	m_pCVarManager = nullptr;
}

void CVar::checkOsValid() const
{
	//if this asserts, means your using a cvar thats only meant for a different platform.
#ifdef DEBUG
#ifdef WIN32
	if (m_iFlags&CVAR_LINUX_ONLY)
		gcAssert(true);
#else
	if (m_iFlags&CVAR_WINDOWS_ONLY)
		gcAssert(true);
#endif	
#endif
}

bool CVar::getBool() const
{
	checkOsValid();

	if (m_szData == "1" || m_szData == "true" || m_szData == "T" || m_szData == "TRUE" || m_szData == "True")
		return true;

	return false;
}

int32 CVar::getInt() const
{
	checkOsValid();
	return Safe::atoi(m_szData.c_str());
}

float CVar::getFloat() const
{
	checkOsValid();
	return (float)atof(m_szData.c_str());
}

const char* CVar::getExitString()
{
	checkOsValid();

	if (m_szExitData.length() > 0)
		return m_szExitData.c_str();
	else
		return getString();
}

const char* CVar::getString() const
{
	checkOsValid();

	if (m_szData.length() == 0 || m_szData[0] == '\0')
		return nullptr;

	return m_szData.c_str();
}

const char* CVar::getDefault() const
{
	checkOsValid();
	return m_szDefault.c_str();
}

void CVar::setDefault()
{
	setValue(m_szDefault.c_str());
}

void CVar::setValue(bool b)
{
	setValue(gcString("{0}", b).c_str());
}

void CVar::setValue(int32 i)
{
	setValue(gcString("{0}", i).c_str());
}

void CVar::setValue(float f)
{
	setValue(gcString("{0}", f).c_str());
}

void CVar::setValue(const char* s)
{
	checkOsValid();

	if (m_iFlags & CFLAG_SAVEONEXIT)
		m_szExitData = s;
	else
		setValueOveride(s);
}

void CVar::setValueOveride(const char* s, bool bLoadFromDb)
{
	if (s && strcmp(s, m_szData.c_str()) == 0)
		return;

	//we are in the call back all ready. Accept value to stop infant loop
	if (m_bInCallback || (bLoadFromDb && HasAnyFlags(m_iFlags, CFLAG_NOCALLBACKONLOAD)))
	{
		m_szData = s;
		return;
	}

	bool res = true;

	m_bInCallback = true;

	if (m_fnCallback)
		res = m_fnCallback(this, s);

	m_bInCallback = false;

	if (res)
		m_szData = s;
}

void CVar::parseCommand(std::vector<gcString> &vArgList)
{
	if (vArgList.size() > 1)
	{
		Msg(gcString("CVAR: {0}\n", m_szName));
		Msg(gcString("  Old Value: {0}\n", m_szData));
		setValue(vArgList[1].c_str());
		Msg(gcString("  New Value: {0}\n", m_szData));
			
		if (m_iFlags & CFLAG_SAVEONEXIT)
			Msg("  Note: Restart needed be for value is applied.\n");

		Msg("\n");
	}
	else
	{
		Msg(gcString("CVAR: {0}\n", m_szName));
		Msg(gcString("  Value: {0}\n", m_szData));	
		Msg("\n");
	}
}
