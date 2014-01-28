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
#include "UserThreadManager.h"

#include "UpdateThread.h"

#include "GatherInfoThread.h"
#include "InstalledWizardThread.h"
#include "CreateMCFThread.h"
#include "UploadPrepThread.h"
#include "UploadResumeThread.h"
#include "UploadThread.h"

#include "GetItemListThread.h"

namespace UserCore
{


UserThreadManager::UserThreadManager()
{
	m_bDestructor = false;
}

UserThreadManager::~UserThreadManager()
{
	m_bDestructor = true;

	for (size_t x=0; x<m_vThreadList.size(); x++)
	{
		m_vThreadList[x]->stop();

		//must set thread manager to null so they dont try and remove them selfs after we are deleted
		Thread::UserServiceI*	us = dynamic_cast<Thread::UserServiceI*>(m_vThreadList[x]);
		Thread::MCFThreadI*		mt = dynamic_cast<Thread::MCFThreadI*>(m_vThreadList[x]);
		Thread::UserThreadI*	ut = dynamic_cast<Thread::UserThreadI*>(m_vThreadList[x]);	

		if (us)
			us->setThreadManager(nullptr);

		if (mt)
			mt->setThreadManager(nullptr);

		if (ut)
			ut->setThreadManager(nullptr);
	}
}


void UserThreadManager::enlist(::Thread::BaseThread* pThread)
{
	//we could of all ready been shutdown
	if (m_bDestructor || !this)
		return;

	for (size_t x=0; x<m_vThreadList.size(); x++)
	{
		if (m_vThreadList[x] == pThread)
			return;	
	}

	m_vThreadList.push_back(pThread);
}

void UserThreadManager::delist(::Thread::BaseThread* pThread)
{
	//we could of all ready been shutdown
	if (!this)
		return;

	for (size_t x=0; x<m_vThreadList.size(); x++)
	{
		if (m_vThreadList[x] == pThread)
		{
			m_vThreadList.erase(m_vThreadList.begin()+x);
			break;
		}	
	}
}

Thread::UserThreadI* UserThreadManager::newUpdateThread(Event<std::tuple<gcOptional<bool>, gcOptional<bool>, gcOptional<bool>>> *onForcePollEvent, bool loadLoginItems)
{
	UserCore::Thread::UpdateThread* thread = new UserCore::Thread::UpdateThread(onForcePollEvent, loadLoginItems);
	setUpThread(thread);
	return thread;
}

Thread::MCFThreadI* UserThreadManager::newGetItemListThread()
{
	UserCore::Thread::GetItemListThread* thread = new UserCore::Thread::GetItemListThread();
	setUpThread(thread);
	return thread;
}

Thread::MCFThreadI* UserThreadManager::newInstalledWizardThread()
{
	UserCore::Thread::InstalledWizardThread* thread = new UserCore::Thread::InstalledWizardThread();
	setUpThread(thread);
	return thread;
}

Thread::MCFThreadI* UserThreadManager::newGatherInfoThread(DesuraId id, MCFBranch branch, MCFBuild build)
{
	UserCore::Thread::GatherInfoThread* thread = new UserCore::Thread::GatherInfoThread(id, branch, build);
	setUpThread(thread);
	return thread;
}

Thread::MCFThreadI* UserThreadManager::newCreateMCFThread(DesuraId id, const char* path)
{
	UserCore::Thread::CreateMCFThread* thread = new UserCore::Thread::CreateMCFThread(id, path);
	setUpThread(thread);
	return thread;
}

Thread::MCFThreadI* UserThreadManager::newUploadPrepThread(DesuraId id, const char* file)
{
	UserCore::Thread::UploadPrepThread* thread = new UserCore::Thread::UploadPrepThread(id, file);
	setUpThread(thread);
	return thread;
}

Thread::MCFThreadI* UserThreadManager::newUploadResumeThread(DesuraId id, const char* key, WebCore::Misc::ResumeUploadInfo *info)
{
	UserCore::Thread::UploadResumeThread* thread = new UserCore::Thread::UploadResumeThread(id, key, info);
	setUpThread(thread);
	return thread;
}

void UserThreadManager::setUpThread(Thread::MCFThreadI* thread)
{
	thread->setThreadManager(this);
	thread->setUpLoadManager(m_pUserCore->getUploadManager());
	thread->setWebCore(m_pUserCore->getWebCore());
	thread->setUserCore(m_pUserCore);
}

void UserThreadManager::setUpThread(Thread::UserThreadI* thread)
{
	thread->setThreadManager(this);
	thread->setWebCore(m_pUserCore->getWebCore());
	thread->setUserCore(m_pUserCore);
}


void UserThreadManager::printThreadList()
{
	Msg("-------------------------------------------\n");
	Msg(gcString("Thread count: {0}\n", m_vThreadList.size()));
	Msg("-------------------------------------------\n");

	for (size_t x=0; x<m_vThreadList.size(); x++)
	{
		gcString status;

		if (m_vThreadList[x]->isPaused())
		{
			status = gcString(" {0}: {1} [{2}]\n", x, m_vThreadList[x]->getName(), "Paused");
		}
		else if (m_vThreadList[x]->isRunning())
		{
			status = gcString(" {0}: {1} [{2}]\n", x, m_vThreadList[x]->getName(), "Running");
		}
		else if (m_vThreadList[x]->isStopped())
		{
			status = gcString(" {0}: {1} [{2}]\n", x, m_vThreadList[x]->getName(), "Stopped");
		}
		else
		{
			status = gcString(" {0}: {1} [{2}]\n", x, m_vThreadList[x]->getName(), "Unknown");
		}

		Msg(status);
	}

	Msg("-------------------------------------------\n");
}


}