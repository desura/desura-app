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

#ifndef DESURA_VSBASETASK_H
#define DESURA_VSBASETASK_H
#ifdef _WIN32
#pragma once
#endif

#include "ItemHandle.h"
#include "mcfcore/MCFMain.h"
#include "mcfcore/MCFI.h"

namespace WebCore
{
	class WebCoreI;
}

namespace UserCore
{
	class UserI;

	namespace Item
	{
		class ItemHandle;
		class ItemInfo;
	}

namespace ItemTask
{


class VSBaseTask
{
public:
	VSBaseTask();

	enum RESULT
	{
		RES_NONE,
		RES_COMPLETE,
		RES_DOWNLOAD,
		RES_INSTALL,
		RES_DOWNLOADMISSING,
		RES_STOPPED,
	};

	RESULT getResult();
	void stop();

	Event<gcException> onErrorEvent;

	void setWebCore(WebCore::WebCoreI *wc);
	void setUserCore(UserCore::UserI *uc);

	void setItemHandle(UserCore::Item::ItemHandleI* handle);
	void setMcfBuild(MCFBuild build);
	void setMcfBranch(MCFBranch branch);

	void setMcfHandle(MCFCore::MCFI* handle);

	virtual void onProgress(MCFCore::Misc::ProgressInfo& prog){}

protected:
	DesuraId getItemId();

	UserCore::Item::ItemHandleI* getItemHandle();
	UserCore::Item::ItemInfoI* getItemInfo();
	UserCore::Item::ItemInfoI* getParentItemInfo();

	WebCore::WebCoreI* getWebCore();
	UserCore::UserI* getUserCore();

	MCFBuild getMcfBuild();
	MCFBranch getMcfBranch();

	bool isStopped();

	virtual void onStop(){;}

	void setResult(RESULT res);
	bool loadMcf(gcString mcfPath);

	MCFCore::MCFI* m_hMcf;

private:
	volatile bool m_bIsStopped;

	UserCore::Item::ItemHandleI* m_pHandle;
	WebCore::WebCoreI* m_pWebCore;
	UserCore::UserI* m_pUserCore;

	MCFBranch m_uiMcfBranch;
	MCFBuild m_uiMcfBuild;

	RESULT m_Result;
};

}
}

#endif //DESURA_VSBASETASK_H
