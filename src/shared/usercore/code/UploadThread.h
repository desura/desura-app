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

#ifndef DESURA_MCF_UPLOAD_THREAD_H
#define DESURA_MCF_UPLOAD_THREAD_H
#ifdef _WIN32
#pragma once
#endif


#include "MCFThread.h"
#include "util/gcTime.h"


namespace UserCore
{
	namespace Thread
	{

		enum class MCFUploadStatus : int
		{
			Ok = 0,
			GenericError = 100,
			ValidationError = 107,
			PermissionError = 108,
			ItemNotFound = 110,
			Started = 121,
			Failed = 122,
			Finished = 999,
		};

		class UploadThreadInfo : public gcRefBase
		{
		public:
			UploadThreadInfo(DesuraId id, const char* file, const char* key, uint64 start = 0)
			{
				itemId = id;
				szKey = key;
				szFile = file;
				uiStart = start;
			}

			gcString szKey;
			gcString szFile;
			DesuraId itemId;
			uint64 uiStart;

			gc_IMPLEMENT_REFCOUNTING(UploadThreadInfo)
		};

		class UploadThread : public MCFThread
		{
		public:
			UploadThread(gcRefPtr<UploadThreadInfo> info);
			virtual ~UploadThread();

		protected:
			void doRun();
			void onProgress(Prog_s& p);

			virtual void onPause();
			virtual void onUnpause();
			virtual void onStop();

			friend class UploadInfoThread;

			EventV onPauseEvent;
			EventV onUnpauseEvent;

		private:
			gcRefPtr<UploadThreadInfo> m_pInfo;
			HttpHandle m_hHttpHandle;

			uint32 m_uiChunkSize;
			uint32 m_uiContinueCount;

			uint64 m_uiFileSize;
			uint64 m_uiAmountRead;

			gcTime m_tLastProgUpdate;

			bool m_bCancel;
		};
	}
}

#endif //DESURA_UploadThread_H
