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

#include "Common.h"
#include "mcf/MCF.h"
#include "mcf/MCFHeader.h"

#include <branding/mcfcore_version.h>
#include "mcfcore/MCFMain.h"

#include "MCFDPReporter.h"

gcString g_szMCFVersion("{0}.{1}.{2}.{3}", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILDNO, VERSION_EXTEND);


gcRefPtr<MCFCore::DPReproter> g_pDPReporter;

namespace MCFCore
{
	CEXPORT void* FactoryBuilder(const char* name)
	{
		if (strcmp(name, MCF_FACTORY)==0)
		{
			return static_cast<void*>(new MCFCore::MCF());
		}
		if (strcmp(name, MCF_HEADER_FACTORY)==0)
		{
			return static_cast<void*>(new MCFCore::MCFHeader());
		}
		if (strcmp(name, MCF_DPREPORTER) == 0)
		{
			if (!g_pDPReporter)
				g_pDPReporter = MCFCore::GetDPReporter();

			return static_cast<void*>(g_pDPReporter.get());
		}
		return nullptr;
	}

	CEXPORT void FactoryDelete(void *p, const char* name)
	{
		if (!p)
			return;

		if (strcmp(name, MCF_FACTORY)==0)
		{
			MCFCore::MCF *temp = static_cast<MCFCore::MCF*>(p);
			delete temp;
		}
		if (strcmp(name, MCF_HEADER_FACTORY)==0)
		{
			MCFCore::MCFHeader *temp = static_cast<MCFCore::MCFHeader*>(p);
			delete temp;
		}
	}
}

extern "C"
{

CEXPORT void* FactoryBuilder(const char* name)
{
	return MCFCore::FactoryBuilder(name);
}

CEXPORT void FactoryDelete(void *p, const char* name)
{
	MCFCore::FactoryDelete(p, name);
}

CEXPORT const char* GetMCFCoreVersion()
{
	return g_szMCFVersion.c_str();
}

}





