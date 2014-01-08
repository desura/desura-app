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

#ifndef DESURA_MWPROGRESSPAGE_H
#define DESURA_MWPROGRESSPAGE_H
#ifdef _WIN32
#pragma once
#endif

#include "BasePage.h"
#include "wx_controls/gcControls.h"
#include "wx_controls/gcSpinnerProgBar.h"
#include "wx/wx.h"

#include "usercore/MCFThreadI.h"



class MWProgressPage : public BasePage  
{
public:
	MWProgressPage( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
	~MWProgressPage();

	void dispose();

	void run();
	void setAddToAccount(bool state){m_bAddToAccount = state;}

	volatile bool isStopped(){return m_bStopped;}

protected:
	friend class LanguageTestDialog;

	gcStaticText* m_labTitle;
	gcStaticText* m_labInfo;
	gcSpinnerProgBar* m_pbProgress;
	
	gcButton* m_butCancel;
	
	void onComplete(uint32&);
	void onError(gcException& e);
	void onProgress(MCFCore::Misc::ProgressInfo& info);

	void onNewItem(gcString& itemName);
	void onItemFound(DesuraId& id);

	void onButtonClicked( wxCommandEvent& event );

	void onClose( wxCloseEvent& event );
private:

	UserCore::Thread::MCFThreadI* m_pThread;
	volatile bool m_bStopped;

	bool m_bAddToAccount;

	DECLARE_EVENT_TABLE()
};


#endif //DESURA_MWPROGRESSPAGE_H
