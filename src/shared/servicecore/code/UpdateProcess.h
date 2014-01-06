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

#ifndef DESURA_UPDATEPROCESS_H
#define DESURA_UPDATEPROCESS_H
#ifdef _WIN32
#pragma once
#endif

#include "util_thread/BaseThread.h"

class UMcf;

class GCUpdateProcess : public Thread::BaseThread
{
public:
	GCUpdateProcess(const char* mcfpath, const char* inspath, bool testMode);
	~GCUpdateProcess();

	EventV onCompleteEvent;
	Event<gcException> onErrorEvent;
	Event<uint32> onProgressEvent;

	static bool updateService(const char* curDir);
	static bool fixDataDir();

protected:
	void run();
	void deleteDumps();

	void install();

	void onError(gcException& e);
	void onProgress(uint32& p);

private:
	gcWString m_szIPath;
	gcWString m_szMCFPath;

	bool m_bTestMode;

	UMcf* m_pUMcf;
	UMcf* m_pOldMcf;
};

#endif //DESURA_UPDATEPROCESS_H
