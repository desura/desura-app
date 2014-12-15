/*
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)
Copyright (C) 2014 Bad Juju Games, Inc.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA.

Contact us at legal@badjuju.com.
*/

#ifndef DESURA_INSTALLBANNERPAGE_H
#define DESURA_INSTALLBANNERPAGE_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcControls.h"
#include "BaseInstallPage.h"

#include "usercore/MCFThreadI.h"
#include "usercore/GuiDownloadProvider.h"
#include "mcfcore/DownloadProvider.h"
#include "wx_controls/gcSpinnerProgBar.h"
#include "util/gcTime.h"



namespace UI
{
namespace Forms
{
namespace ItemFormPage
{

///////////////////////////////////////////////////////////////////////////////
/// Class InstallBannerPage
///////////////////////////////////////////////////////////////////////////////
class InstallBannerPage : public BaseInstallPage
{
public:
	InstallBannerPage(wxWindow* parent, bool useSpinnerFirst = false);
	~InstallBannerPage();


protected:
	virtual void onButtonPressed(wxCommandEvent& event);
	virtual void onMouseClick( wxMouseEvent& event );
	virtual void onIdle( wxIdleEvent& event );

	gcStaticText* m_labProv;
	gcStaticText* m_labLabel;
	gcStaticText* m_labInfo;
	gcSpinnerProgBar* m_pbProgress;

	gcButton* m_butCancel;
	gcImageControl* m_imgBanner;

	wxBoxSizer* m_pButSizer;


	void updateBanner();
	void setProvider(MCFCore::Misc::DownloadProvider* dprov);
	void updateProviderList();

	void onDownloadProvider(UserCore::Misc::GuiDownloadProvider& dp);

	void showDownloadBanner();
	void hideDownloadBanner();

	void checkSwap(uint8 prog);

private:
	std::vector<MCFCore::Misc::DownloadProvider*> m_vDownloadProv;
	int32 m_iCurProvider;
	gcTime m_tLastBannerChange;

	bool m_bDefaultBanner;
};

}
}
}

#endif //DESURA_INSTALLBANNERPAGE_H
