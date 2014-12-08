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

#ifndef DESURA_INSTALLINPAGE_H
#define DESURA_INSTALLINPAGE_H
#ifdef _WIN32
#pragma once
#endif

#include "BaseInstallPage.h"
#include "wx_controls/gcSpinnerProgBar.h"

namespace UI
{
namespace Forms
{
namespace ItemFormPage
{

class InstallINPage : public BaseInstallPage 
{
public:
	InstallINPage(wxWindow* parent);
	~InstallINPage();

protected:
	virtual void init();
	virtual void onComplete(uint32&);
	virtual void onError(gcException& e);
	virtual void onMcfProgress(MCFCore::Misc::ProgressInfo& info);
	virtual void onPause(bool &state);

	void onButtonPressed(wxCommandEvent& event);

	gcStaticText* m_labLabel;
	gcStaticText* m_labInfo;
	gcSpinnerProgBar* m_pbProgress;

	gcButton* m_butPause;
	gcButton* m_butHide;

private:
	gcString m_szPath;
	bool m_bPaused;
	bool m_bCompleted;
};

}
}
}

#endif //DESURA_INSTALLINPAGE_H
