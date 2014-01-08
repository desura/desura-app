/*
Desura is the leading indie game distribution platform
Copyright (C) 2013 Mark Chandler (Desura Net Pty Ltd)

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
#include "SharedObjectLoader.h"

#include "mcfcore/MCFI.h"
#include "mcfcore/MCFHeaderI.h"

typedef void* (*BFACT)(const char*);
typedef void (*DFACT)(void*, const char*);
typedef const char* (*VFACT)();


BFACT buildFactory = nullptr;
DFACT delFactory = nullptr;

SharedObjectLoader g_hMcfCore;

const char* GetMcfVersion()
{
	VFACT mcfVersion = g_hMcfCore.getFunction<VFACT>("GetMCFCoreVersion");

	if (!mcfVersion)
		return "Null";

	return mcfVersion();
}

void InitFactory()
{
#ifdef NIX
    if (!g_hMcfCore.load("libmcfcore.so"))
    {
        fprintf(stderr, "Cannont open library: %s\n", dlerror());
        printf("Error loading mcfcore.so\n");
        exit(-1);
    }
#else
		if (!g_hMcfCore.load("mcfcore.dll"))
		{
			printf("Error loading mcfcore.dll\n");
			exit(-1);
		}
#endif

    buildFactory = g_hMcfCore.getFunction<BFACT>("FactoryBuilder");
    if (!buildFactory)
    {
        printf("Error loading buildfactory\n");
        exit(-1);
    }

	delFactory = g_hMcfCore.getFunction<DFACT>("FactoryDelete");
    if (!delFactory)
    {
        printf("Error loading del factory\n");
        exit(-1);
    }
}

MCFCore::MCFI* mcfFactory()
{
    if (!buildFactory)
        return nullptr;

    void* temp = buildFactory(MCF_FACTORY);

    return static_cast<MCFCore::MCFI*>(temp);
}

void mcfDelFactory(void* p)
{
    if (!delFactory)
        return;

	delFactory(p, MCF_FACTORY);
}
