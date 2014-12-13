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

#ifndef DESURA_UPLOADINFOTHREAD_H
#define DESURA_UPLOADINFOTHREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "BaseManager.h"
#include "usercore/UploadInfo.h"
#include "usercore/UploadInfoThreadI.h"

namespace WebCore
{
	class WebCoreI;
}

namespace UserCore
{

	class UserI;
	class UploadManagerI;

	namespace Thread
	{

		class UploadThreadInfo;
		class UploadThread;

		class UploadInfoThread : public UserCore::Misc::UploadInfoThreadI, public BaseItem
		{
		public:
			UploadInfoThread(DesuraId id, const char* key, const char* path, uint32 start = 0);
			~UploadInfoThread();

			bool isDeleted(){ return m_bDeleted; }
			void setDeleted(){ m_bDeleted = true; }

			bool shouldDelMcf(){ return m_bDelMcf; }
			void setDelMcf(bool state = true);
			bool hasStarted(){ return m_bStarted; }
			bool isCompleted(){ return m_bComplete; }

			bool hasError(){ return m_bErrored; }
			uint32 getProgress(){ return m_uiProgress; }

			void start();

			virtual DesuraId getItemId();

			//Overrides
			virtual uint64 getHash(){ return BaseItem::getHash(); }
			virtual void setStart(uint32 start);

			virtual Event<uint32>& getCompleteEvent();
			virtual Event<gcException>& getErrorEvent();
			virtual EventV& getActionEvent(){ return onActionEvent; }
			virtual Event<UserCore::Misc::UploadInfo>& getUploadProgressEvent();

			//BaseThread
			virtual void stop();
			virtual void nonBlockStop();
			virtual void unpause();
			virtual void pause();
			virtual bool isPaused();

			virtual const char* getFile();
			virtual const char* getKey();

			EventV onActionEvent;

			void setWebCore(gcRefPtr<WebCore::WebCoreI> wc);
			void setUserCore(gcRefPtr<UserCore::UserI> uc);
			void setUpLoadManager(gcRefPtr<UserCore::UploadManagerI> um);


			int addRef() override
			{
				return BaseItem::addRef();
			}

			int delRef() override
			{
				return BaseItem::delRef();
			}

			int getRefCt() override
			{
				return BaseItem::getRefCt();
			}

#if defined(DEBUG) && defined(WIN32)
			void addStackTrace(void* pObj)
			{
				BaseItem::addStackTrace(pObj);
			}

			void delStackTrace(void* pObj)
			{
				BaseItem::delStackTrace(pObj);
			}

			void dumpStackTraces()
			{
				BaseItem::dumpStackTraces();
			}
#endif

			void cleanup();

		protected:
			void onComplete(uint32& status);
			void onProgress(UserCore::Misc::UploadInfo& info);
			void onPause();
			void onUnpause();

			void onError(gcException& e);
			void deleteThread();

			Event<uint32> onCompleteEvent;
			Event<UserCore::Misc::UploadInfo> onUploadProgressEvent;
			Event<gcException> onErrorEvent;

		private:
			bool m_bStarted;
			bool m_bDeleted;
			bool m_bDelMcf;
			bool m_bComplete;
			bool m_bErrored;
			uint32 m_uiProgress;

			gcRefPtr<UploadThreadInfo> m_pUploadThreadInfo;
			gcRefPtr<UploadThread> m_pThread;

			gcRefPtr<WebCore::WebCoreI> m_pWebCore;
			gcRefPtr<UserCore::UserI> m_pUser;
			gcRefPtr<UserCore::UploadManagerI> m_pUploadManager;
		};
	}
}


#endif //DESURA_UPLOADINFO_H
