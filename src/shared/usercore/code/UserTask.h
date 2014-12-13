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

#ifndef DESURA_USERTASK_H
#define DESURA_USERTASK_H
#ifdef _WIN32
#pragma once
#endif

#include "webcore/WebCoreI.h"
#include "util_thread/ThreadPool.h"

namespace UserCore
{
	class UserI;

	namespace Item
	{
		class ItemInfo;
	}

	namespace Task
	{
		class UserTask : public ::Thread::BaseTask
		{
		public:
			//! Constructor
			//!
			//! @param user Usercore handle
			//! @param itemId Item id
			//!
			UserTask(gcRefPtr<UserCore::UserI> &user, DesuraId itemId = DesuraId());
			virtual ~UserTask();

			gcRefPtr<UserCore::UserI> getUserCore();
			gcRefPtr<WebCore::WebCoreI> getWebCore();


			gc_IMPLEMENT_REFCOUNTING(UserTask);

		protected:
			DesuraId getItemId();
			gcRefPtr<UserCore::Item::ItemInfo> getItemInfo();

			virtual void onStop();
			volatile bool isStopped();

            std::atomic<bool> m_bStopped = {false};

		private:
			DesuraId m_iId;

			gcRefPtr<WebCore::WebCoreI> m_pWebCore;
			gcRefPtr<UserCore::UserI> m_pUserCore;
		};


		inline DesuraId UserTask::getItemId()
		{
			return m_iId;
		}

		inline gcRefPtr<WebCore::WebCoreI> UserTask::getWebCore()
		{
			gcAssert(m_pWebCore);
			return m_pWebCore;
		}

		inline gcRefPtr<UserCore::UserI> UserTask::getUserCore()
		{
			gcAssert(m_pUserCore);
			return m_pUserCore;
		}
	}
}

#endif //DESURA_USERTASK_H
