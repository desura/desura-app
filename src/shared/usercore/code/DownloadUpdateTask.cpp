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
#include "DownloadUpdateTask.h"

#include "mcfcore/MCFI.h"
#include "mcfcore/MCFMain.h"

#include "XMLMacros.h"
#include "User.h"

#include "IPCServiceMain.h"

#include <branding/branding.h>

using namespace UserCore::Task;


DownloadUpdateTask::DownloadUpdateTask(UserCore::User* user, uint32 appver, uint32 build, bool bForced) 
	: UserTask(user)
	, m_uiAppVer(appver)
	, m_uiAppBuild(build)
	, m_bForced(bForced)
{
}

void DownloadUpdateTask::doTask()
{
	try
	{
		downloadUpdate();
		m_bCompleted = true;
	}
	catch (gcException &except)
	{
		Warning("Failed to download " PRODUCT_NAME " Update: {0}.\n", except);
	}

	if (!m_bStopped)
	{
		uint32 prog = 0;
		onDownloadProgressEvent(prog);		
	}

	onDownloadCompleteEvent.reset();
	onDownloadStartEvent.reset();
}


void DownloadUpdateTask::onStop()
{
	m_bStopped = true;

	if (m_hMcfHandle.handle())
		m_hMcfHandle->stop();
	
	//if we didnt finish delete the incomplete file
	if (!m_bCompleted)
		UTIL::FS::delFile(UTIL::FS::PathWithFile(m_szPath));
}


//this is a feature of webcore but we need it here in case we need to shutdown during a download
void DownloadUpdateTask::downloadUpdate()
{
#ifdef DEBUG
	return;
#endif

	uint32 appver = m_uiAppVer;
	uint32 appbuild = m_uiAppBuild;

	gcString url = getWebCore()->getAppUpdateDownloadUrl(appver, appbuild);

	//check to see if its newer than last
	if (m_uiAppBuild != 0 && appbuild < m_uiAppBuild && appver == m_uiAppVer)
		return;

#ifdef WIN32
	m_szPath = UTIL::FS::Path(getUserCore()->getAppDataPath(), UPDATEFILE, false).getFullPath();
#else
	m_szPath = gcString("{0}", UPDATEFILE);
#endif

	m_hMcfHandle->setFile(m_szPath.c_str());
	m_hMcfHandle->setWorkerCount(1);
	
	try
	{
		m_hMcfHandle->dlHeaderFromHttp(url.c_str());

#ifdef WIN32
		const char* dir = ".\\";
#else
		const char* dir = "./";
#endif

		bool res = false;

		if (!m_bForced)
			res = m_hMcfHandle->verifyInstall(dir, true, false);

		m_hMcfHandle->getProgEvent() += delegate(this, &DownloadUpdateTask::onDownloadProgress);

		UserCore::Misc::update_s info;
		info.build = appbuild;

		if (!res)
		{
			Msg(gcString("Downloading " PRODUCT_NAME " update: Ver {0} build {1}\n", appver, appbuild));
			info.alert = true;

			onDownloadStartEvent(info);
			m_hMcfHandle->dlFilesFromHttp(url.c_str());

			if (!isStopped())
				onDownloadCompleteEvent(info);
		}
		else
		{
			//sometimes this gets called after shutdown and causes major problems
			if (!getUserCore() || !getUserCore()->getServiceMain())
				return;

			gcString av("{0}", appver);
			gcString ab("{0}", appbuild);
			info.alert = false;

			try
			{
				getUserCore()->getServiceMain()->updateRegKey(APPID, av.c_str());
				getUserCore()->getServiceMain()->updateRegKey(APPBUILD, ab.c_str());
			}
			catch (gcException &e)
			{
				Warning("Failed to update reg key: {0}\n", e);
			}

			onDownloadCompleteEvent(info);
		}
	}
	catch (gcException &e)
	{
		Warning("Download update failed: {0}\n", e);
	}
}

void DownloadUpdateTask::onDownloadProgress(MCFCore::Misc::ProgressInfo& p)
{
	uint32 prog = p.percent;
	
	if (m_uiLastPercent == prog)
		return;
		
	m_uiLastPercent = prog;
	onDownloadProgressEvent(prog);
}
