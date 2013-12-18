/*
Desura is the leading indie game distribution platform
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef DESURA_MCFDPREPORTER_H
#define DESURA_MCFDPREPORTER_H
#ifdef _WIN32
#pragma once
#endif

#include "mcfcore/MCFDPReporterI.h"
#include "BaseManager.h"

#include "util_thread/BaseThread.h"
#include "util/gcTime.h"

namespace MCFCore
{



class DPProvider : public BaseItem
{
public:
	DPProvider(const char* name, uint32 id);
	~DPProvider();

	uint32 getId() const;
	uint32 getLastRate() const;
	const char* getName() const;

	void reportProgress(uint32 prog);
	void clear();

private:
	gcTime m_tStart;
	gcTime m_tLastUpdate;
	uint64 m_ullAmmount = 0;

	const gcString m_szName;
	uint32 m_uiId = 0;
};


class DPReproter : public DPReporterI, private BaseManager<DPProvider>
{
public:
	DPReproter();

	virtual uint32 getProviderCount();
	virtual uint32 getProviderId(uint32 index);
	virtual uint32 getLastRate(uint32 id);
	virtual void getName(uint32 id, char* buff, uint32 size);


	virtual uint64 getTotalSinceStart();
	virtual void resetStart();

	void reportProgress(uint32 id, uint32 prog);
	uint32 newProvider(const char* name);
	void delProvider(uint32 id);

private:
	std::mutex m_MapLock;

	uint64 m_uiTotal = 0;
	uint32 m_uiLastId = 0;
};



DPReproter* GetDPReporter();

}

#endif //DESURA_MCFDPREPORTER_H
