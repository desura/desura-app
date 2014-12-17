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
#include "UnInstallBranchProcess.h"
#include "McfInit.h"
#include "InstallScriptRunTime.h"
#include "mcfcore/MCFMain.h"

UninstallBranchProcess::UninstallBranchProcess(const char* oldBranchMcf, const char* newBranchMcf, const char* inspath, const char* installScript)
	: Thread::BaseThread("UninstallBranch Thread")
	, m_szOldMcfPath(oldBranchMcf)
	, m_szNewMcfPath(newBranchMcf)
	, m_szInstallPath(inspath)
	, m_szInstallScript(installScript)
{
}

UninstallBranchProcess::~UninstallBranchProcess()
{
	stop();
}

void UninstallBranchProcess::run()
{
	gcTrace("");

	if (m_szOldMcfPath == "" || m_szNewMcfPath == "")
	{
		onFinishEvent();
		return;
	}

	McfHandle oldMcf;
	McfHandle newMcf;

	oldMcf->setFile(m_szOldMcfPath.c_str());
	newMcf->setFile(m_szNewMcfPath.c_str());

	InstallScriptRunTime isrt(m_szInstallScript.c_str(), m_szInstallPath.c_str());

	try
	{
		AutoScopeLockedMemberVar<MCFCore::MCFI> asmv1(m_pOldBranchMcf, m_McfLock, oldMcf);
		AutoScopeLockedMemberVar<MCFCore::MCFI> asmv2(m_pNewBranchMcf, m_McfLock, newMcf);

		oldMcf->parseMCF();
		newMcf->parseMCF();

		oldMcf->markChanged(newMcf);

		oldMcf->getErrorEvent() += delegate(this, &UninstallBranchProcess::onError);
		oldMcf->getProgEvent()  += delegate(this, &UninstallBranchProcess::onProgress);

		isrt.run("PreUninstall");
		oldMcf->removeFiles(m_szInstallPath.c_str(), false);
		isrt.run("PostUninstall");
	}
	catch (gcException &)
	{
	}

	onCompleteEvent();
	onFinishEvent();
}

void UninstallBranchProcess::onStop()
{
	std::lock_guard<std::mutex> guard(m_McfLock);

	if (m_pNewBranchMcf)
		m_pNewBranchMcf->stop();

	if (m_pOldBranchMcf)
		m_pOldBranchMcf->stop();
}

void UninstallBranchProcess::onPause()
{
	std::lock_guard<std::mutex> guard(m_McfLock);

	if (m_pNewBranchMcf)
		m_pNewBranchMcf->pause();

	if (m_pOldBranchMcf)
		m_pOldBranchMcf->pause();
}

void UninstallBranchProcess::onUnpause()
{
	std::lock_guard<std::mutex> guard(m_McfLock);

	if (m_pNewBranchMcf)
		m_pNewBranchMcf->unpause();

	if (m_pOldBranchMcf)
		m_pOldBranchMcf->unpause();
}


void UninstallBranchProcess::onError(gcException& e)
{
	gcTrace("E: {0}", e);
	onErrorEvent(e);
}

void UninstallBranchProcess::onProgress(MCFCore::Misc::ProgressInfo& p)
{
	uint64 val = p.toInt64();

	if (val == m_uiLastProg)
		return;

	onProgressEvent(val);
	m_uiLastProg = val;
}
