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

#ifndef DESURA_GCUPDATEFORM_H
#define DESURA_GCUPDATEFORM_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcControls.h"
#include "gcMiscWebControl.h"


///////////////////////////////////////////////////////////////////////////////
/// Class GCUpdateInfo
///////////////////////////////////////////////////////////////////////////////
class GCUpdateInfo : public gcFrame
{
public:
	GCUpdateInfo(wxWindow* parent);
	~GCUpdateInfo();

	void setInfo(uint32 appver);

	virtual bool Show(bool show = true);
	virtual void Raise();

protected:
	wxStaticText* m_labInfo;
	gcMiscWebControl* m_ieBrowser;
	gcButton* m_butRestartNow;
	gcButton* m_butRestartLater;

	void onFormClose( wxCloseEvent& event );
	void onButClick( wxCommandEvent& event );

	void onPageLoad();
	void doRestart();

private:
};





#endif //DESURA_GCUPDATEFORM_H
