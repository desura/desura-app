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

#pragma once

#include "util_thread/ThreadPool.h"

namespace UserCore
{
	class UserI;

	class UpdateUninstallTask : public ::Thread::BaseTask
	{
	public:
		UpdateUninstallTask(gcRefPtr<UserI> pUser)
			: m_pUser(pUser)
		{
			gcAssert(m_pUser);
		}

		virtual ~UpdateUninstallTask()
		{
		}

		const char* getName() override
		{
			return "Update Uninstall Info";
		}

		void doTask() override;

	protected:
		void getAllUninstallIds(std::vector<int64> &vIds);

	private:
		gcRefPtr<UserI> m_pUser;
	};
}
