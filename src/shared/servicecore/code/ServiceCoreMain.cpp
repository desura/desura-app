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
#include <branding/servicecore_version.h>

#include "ServiceCore.h"

gcString g_szSCVersion("{0}.{1}.{2}.{3}", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILDNO, VERSION_EXTEND);

#ifdef NIX
#include "IPCServerI.h"

class Server : public IPC::PipeServer, public IPCServerI
{
public:
	Server() : IPC::PipeServer("ServiceCore", 1, true)
	{
		start();
	}

	virtual void sendMessage(const char* buffer, size_t size)
	{
		recvMessage(buffer, size);
	}

	virtual void destroy()
	{
		delete this;
	}

	void setUpPipes()
	{
	}

	void setSendCallback(void* obj, SendFn funct)
	{
		IPC::PipeServer::setSendCallback(obj, funct);
	}
};
#endif

namespace SCore
{
	CEXPORT void* FactoryBuilder(const char* name)
	{
		if (strcmp(name, SERVICE_CORE)==0)
		{
			return static_cast<void*>(new ServiceCore());
		}
#ifdef NIX
		else if (strcmp(name, IPC_SERVER) == 0)
		{
			return (IPCServerI*)new Server();
		}
#endif

		return nullptr;
	}
}

extern "C"
{

CEXPORT void* FactoryBuilderSC(const char* name)
{
	return SCore::FactoryBuilder(name);
}

CEXPORT const char* GetMCFCoreVersion()
{
	return g_szSCVersion.c_str();
}

}





