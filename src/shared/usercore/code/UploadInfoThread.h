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
