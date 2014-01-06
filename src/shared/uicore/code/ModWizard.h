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

#ifndef DESURA_MODWIZARD_H
#define DESURA_MODWIZARD_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcControls.h"
#include "BasePage.h"
#include "ModWizardProgPage.h"
#include "ModWizardInfoPage.h"


///////////////////////////////////////////////////////////////////////////////
/// Class ModWizardForm
///////////////////////////////////////////////////////////////////////////////
class ModWizardForm : public gcFrame 
{
public:
	ModWizardForm(wxWindow* parent);
	~ModWizardForm();

	void run(bool addToAccount);
	void finish();

protected:
	void onFormClose( wxCloseEvent& event );
	void cleanUpPages();

private:
	BasePage* m_pPage;	
	wxBoxSizer* m_bsSizer;
};



#endif //DESURA_MODWIZARD_H
