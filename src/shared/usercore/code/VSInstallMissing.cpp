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
#include "VSInstallMissing.h"

#include "IPCServiceMain.h"
#include "IPCInstallMcf.h"

#include "ItemHandle.h"
#include "ItemInfo.h"
#include "usercore/UserCoreI.h"
#include "webcore/WebCoreI.h"

namespace UserCore
{
namespace ItemTask
{

VSInstallMissing::VSInstallMissing()
{
	m_bFinished = false;
	m_pIPCIM = nullptr;
}

VSInstallMissing::~VSInstallMissing()
{
	if (!m_bFinished)
		m_WaitCond.wait();

	if (m_pIPCIM)
		m_pIPCIM->destroy();

	m_pIPCIM = nullptr;
}

bool VSInstallMissing::doTask()
{
	const char* val = getUserCore()->getCVarValue("gc_corecount");
	gcException errFailMCFServ(ERR_NULLHANDLE, "Failed to create uninstall MCF service!\n");

	m_pIPCIM = getUserCore()->getServiceMain()->newInstallMcf();
	if (!m_pIPCIM)
	{
		onErrorEvent(errFailMCFServ);
		return false;
	}

	uint8 workers = 1;
		
	if (val)
		workers = atoi(val);

	if (workers == 0)
		workers = 1;

	m_pIPCIM->onCompleteEvent += delegate(this, &VSInstallMissing::onComplete);
	m_pIPCIM->onProgressEvent += delegate(&onProgressEvent);
	m_pIPCIM->onErrorEvent += delegate(&onErrorEvent);
	m_pIPCIM->start(m_hMcf->getFile(), getItemInfo()->getPath(), getItemInfo()->getInstallScriptPath(), workers);

	m_WaitCond.wait();
	
	m_bFinished = true;

	return isStopped() == false;
}

void VSInstallMissing::onStop()
{
	m_WaitCond.notify();
}

void VSInstallMissing::onComplete()
{
	m_WaitCond.notify();
}





}
}
