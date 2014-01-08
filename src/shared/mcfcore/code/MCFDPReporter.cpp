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
#include "MCFDPReporter.h"

namespace MCFCore
{


DPReproter g_DPReporter;

DPReproter* GetDPReporter()
{
	return &g_DPReporter;
}




DPReproter::DPReproter() 
	: BaseManager<DPProvider>(true)
{
}

uint32 DPReproter::getProviderCount()
{
	return getCount();
}

uint32 DPReproter::getProviderId(uint32 index)
{
	std::lock_guard<std::mutex> guard(m_MapLock);
	DPProvider* item = getItem(index);

	if (item)
		return item->getId();

	return -1;
}

void DPReproter::getName(uint32 id, char* buff, uint32 size)
{
	std::lock_guard<std::mutex> guard(m_MapLock);
	DPProvider* item = findItem(id);

	if (item)
	{
		const char* name = item->getName();
		Safe::strncpy(buff, size, name, size);
	}
}

uint32 DPReproter::getLastRate(uint32 id)
{
	std::lock_guard<std::mutex> guard(m_MapLock);
	DPProvider* item = findItem(id);

	if (item)
		return item->getLastRate();

	return -1;
}

void DPReproter::reportProgress(uint32 id, uint32 prog)
{
	m_uiTotal += prog;

	std::lock_guard<std::mutex> guard(m_MapLock);
	DPProvider* item = findItem(id);

	if (item)
		item->reportProgress(prog);
}

uint32 DPReproter::newProvider(const char* name)
{
	uint32 id = m_uiLastId;
	m_uiLastId++;

	std::lock_guard<std::mutex> guard(m_MapLock);
	addItem(new DPProvider(name, id));
	return id;
}

void DPReproter::delProvider(uint32 id)
{
	std::lock_guard<std::mutex> guard(m_MapLock);
	removeItem(id, true);
}

uint64 DPReproter::getTotalSinceStart()
{
	return m_uiTotal;
}

void DPReproter::resetStart()
{
	m_uiTotal = 0;
}























DPProvider::DPProvider(const char* name, uint32 id) 
	: BaseItem()
	, m_uiId(id)
	, m_szName(name)
{
	m_uiHash = id;
}

DPProvider::~DPProvider()
{
}

void DPProvider::clear()
{
	reportProgress(0);
}

void DPProvider::reportProgress(uint32 prog)
{
	m_ullAmmount += prog;
	m_tLastUpdate = gcTime();
}

uint32 DPProvider::getId() const
{
	return m_uiId;
}

const char* DPProvider::getName() const
{
	return m_szName.c_str();
}

uint32 DPProvider::getLastRate() const
{
	if (m_ullAmmount < 1)
		return 0;
	
	auto total = gcTime() - m_tStart;

	if (total.seconds() == 0)
		return 0;

	double avgRate = ((double)m_ullAmmount) / ((double)total.seconds());
	return (uint32)(avgRate/1024.0);
}




}
