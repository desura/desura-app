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

#ifndef DESURA_CDKEYMANAGER_H
#define DESURA_CDKEYMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "util_thread/BaseThread.h"
#include "usercore/CDKeyManagerI.h"
#include "UserTasks.h"

typedef std::vector<std::pair<DesuraId, gcRefPtr<UserCore::Misc::CDKeyCallBackI>> > CDKeyList;
typedef std::vector<gcRefPtr<UserCore::Task::CDKeyTask>> CDKeyTaskList;

namespace UserCore
{
	class User;

	class CDKeyManager : public CDKeyManagerI
	{
	public:
		CDKeyManager(gcRefPtr<UserCore::User> user);
		~CDKeyManager();

		void getCDKeyForCurrentBranch(DesuraId id, gcRefPtr<UserCore::Misc::CDKeyCallBackI> callback) override;
		void cancelRequest(DesuraId id, gcRefPtr<UserCore::Misc::CDKeyCallBackI> callback) override;
		bool hasCDKeyForCurrentBranch(DesuraId id) override;

		gc_IMPLEMENT_REFCOUNTING(CDKeyManager);

	protected:
		void onCDKeyComplete(UserCore::Task::CDKeyEventInfo<gcString> &info);
		void onCDKeyError(UserCore::Task::CDKeyEventInfo<gcException> &info);

		void removeTask(gcRefPtr<UserCore::Task::CDKeyTask> &task);

	private:
		std::mutex m_MapLock;
		CDKeyList m_mCDKeyCallbackList;

		std::mutex m_TaskListLock;
		CDKeyTaskList m_vCDKeyTaskList;

		gcRefPtr<UserCore::User> m_pUser;
	};

}

#endif //DESURA_CDKEYMANAGER_H
