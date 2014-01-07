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

#include "Common.h"
#include "InstallDLPage.h"
#include "MainApp.h"

#define DEFAULT_BANNER "#icon_download"

namespace UI
{
namespace Forms
{
namespace ItemFormPage
{

InstallDLPage::InstallDLPage(wxWindow* parent) : InstallBannerPage(parent, true)
{
	m_butPause = new gcButton(this, wxID_ANY, Managers::GetString(L"#PAUSE"));
	m_butHide = new gcButton(this, wxID_ANY, Managers::GetString(L"#HIDE"));
	m_butCancel->SetLabel(Managers::GetString(L"#CANCEL"));

	m_pButSizer->Clear(false);
	m_pButSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	m_pButSizer->Add( m_butHide, 0,  wxTOP|wxBOTTOM|wxLEFT, 5 );
	m_pButSizer->Add( m_butPause, 0,  wxTOP|wxBOTTOM|wxLEFT, 5 );
	m_pButSizer->Add( m_butCancel, 0, wxALL, 5 );

	m_butPause->Enable(false);
	m_butCancel->Enable(false);

	this->Layout();

	m_bPaused = false;
	m_bError = false;
	m_bInit = true;

	this->setParentSize(-1, 240);
}

InstallDLPage::~InstallDLPage()
{
}

void InstallDLPage::init()
{
}

void InstallDLPage::onButtonPressed(wxCommandEvent& event)
{
	if (event.GetId() == m_butPause->GetId())
		getItemHandle()->setPaused(!m_bPaused);

	if (event.GetId() == m_butHide->GetId())
		GetParent()->Close();

	if (event.GetId() == m_butCancel->GetId())
		getItemHandle()->cancelCurrentStage();
}

void InstallDLPage::onPause(bool &state)
{
	gcFrame* par = dynamic_cast<gcFrame*>(GetParent());

	m_bPaused = state;

	if (!state)
	{
		m_butPause->SetLabel(Managers::GetString(L"#PAUSE"));

		if (par)
			par->setProgressState(gcFrame::P_NORMAL);
	}
	else
	{
		m_labInfo->SetLabel(Managers::GetString(L"#PAUSED"));
		m_butPause->SetLabel(Managers::GetString(L"#RESUME"));

		if (par)
			par->setProgressState(gcFrame::P_PAUSED);
	}
}

void InstallDLPage::onComplete(gcString& path)
{
	if (m_bError)
		return;

	gcFrame* par = dynamic_cast<gcFrame*>(GetParent());
	if (par)
		par->setProgressState(gcFrame::P_NONE);

	m_labInfo->SetLabel(Managers::GetString(L"#COMPLTETED"));
}


class DownloadErrorHelper : public HelperButtonsI
{
public:
	DownloadErrorHelper(DesuraId id)
	{
		m_Id = id;
	}

	virtual uint32 getCount()
	{
		return 2;
	}

	virtual const wchar_t* getLabel(uint32 index)
	{
		if (index == 0)
			return Managers::GetString(L"#TRYAGAIN");

		return Managers::GetString(L"#MENU_CONSOLE");
	}

	virtual const wchar_t* getToolTip(uint32 index)
	{
		return nullptr;
	}

	virtual void performAction(uint32 index)
	{
		if (index == 0)
			g_pMainApp->handleInternalLink(m_Id, ACTION_INSTALL);

		return g_pMainApp->handleInternalLink(m_Id, ACTION_SHOWCONSOLE);
	}

	DesuraId m_Id;
};

void InstallDLPage::onError(gcException& e)
{
	gcFrame* par = dynamic_cast<gcFrame*>(GetParent());
	if (par)
		par->setProgressState(gcFrame::P_ERROR);

	m_bError = true;

	if (!getItemHandle()->shouldPauseOnError())
	{
		DownloadErrorHelper helper(getItemId());
		gcErrorBox(GetParent(), "#IF_DLERRTITLE", "#IF_DLERROR", e, &helper);
	}
}

void InstallDLPage::onMcfProgress(MCFCore::Misc::ProgressInfo& info)
{
	if (m_bInit && (info.flag & MCFCore::Misc::ProgressInfo::FLAG_INITFINISHED))
	{
		m_bInit = false;

		m_butPause->Enable(true);
		m_butCancel->Enable(true);
		m_pbProgress->setProgress(0);
	}
	else if (info.flag & MCFCore::Misc::ProgressInfo::FLAG_FINALIZING)
	{
		m_labInfo->SetLabel( Managers::GetString(L"#FINALIZING") );
		m_butPause->Enable(false);
		m_butCancel->Enable(false);
		m_pbProgress->setProgress(100);
	}
	else
	{
		if (!m_bInit)
		{
			std::string lab = UTIL::MISC::genTimeString(info.hour, info.min, info.rate);
			m_labInfo->SetLabel(lab);


			if (info.totalAmmount > 0)
			{
				std::string done = UTIL::MISC::niceSizeStr(info.doneAmmount, true);
				std::string total = UTIL::MISC::niceSizeStr(info.totalAmmount);

				m_pbProgress->setCaption(gcString(Managers::GetString("#PROGRESS_INFO"), done, total));
			}
		}

		m_pbProgress->setProgress(info.percent);

		gcFrame* par = dynamic_cast<gcFrame*>(GetParent());
		if (par)
			par->setProgress(info.percent);
	}

	Refresh(false);
}


}
}
}