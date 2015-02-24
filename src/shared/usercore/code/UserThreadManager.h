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
		void delist(const UserThreadProxyI* pThread) override;

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
