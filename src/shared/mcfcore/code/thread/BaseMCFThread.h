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

#ifndef DESURA_BASEFILETHREAD_H
#define DESURA_BASEFILETHREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "util_thread/BaseThread.h"
#include "UpdateThread.h"
#include "mcfcore/ProgressInfo.h"


#define BLOCKSIZE (512*1024)

namespace MCFCore
{
	class MCFFile;
	class MCF;
	class MCFHeaderI;

	//! namespace for all MCFCore thread processes
	namespace Thread
	{
		enum class MCFThreadStatus
		{
			SF_STATUS_NULL,				//!< Unknown Process status
			SF_STATUS_PAUSE,			//!< Process paused
			SF_STATUS_STOP,				//!< Process stopped
			SF_STATUS_ENDFILE,			//!< Process is at the end of the file
			SF_STATUS_SKIP,				//!< Process is skipping current task
			SF_STATUS_CONTINUE,			//!< Process is continuing current task
			SF_STATUS_COMPLETE,			//!< Process has completed task
			SF_STATUS_HASHMISSMATCH,	//!< Process has a hask miss match
			SF_STATUS_ERROR,			//!< Process has an error
			SF_STATUS_WAITTASK = SF_STATUS_PAUSE,	//!< Process is waiting on task
		};

		//! BaseMCFThread is the base class for all MCFCore processes that use the producer consumer patteren
		//! and also need progress reporting
		//!
		class BaseMCFThread : public ::Thread::BaseThread
		{
		public:


			//! Constuctor
			//!
			//! @param num Number of workers
			//! @param caller Parent MCF
			//! @param name Thread name
			//!
			BaseMCFThread(uint16 num, MCFCore::MCF* caller, const char* name);

			//! virual deconstuctor
			//!
			virtual ~BaseMCFThread();

			//! Error event
			//!
			Event<gcException> onErrorEvent;

			//! Progress event
			//!
			Event<MCFCore::Misc::ProgressInfo> onProgressEvent;

		protected:
			virtual void onPause();
			virtual void onUnpause();
			virtual void onStop();

			bool m_bCompress;
			const char *m_szFile;
			uint16 m_uiNumber;

			MCFCore::MCFHeaderI* m_pHeader;
			MCFCore::Thread::UpdateProgThread *m_pUPThread;
			std::mutex m_pFileMutex;

			std::vector<std::shared_ptr<MCFCore::MCFFile>> &m_rvFileList;
			std::vector<size_t> m_vFileList;

			uint64 m_uiFileOffset;
		};
	}
}

#endif
