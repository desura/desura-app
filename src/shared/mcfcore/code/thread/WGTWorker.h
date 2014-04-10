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

#ifndef DESURA_WTWORKERTHREAD_H
#define DESURA_WTWORKERTHREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "util_thread/BaseThread.h"
#include "MCFServerCon.h"
#include "mcfcore/MCFI.h"

class webControl;
class OutToBlock;

namespace UnitTest
{
	class TestWGTWorker;
}

namespace MCFCore
{
	namespace Misc
	{
		class ProviderManager;
		class MCFServerConI;
	}

	namespace Thread
	{
		class WGTControllerI;

		namespace Misc
		{
			class WGTSuperBlock;
			class WGTBlock;
		}

		//! Web get thread worker. Does all the work in regards to downloading mcf files
		//!
		class WGTWorker : public ::Thread::BaseThread, protected MCFCore::Misc::OutBufferI
		{
		public:
			//! Constuctor
			//! 
			//! @param controller Parent class
			//! @param id Worker id
			//! @param pProvMng Provider manager
			//! @param pFileAuth Mcf download auth
			//!
			WGTWorker(WGTControllerI* controller, uint16 id, MCFCore::Misc::ProviderManager &provMng);
			~WGTWorker();

			//! Reports a download error from controller
			//!
			void reportError(gcException &e, gcString provider);

		protected:
			void run();
			void onStop();

			//! Does the download block work
			//!
			void doDownload();

			//! On progress event callback
			//!
			//! @param prog Current progress
			//!
			void onProgress(uint32& prog);

			void takeProgressOff();
			void requestNewUrl(gcException& e);

			bool isGoodSocketError(uint32 errId);

			virtual bool writeData(char* data, uint32 size);
			virtual void reset();

			bool checkBlock(MCFCore::Thread::Misc::WGTBlock *block);

            virtual bool isThreadStopped();

		private:
			friend class UnitTest::TestWGTWorker;

			MCFCore::Misc::MCFServerConI *m_pMcfCon = nullptr;
			std::shared_ptr<const MCFCore::Misc::DownloadProvider> m_DownloadProvider;

			uint32 m_iAttempt = 0;
			uint32 m_uiId = 0;

			std::mutex m_DeleteMutex;

			MCFCore::Thread::WGTControllerI *m_pCT = nullptr;
			MCFCore::Thread::Misc::WGTSuperBlock *m_pCurBlock = nullptr;
			MCFCore::Misc::ProviderManager &m_ProvMng;

			std::mutex m_ErrorMutex;
			bool m_bError = false;
			gcException m_Error;
		};

	}
}

#endif //DESURA_WTWORKERTHREAD_H
