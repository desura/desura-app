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
#include "UnInstallProcess.h"
#include "McfInit.h"
#include "InstallScriptRunTime.h"


UninstallProcess::UninstallProcess(const char* mcfpath, const char* inspath, const char* installScript) : Thread::BaseThread("UninstallProcess Thread")
{
	m_szMCFPath = gcString(mcfpath);
	m_szIPath = gcString(inspath);
	m_szInstallScript = installScript;

	m_pMcfHandle = nullptr;
	m_uiLastProg = 0;
}

UninstallProcess::~UninstallProcess()
{
	stop();

	if (m_pMcfHandle)
		mcfDelFactory(m_pMcfHandle);
}

void UninstallProcess::run()
{
	gcTrace("");

	if (m_szIPath == "" || m_szMCFPath == "")
	{
		gcException errPathNull(ERR_BADPATH, gcString("One of the paths for uninstall was nullptr [IP: {0} MCF: {1}].", m_szIPath, m_szMCFPath));
		onErrorEvent(errPathNull);
		return;
	}

	MCFCore::MCFI *mcfHandle = mcfFactory();
	m_pMcfHandle = mcfHandle;
	mcfHandle->getErrorEvent() += delegate(&onErrorEvent);
	mcfHandle->getProgEvent() += delegate(this, &UninstallProcess::onProgress);

	InstallScriptRunTime isrt(m_szInstallScript.c_str(), m_szIPath.c_str());

	try
	{
		mcfHandle->setFile(m_szMCFPath.c_str());
		mcfHandle->parseMCF();

		isrt.run("PreUninstall");
		mcfHandle->removeFiles(m_szIPath.c_str());
		isrt.run("PostUninstall");
	}
	catch (gcException &except)
	{
		onErrorEvent(except);
	}

	m_pMcfHandle=nullptr;
	mcfDelFactory(mcfHandle);

	onCompleteEvent();
}

void UninstallProcess::onStop()
{
	if (m_pMcfHandle)
		m_pMcfHandle->stop();
}

void UninstallProcess::onProgress(MCFCore::Misc::ProgressInfo& p)
{
	uint64 val = p.toInt64();

	if (val == m_uiLastProg)
		return;

	onProgressEvent(val);
	m_uiLastProg = val;
}