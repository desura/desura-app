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


#ifndef DESURA_UPLOADPROGPAGE_H
#define DESURA_UPLOADPROGPAGE_H
#ifdef _WIN32
#pragma once
#endif

#include "BasePage.h"

#include "Event.h"
#include "usercore/UploadInfo.h"
#include "wx_controls/gcControls.h"

#include <wx/tglbtn.h>


///////////////////////////////////////////////////////////////////////////////
/// Class UploadProgPage
///////////////////////////////////////////////////////////////////////////////
class UploadProgPage : public BasePage 
{
public:
	UploadProgPage(wxWindow* parent);
	~UploadProgPage();

	void dispose();

	void setInfo(DesuraId id, uint32 hash, uint32 start);
	void run();

protected:
	void uploadDone();
	void onButClick( wxCommandEvent& event );

	void onUploadUpdate();
	void onComplete(uint32& status);
	void onError(gcException& e);
	void onProgress(UserCore::Misc::UploadInfo& info);
	void onAction();
	void onChecked( wxCommandEvent& event );

	gcStaticText* m_staticText3;
	gcStaticText* m_labTimeLeft;

	gcULProgressBar* m_pbProgress;
	
	gcButton* m_butPause;
	gcButton* m_butCancel;

	gcCheckBox *m_cbDeleteMcf;

private:	
	uint32 m_iStart;
	uint32 m_uiUploadHash;

	bool m_bDone;

	uint64 m_llTotalUpload;

	DECLARE_EVENT_TABLE();
};


#endif //DESURA_UPLOADPROGPAGE_H
