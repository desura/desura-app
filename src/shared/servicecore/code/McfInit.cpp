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
#include "McfInit.h"
#include "SharedObjectLoader.h"
#include "Log.h"

typedef void* (*BFACT)(const char*);
typedef void (*DFACT)(void*, const char*);



SharedObjectLoader g_pMCFCore;

BFACT buildFactory = nullptr;
DFACT delFactory = nullptr;


void initFactory()
{
	bool res = false;

#if defined(WIN32) && !defined(DEBUG) && defined(DESURA_OFFICIAL_BUILD) && defined(WITH_CODESIGN)
	char message[255] = {0};
	if (UTIL::WIN::validateCert(L".\\bin\\mcfcore.dll", message, 255) != ERROR_SUCCESS)
	{
		WarningS("Cert validation failed on mcfcore.dll: {0}\n", message);
		exit(-1);
	}
#endif

#ifdef WIN32
	res = g_pMCFCore.load("mcfcore.dll");
#else
	res = g_pMCFCore.load("libmcfcore.so");
#endif

    if (!res)
        exit(-1);

	buildFactory = g_pMCFCore.getFunction<BFACT>("FactoryBuilder");

    if (!buildFactory)
        exit(-1);

	delFactory = g_pMCFCore.getFunction<DFACT>("FactoryDelete");

    if (!delFactory)
        exit(-1);

	RegDLLCB_MCF regMsg = g_pMCFCore.getFunction<RegDLLCB_MCF>("RegDLLCB_MCF");

	if (regMsg)
		InitLogging(regMsg);
}


MCFCore::MCFI* mcfFactory()
{
    if (!buildFactory)
		initFactory();

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


void shutDownFactory()
{
	buildFactory = nullptr;
	delFactory = nullptr;
	DestroyLogging();
}

namespace MCFCore
{
	CEXPORT void* FactoryBuilder(const char* name)
	{
		if (!buildFactory)
			initFactory();

		if (!buildFactory)
			return nullptr;

		return buildFactory(name);
	}

	CEXPORT void FactoryDelete(void *p, const char* name)
	{
		if (!delFactory)
			return;

		delFactory(p, name);
	}
}
