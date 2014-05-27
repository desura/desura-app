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

#ifndef DESURA_BASEUSERTHREAD_H
#define DESURA_BASEUSERTHREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "webcore/WebCoreI.h"
#include "usercore/UserCoreI.h"
#include "usercore/UserThreadManagerI.h"

#include "ItemInfo.h"

namespace UserCore
{
	namespace Thread
	{
		template<class Interface, class Base>
		class BaseUserThread : public Interface, public Base, public UserThreadProxyI
		{
		public:
			BaseUserThread(const char* name, DesuraId id) : Base(name)
			{
				m_iId = id;
				m_pWebCore = nullptr;
				m_pUserCore = nullptr;
				m_pThreadManager = nullptr;
			}

			virtual ~BaseUserThread()
			{
				stop();

				if (m_pThreadManager)
					m_pThreadManager->delist(gcRefPtr<UserThreadProxyI>(this));
			}

			::Thread::BaseThread* getThread()
			{
				return this;
			}

			DesuraId getItemId()
			{
				return m_iId;
			}

			gcRefPtr<WebCore::WebCoreI> getWebCore()
			{
				return m_pWebCore;
			}

			gcRefPtr<UserCore::UserI> getUserCore()
			{
				return m_pUserCore;
			}

			void setThreadManager(gcRefPtr<UserCore::UserThreadManagerI> tm) override
			{
				m_pThreadManager = tm;

				if (m_pThreadManager)
					m_pThreadManager->enlist(gcRefPtr<UserThreadProxyI>(this));
			}

			void setWebCore(gcRefPtr<WebCore::WebCoreI> wc) override
			{
				m_pWebCore = wc;
			}

			void setUserCore(gcRefPtr<UserCore::UserI> uc) override
			{
				m_pUserCore = uc;
			}

			Event<uint32>& getCompleteEvent()
			{
				return onCompleteEvent;
			}

			Event<uint32>& getProgressEvent()
			{
				return onProgUpdateEvent;
			}

			Event<gcException>& getErrorEvent()
			{
				return onErrorEvent;
			}

			Event<WCSpecialInfo>& getNeedWCEvent()
			{
				return onNeedWCEvent;
			}

			Event<UserCore::Misc::GuiDownloadProvider>& getDownloadProviderEvent()
			{
				return onNewProviderEvent;
			}

			void start() override
			{
				Base::start();
			}

			void stop() override
			{
				Base::stop();
			}

			void nonBlockStop() override
			{
				Base::nonBlockStop();
			}

			void stopAndDelete(bool nonBlock = false)
			{
				Base::stopAndDelete(nonBlock);
			}

			void unpause() override
			{
				Base::unpause();
			}

			void pause() override
			{
				Base::pause();
			}


			gcRefPtr<UserCore::Item::ItemInfo> getItemInfo()
			{
				if (!m_pUserCore || !m_iId.isOk())
					return nullptr;

				return gcRefPtr<UserCore::Item::ItemInfo>::dyn_cast(m_pUserCore->getItemManager()->findItemInfo(m_iId));
			}

			gcRefPtr<UserCore::Item::ItemInfo> getParentItemInfo()
			{
				auto item = getItemInfo();

				if (!m_pUserCore || !item)
					return nullptr;

				return gcRefPtr<UserCore::Item::ItemInfo>::dyn_cast(m_pUserCore->getItemManager()->findItemInfo(item->getParentId()));
			}

			void cleanup() override
			{
				gcAssert(Base::isStopped() || !Base::isRunning());

				m_pWebCore.reset();
				m_pUserCore.reset();
				m_pThreadManager.reset();
			}

			gc_IMPLEMENT_REFCOUNTING(BaseUserThread);

		protected:
			virtual void run()
			{
				if (!m_pWebCore || !m_pUserCore)
				{
					gcException e(ERR_BADCLASS);
					onErrorEvent(e);
					return;
				}

				try
				{
					doRun();
				}
				catch (gcException& e)
				{
					onErrorEvent(e);
				}
			}

			Event<uint32> onCompleteEvent;
			Event<uint32> onProgUpdateEvent;
			Event<gcException> onErrorEvent;
			Event<WCSpecialInfo> onNeedWCEvent;

			//download mcf
			Event<UserCore::Misc::GuiDownloadProvider> onNewProviderEvent;

			virtual void doRun() = 0;

		private:
			DesuraId m_iId;

			gcRefPtr<WebCore::WebCoreI> m_pWebCore;
			gcRefPtr<UserCore::UserI> m_pUserCore;
			gcRefPtr<UserCore::UserThreadManagerI> m_pThreadManager;

			friend class CreateMCFThread; //so we can access usercore
		};
	}
}

#endif //DESURA_BASEUSERTHREAD_H
