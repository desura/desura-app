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


#ifndef DESURA_MCF_MAIN_H
#define DESURA_MCF_MAIN_H

#include "MCFI.h"

namespace MCFCore
{
	CEXPORT void* FactoryBuilder(const char* name);
	CEXPORT void FactoryDelete(void *p, const char* name);
}

extern "C" CEXPORT const char* GetMCFCoreVersion();

//! Basic handler class for mcf files that auto cleans up
class McfHandle
{
public:
	McfHandle()
	{
		m_pMcf = nullptr;
	}

	McfHandle(MCFCore::MCFI *MCF)
	{
		m_pMcf = nullptr;
		setHandle(MCF);
	}

	McfHandle(const McfHandle &handle)
	{
		m_pMcf = nullptr;
		setHandle(handle.m_pMcf);

		handle.m_pMcf = nullptr;
	}

	~McfHandle()
	{
		if (m_pMcf)
			MCFCore::FactoryDelete(m_pMcf, MCF_FACTORY);

		m_pMcf = nullptr;
	}

	MCFCore::MCFI* operator ->()
	{
		if (!m_pMcf)
			m_pMcf = (MCFCore::MCFI*)MCFCore::FactoryBuilder(MCF_FACTORY);

		return m_pMcf;
	}

	MCFCore::MCFI* handle() const
	{
		return m_pMcf;
	}

	MCFCore::MCFI* releaseHandle()
	{
		MCFCore::MCFI* temp = m_pMcf;
		m_pMcf = nullptr;
		return temp;
	}

	operator MCFCore::MCFI*() const
	{
		return m_pMcf;
	}

	void setHandle(MCFCore::MCFI* handle)
	{
		if (m_pMcf)
			MCFCore::FactoryDelete(m_pMcf, MCF_FACTORY);

		m_pMcf = handle;
	}

private:
	mutable MCFCore::MCFI* m_pMcf;
};

#endif
