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

#ifndef DESURA_CVAR_MANAGER_H
#define DESURA_CVAR_MANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "managers/CVar.h"
#include "BaseManager.h"
#include "sqlite3x.hpp"

#include "managers/Managers.h"

enum
{
	CM_OK =0,
	CM_ERR_NOSETT_NODE,
	CM_ERR_NOUSER_NODE,
	CM_ERR_USER_NOTFOUND,
	CM_ERR_NULLUSER,
};

class CVarManager;

extern CVarManager* g_pCVarMang;
extern CVarRegTargetI* g_pCVarRegTarget;

void InitCVarManger();
void DestroyCVarManager();
void SaveCVars();


class CVarManager : public BaseManager<CVar>, public CVarManagerI, public CVarRegTargetI
{
public:
	CVarManager();
	~CVarManager();

	bool RegCVar(const gcRefPtr<CVar> &var) override;
	void UnRegCVar(const gcRefPtr<CVar> &var) override;

	void cleanUserCvars();

	void loadUser(uint32 userId);
	void loadWinUser();
	void loadNormal();

	void saveAll();
	void saveUser();
	void saveWinUser();
	void saveNormal();

	void loadFromFile(const char* file);

	gcRefPtr<CVar> findCVar(const char* name) override;
	void getCVarList(std::vector<gcRefPtr<CVar>> &vList) override;

protected:
	void loadFromDb(sqlite3x::sqlite3_reader &reader);
	void saveToDb(sqlite3x::sqlite3_command &cmd, uint8 flags);

	void loadUser(CVar* var);
	void loadWinUser(CVar* var);
	void loadNormal(CVar* var);

	void loadFromDb(sqlite3x::sqlite3_reader &reader, CVar* var);

	std::wstring getWinUser();

	void loadCVarFromDb(CVar *var, const char* szSql, gcString strExtra);

private:
	uint32 m_uiUserId = -1;
	gcString m_szCVarDb;

	bool m_bUserLoaded = false;
	bool m_bWinUserLoaded = false;
	bool m_bNormalLoaded = false;
};

#endif //DESURA_CVAR_MANAGER_H
