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

#ifndef DESURA_UNINSTALLTHREAD_H
#define DESURA_UNINSTALLTHREAD_H
#ifdef _WIN32
#pragma once
#endif

#include "UIBaseServiceTask.h"

class IPCUninstallMcf;

namespace UserCore
{
namespace ItemTask
{


class UIServiceTask : public UIBaseServiceTask
{
public:
	UIServiceTask(UserCore::Item::ItemHandle* handle, bool removeAll, bool removeAcc);
	~UIServiceTask();

protected:
	virtual bool initService();
	virtual void onComplete();

	virtual void onServiceError(gcException& e);

private:
	std::shared_ptr<IPCUninstallMcf> m_pIPCUI;

	bool m_bRemoveAll;
	bool m_bRemoveAcc;
	volatile bool m_bRunning;
};

}
}

#endif
