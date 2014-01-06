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

#ifndef DESURA_CreateInfoPage_H
#define DESURA_CreateInfoPage_H
#ifdef _WIN32
#pragma once
#endif

#include "BasePage.h"
#include "wx_controls/gcSpinnerProgBar.h"
#include "mcfcore/ProgressInfo.h"

namespace UserCore
{
	namespace Thread
	{
		class MCFThreadI;
	}
}


///////////////////////////////////////////////////////////////////////////////
/// Class CreateProgPage
///////////////////////////////////////////////////////////////////////////////
class CreateProgPage : public BasePage 
{
public:
	CreateProgPage( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
	~CreateProgPage();

	void dispose();

	void setInfo(DesuraId id, const char* path);
	void run();
		
	void setPercent(uint32 per);
	void finished();

protected:
	gcStaticText* m_staticText3;
	wxStaticText* m_labPercent;
	gcSpinnerProgBar* m_pbProgress;
	
	gcButton* m_butPause;
	gcButton* m_butCancel;
	
	void onComplete(gcString& path);
	void onError(gcException& e);
	void onProgress(MCFCore::Misc::ProgressInfo& info);

	void onButtonClick(wxCommandEvent& event);

private:
	bool m_bThreadPaused;
	gcString m_szFolderPath;

	UserCore::Thread::MCFThreadI* m_pThread;
};



#endif //DESURA_CreateInfoPage_H
