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
