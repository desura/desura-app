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
			//! Constuctor
			//!
			//! @param user Usercore handle
			//! @param itemId Item id
			//!
			UserTask(UserCore::UserI *user, DesuraId itemId = DesuraId());
			virtual ~UserTask();

			UserCore::UserI* getUserCore();
			WebCore::WebCoreI* getWebCore();

		protected:
			DesuraId getItemId();
			UserCore::Item::ItemInfo* getItemInfo();

			virtual void onStop();
			volatile bool isStopped();

			std::atomic<bool> m_bStopped;
		private:
			DesuraId m_iId;

			WebCore::WebCoreI* m_pWebCore;
			UserCore::UserI* m_pUserCore;
		};


		inline DesuraId UserTask::getItemId()
		{
			return m_iId;
		}

		inline WebCore::WebCoreI* UserTask::getWebCore()
		{
			return m_pWebCore;
		}

		inline UserCore::UserI* UserTask::getUserCore()
		{
			return m_pUserCore;
		}
	}
}

#endif //DESURA_USERTASK_H
