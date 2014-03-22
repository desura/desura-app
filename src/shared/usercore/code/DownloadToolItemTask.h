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

#ifndef DESURA_DOWNLOADTOOLITEMTASK_H
#define DESURA_DOWNLOADTOOLITEMTASK_H
#ifdef _WIN32
#pragma once
#endif

#include "BaseItemTask.h"
#include "usercore/ItemHelpersI.h"
#include "ToolManager.h"

#include "util_thread/BaseThread.h"

namespace UserCore
{
namespace ItemTask
{


class DownloadToolTask : public UserCore::ItemTask::BaseItemTask
{
public:
	DownloadToolTask(UserCore::Item::ItemHandle* handle, ToolTransactionId ttid, const char* downloadPath, MCFBranch branch, MCFBuild build);
	DownloadToolTask(UserCore::Item::ItemHandle* handle, bool launch, ToolTransactionId ttid = UINT_MAX);

	virtual ~DownloadToolTask();
	virtual void cancel();

protected:
	void doRun();

	virtual void onPause();
	virtual void onUnpause();
	virtual void onStop();

	void onDLProgress(UserCore::Misc::ToolProgress &p);
	void onDLError(gcException &e);
	void onDLComplete();

	void onComplete();
	void validateTools();

private:
	gcString m_szDownloadPath;
	ToolTransactionId m_ToolTTID = UINT_MAX;

	bool m_bLaunch = false;
	bool m_bCancelled = false;

	::Thread::WaitCondition m_WaitCond;
};

}
}

#endif //DESURA_DOWNLOADTOOLITEMTASK_H
