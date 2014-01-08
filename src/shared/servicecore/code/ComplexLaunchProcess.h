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

#ifndef DESURA_COMPLEXLAUNCHPROCESS_H
#define DESURA_COMPLEXLAUNCHPROCESS_H
#ifdef _WIN32
#pragma once
#endif

#include "util_thread/BaseThread.h"
#include "mcfcore/ProgressInfo.h"

namespace MCFCore{ class MCFI; }

class ComplexLaunchProcess : public Thread::BaseThread
{
public:
	ComplexLaunchProcess();
	~ComplexLaunchProcess();

	void setRemove(const char* modMcfPath, const char* backupMcfPath, const char* installPath, const char* installScript);
	void setInstall(const char* mcfPath, const char* installPath, const char* installScript);

	EventV onCompleteEvent;
	Event<gcException> onErrorEvent;
	Event<uint64> onProgressEvent;

protected:
	void run();
	void onStop();

	void onError(gcException& e);
	void onProgress(MCFCore::Misc::ProgressInfo& p);

	void doRemove();
	void doInstall();

private:
	gcString m_szIPath;
	gcString m_szInsMCFPath;
	gcString m_szRemMCFPath;

	gcString m_szInstallScript;

	bool m_bHashMissMatch;
	bool m_iFirstStage;
	uint8 m_iMode;
	uint64 m_uiLastProg;
};

#endif //DESURA_COMPLEXLAUNCHPROCESS_H
