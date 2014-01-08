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

#ifndef DESURA_INSTALLWAITPAGE_H
#define DESURA_INSTALLWAITPAGE_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcControls.h"
#include "BaseInstallPage.h"

class ItemPanel;

namespace UI
{
namespace Forms
{
namespace ItemFormPage
{

class InstallWaitPage : public BaseInstallPage 
{
public:
	InstallWaitPage(wxWindow* parent);
	~InstallWaitPage();
	
	void init();

protected:
	wxScrolledWindow* m_swItemList;
	wxFlexGridSizer* m_pSWSizer;

	wxStaticText* m_labInfo;
	
		
	gcButton* m_butCancelAll;
	gcButton* m_butForce;
	gcButton* m_butHide;
	
	virtual void onProgressUpdate(uint32& progress);
	virtual void onMcfProgress(MCFCore::Misc::ProgressInfo& info);

	void onButtonPressed(wxCommandEvent& event);

private:
	std::vector<ItemPanel*> m_vPanelList;

};


}
}
}

#endif //DESURA_InstallWaitPage_H
