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


#ifndef DESURA_USERTHREADMANAGERI_H
#define DESURA_USERTHREADMANAGERI_H
#ifdef _WIN32
#pragma once
#endif

#include "usercore/MCFThreadI.h"
#include "usercore/UserThreadI.h"
#include "usercore/UserServiceI.h"

namespace Thread
{
	class BaseThread;
}

namespace WebCore
{
	namespace Misc
	{
		class ResumeUploadInfo;
	}
}

namespace UserCore
{
	namespace Misc
	{
		class InstallInfoI;
	}

	class UserThreadProxyI : public gcRefBase
	{
	public:
		virtual ::Thread::BaseThread* getThread() = 0;
		virtual void cleanup() = 0;

		gc_IMPLEMENT_REFCOUNTING(UserThreadProxyI)
	};

	class UserThreadManagerI : public gcRefBase
	{
	public:
		//! Enlist a thread to be managed
		//!
		//! @param pThread thread
		//!
		virtual void enlist(gcRefPtr<UserThreadProxyI> pThread) = 0;

		//! Delist a thread to be managed
		//!
		//! @param pThread thread
		//!
		virtual void delist(const UserThreadProxyI* pThread) = 0;

		//! Sets the UserCore interface that all threads use
		//!
		//! @param uc UserCore interface
		//!
		virtual void setUserCore(gcRefPtr<UserCore::UserI> uc) = 0;

		//! Prints the current thread list
		//!
		virtual void printThreadList() = 0;

		/////////////////////////////////////////////////////////////////////////////////////////////////////
		// Application Threads
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		//! Starts a update poll thread
		//!
		virtual gcRefPtr<Thread::UserThreadI> newUpdateThread(Event<std::tuple<gcOptional<bool>, gcOptional<bool>, gcOptional<bool>>> *onForcePollEvent, bool loadLoginItems) = 0;


		/////////////////////////////////////////////////////////////////////////////////////////////////////
		// Mcf Threads
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		//! Runs the Installed wizard which looks for installed items
		//!
		virtual gcRefPtr<Thread::MCFThreadI> newInstalledWizardThread() = 0;

		virtual gcRefPtr<Thread::MCFThreadI> newGetItemListThread() = 0;

		virtual gcRefPtr<Thread::MCFThreadI> newGatherInfoThread(DesuraId id, MCFBranch branch, MCFBuild build) = 0;

		//! Uses events:
		//!
		//! getMcfProgressEvent
		//! getErrorEvent
		//! getCompleteStrEvent
		//!
		virtual gcRefPtr<Thread::MCFThreadI> newCreateMCFThread(DesuraId id, const char* path) = 0;

		virtual gcRefPtr<Thread::MCFThreadI> newUploadPrepThread(DesuraId id, const char* file) = 0;

		virtual gcRefPtr<Thread::MCFThreadI> newUploadResumeThread(DesuraId id, const char* key, gcRefPtr<WebCore::Misc::ResumeUploadInfo> &info) = 0;
	};
}

#endif //DESURA_USERTHREADMANAGERI_H
