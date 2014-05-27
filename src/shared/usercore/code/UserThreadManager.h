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

#ifndef DESURA_USERTHREADMANAGER_H
#define DESURA_USERTHREADMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#define ENLIST_THREAD() GetThreadMang()->enlist(this);
#define DELIST_THREAD() GetThreadMang()->delist(this);

#include "usercore/UserThreadManagerI.h"
#include "usercore/MCFThreadI.h"
#include "usercore/UserThreadI.h"


namespace UserCore
{

	class UserThreadManager : public UserThreadManagerI
	{
	public:
		UserThreadManager();
		~UserThreadManager();


		void enlist(gcRefPtr<UserThreadProxyI> pThread) override;
		void delist(gcRefPtr<UserThreadProxyI> pThread) override;

		//inherited functions
		void setUserCore(gcRefPtr<UserCore::UserI> uc);

		void printThreadList() override;

		/////////////////////////////////////////////////////////////////////////////////////////////////////
		// Application Threads
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		gcRefPtr<Thread::UserThreadI> newUpdateThread(Event<std::tuple<gcOptional<bool>, gcOptional<bool>, gcOptional<bool>>> *onForcePollEvent, bool loadLoginItems) override;

		/////////////////////////////////////////////////////////////////////////////////////////////////////
		// Mcf Threads
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		gcRefPtr<Thread::MCFThreadI> newGetItemListThread() override;
		gcRefPtr<Thread::MCFThreadI> newInstalledWizardThread() override;
		gcRefPtr<Thread::MCFThreadI> newGatherInfoThread(DesuraId id, MCFBranch branch, MCFBuild build) override;

		gcRefPtr<Thread::MCFThreadI> newCreateMCFThread(DesuraId id, const char* path) override;
		gcRefPtr<Thread::MCFThreadI> newUploadPrepThread(DesuraId id, const char* file) override;
		gcRefPtr<Thread::MCFThreadI> newUploadResumeThread(DesuraId id, const char* key, gcRefPtr<WebCore::Misc::ResumeUploadInfo> &info) override;

		void cleanup();

		gc_IMPLEMENT_REFCOUNTING(UserThreadManager);

	protected:
		void setUpThread(gcRefPtr<Thread::UserThreadI> thread);
		void setUpThread(gcRefPtr<Thread::MCFThreadI> thread);

	private:
		std::vector<gcRefPtr<UserThreadProxyI>> m_vThreadList;
		gcRefPtr<UserCore::UserI> m_pUserCore;

		std::atomic<bool> m_bDestructor;
		std::mutex m_ThreadLock;
	};

	inline void UserThreadManager::setUserCore(gcRefPtr<UserCore::UserI> uc)
	{
		m_pUserCore = uc;
	}
}


#endif //DESURA_USERTHREADMANAGER_H
