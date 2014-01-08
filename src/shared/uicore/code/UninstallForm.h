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

#ifndef DESURA_UNINSTALLFORM_H
#define DESURA_UNINSTALLFORM_H
#ifdef _WIN32
#pragma once
#endif

#include "gcControls.h"

class BasePage;

///////////////////////////////////////////////////////////////////////////////
/// Class UninstallForm
///////////////////////////////////////////////////////////////////////////////
class UninstallForm : public gcFrame 
{
public:
	UninstallForm(wxWindow* parent);
	~UninstallForm();

	void setInfo(DesuraId id);
	void run();

	void showInfo();
	void showProg(bool removeAll, bool removeAccount);

	DesuraId getItemId(){return m_uiInternId;}

	void setCanClose(){m_bCanClose = true;}

protected:
	BasePage* m_pPage;	
	wxBoxSizer* m_bsSizer;

	void cleanUpPages();
	void onClose( wxCloseEvent& event );

private:
	bool m_bCanClose;
	DesuraId m_uiInternId;

	DECLARE_EVENT_TABLE();
};

#endif //DESURA_UNINSTALLFORM_H
