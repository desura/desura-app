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

using namespace UserCore;


UserThreadManager::UserThreadManager()
{
}

UserThreadManager::~UserThreadManager()
{
	m_bDestructor = true;

	{
		std::lock_guard<std::mutex> guard(m_ThreadLock);
		gcAssert(m_vThreadList.empty());
	}

	cleanup();
}


void UserThreadManager::enlist(gcRefPtr<UserThreadProxyI> pThread)
{
	//we could of all ready been shutdown
	if (m_bDestructor || !this)
		return;

	std::lock_guard<std::mutex> guard(m_ThreadLock);

	if (std::find(begin(m_vThreadList), end(m_vThreadList), pThread) != end(m_vThreadList))
	{
		gcAssert(false);
		return;
	}

	m_vThreadList.push_back(pThread);
}

void UserThreadManager::delist(gcRefPtr<UserThreadProxyI> pThread)
{
	//we could of all ready been shutdown
	if (m_bDestructor || !this)
		return;

	std::lock_guard<std::mutex> guard(m_ThreadLock);

	auto it = std::find(begin(m_vThreadList), end(m_vThreadList), pThread);

	if (it == end(m_vThreadList))
	{
		gcAssert(false);
		return;
	}

	m_vThreadList.erase(it);
}

gcRefPtr<UserCore::Thread::UserThreadI> UserThreadManager::newUpdateThread(Event<std::tuple<gcOptional<bool>, gcOptional<bool>, gcOptional<bool>>> *onForcePollEvent, bool loadLoginItems)
{
	gcRefPtr<Thread::UserThreadI> thread = gcRefPtr<UserCore::Thread::UpdateThread>::create(onForcePollEvent, loadLoginItems);
	setUpThread(thread);
	return thread;
}

gcRefPtr<UserCore::Thread::MCFThreadI> UserThreadManager::newGetItemListThread()
{
	gcRefPtr<Thread::MCFThreadI> thread = gcRefPtr<UserCore::Thread::GetItemListThread>::create();
	setUpThread(thread);
	return thread;
}

gcRefPtr<UserCore::Thread::MCFThreadI> UserThreadManager::newInstalledWizardThread()
{
	gcRefPtr<Thread::MCFThreadI> thread = gcRefPtr<UserCore::Thread::InstalledWizardThread>::create();
	setUpThread(thread);
	return thread;
}

gcRefPtr<UserCore::Thread::MCFThreadI> UserThreadManager::newGatherInfoThread(DesuraId id, MCFBranch branch, MCFBuild build)
{
	gcRefPtr<Thread::MCFThreadI> thread = gcRefPtr<UserCore::Thread::GatherInfoThread>::create(id, branch, build);
	setUpThread(thread);
	return thread;
}

gcRefPtr<UserCore::Thread::MCFThreadI> UserThreadManager::newCreateMCFThread(DesuraId id, const char* path)
{
	gcRefPtr<Thread::MCFThreadI> thread = gcRefPtr<UserCore::Thread::CreateMCFThread>::create(id, path);
	setUpThread(thread);
	return thread;
}

gcRefPtr<UserCore::Thread::MCFThreadI> UserThreadManager::newUploadPrepThread(DesuraId id, const char* file)
{
	gcRefPtr<Thread::MCFThreadI> thread = gcRefPtr<UserCore::Thread::UploadPrepThread>::create(id, file);
	setUpThread(thread);
	return thread;
}

gcRefPtr<UserCore::Thread::MCFThreadI> UserThreadManager::newUploadResumeThread(DesuraId id, const char* key, gcRefPtr<WebCore::Misc::ResumeUploadInfo> &info)
{
	gcRefPtr<Thread::MCFThreadI> thread = gcRefPtr<UserCore::Thread::UploadResumeThread>::create(id, key, info);
	setUpThread(thread);
	return thread;
}

void UserThreadManager::setUpThread(gcRefPtr<UserCore::Thread::MCFThreadI> thread)
{
	thread->setThreadManager(gcRefPtr<UserThreadManager>(this));
	thread->setUpLoadManager(m_pUserCore->getUploadManager());
	thread->setWebCore(m_pUserCore->getWebCore());
	thread->setUserCore(m_pUserCore);
}

void UserThreadManager::setUpThread(gcRefPtr<UserCore::Thread::UserThreadI> thread)
{
	thread->setThreadManager(gcRefPtr<UserThreadManager>(this));
	thread->setWebCore(m_pUserCore->getWebCore());
	thread->setUserCore(m_pUserCore);
}


void UserThreadManager::printThreadList()
{
	std::lock_guard<std::mutex> guard(m_ThreadLock);

	Msg("-------------------------------------------\n");
	Msg(gcString("Thread count: {0}\n", m_vThreadList.size()));
	Msg("-------------------------------------------\n");

	size_t x = 1;
	for (auto t : m_vThreadList)
	{
		gcString status;

		if (t->getThread()->isPaused())
		{
			status = gcString(" {0}: {1} [{2}]\n", x, t->getThread()->getName(), "Paused");
		}
		else if (t->getThread()->isRunning())
		{
			status = gcString(" {0}: {1} [{2}]\n", x, t->getThread()->getName(), "Running");
		}
		else if (t->getThread()->isStopped())
		{
			status = gcString(" {0}: {1} [{2}]\n", x, t->getThread()->getName(), "Stopped");
		}
		else
		{
			status = gcString(" {0}: {1} [{2}]\n", x, t->getThread()->getName(), "Unknown");
		}

		Msg(status);
		x++;
	}

	Msg("-------------------------------------------\n");
}

void UserThreadManager::cleanup()
{
	std::vector<gcRefPtr<UserThreadProxyI>> vTemp;

	{
		std::lock_guard<std::mutex> guard(m_ThreadLock);
		vTemp = m_vThreadList;
		m_vThreadList.clear();
	}

	for (auto t : vTemp)
	{
		t->getThread()->stop();
		t->cleanup();
	}

	m_pUserCore = nullptr;
}