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

#ifndef DESURA_UIBASESERVICETASK_H
#define DESURA_UIBASESERVICETASK_H
#ifdef _WIN32
#pragma once
#endif

#include "BaseItemServiceTask.h"
#include "User.h"
#include "ItemInfo.h"
#include "webcore/WebCoreI.h"

namespace UserCore
{
namespace ItemTask
{


class UIBaseServiceTask : public BaseItemServiceTask
{
public:
	UIBaseServiceTask(UserCore::Item::ITEM_STAGE stage, const char* taskname, gcRefPtr<UserCore::Item::ItemHandleI> &handle, MCFBranch installBranch = MCFBranch(), MCFBuild installBuild = MCFBuild());
	~UIBaseServiceTask();

	virtual void onServiceError(gcException& e);

protected:
	virtual void onComplete();
	virtual void onStop();
	
	gcString getBranchMcf(DesuraId id, MCFBranch branch, MCFBuild build);
	bool checkPath(const char* path, MCFBranch branch, MCFBuild build);

	virtual bool initService();

	void completeUninstall(bool removeAll, bool removeAccount);

#ifdef NIX
	void removeScripts();
#endif

	MCFBranch m_OldBranch;
	MCFBuild m_OldBuild;

	DesuraId m_idLastInstalledMod;
};


}
}

#endif //DESURA_UIBASESERVICETASK_H
