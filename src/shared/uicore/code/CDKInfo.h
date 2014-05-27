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

#ifndef DESURA_CDKINFO_H
#define DESURA_CDKINFO_H
#ifdef _WIN32
#pragma once
#endif

#include "BasePage.h"
#include "wx_controls/gcControls.h"

class SplitInfo;

class CDKInfo : public BasePage 
{
public:
	CDKInfo(wxWindow* parent, const char* exe, bool launch, gcRefPtr<UserCore::ItemManagerI> pItemManager);
	~CDKInfo();

	void dispose(){;}
	void run(){;}

	void setInfo(DesuraId id, const char* key);

protected:
	gcTextCtrl* m_tbCdKey;
		
	gcImageButton* m_imgCopyPart;
	gcImageButton* m_imgCopyAll;

	gcButton* m_butClose;
	gcButton* m_butLaunch;
	gcButton* m_butActivate;
	
	wxStaticText* m_labInfo;

	void tokenizeKey(const char* key);
	void tokenizeKey(const char* start, const char* end, const char* key);

	void onButtonClicked(wxCommandEvent& event);

private:
	gcString m_szExe;
	bool m_bLaunch;

	uint32 m_uiCurIndex;
	std::vector<SplitInfo*> m_vSplitKey;

	wxBoxSizer* m_pButtonSizer;
};

#endif //DESURA_CDKINFO_H
