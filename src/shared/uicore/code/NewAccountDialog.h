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


#ifndef DESURA_NEWACCOUNT_H
#define DESURA_NEWACCOUNT_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcDialog.h"
#include "gcMiscWebControl.h"

class gcButton;

class NewAccountDialog : public gcDialog 
{
public:
	NewAccountDialog(wxWindow* parent, const char* szProviderUrl = nullptr);
	
#ifdef NIX
	bool Show(const bool show = true);
#endif

protected:
	void onButtonClicked(wxCommandEvent& event);
	void onPageLoad();

	gcString GetTermsUrl();
	gcString GetRegisterUrl();

private:
	gcButton *m_butBack;
	gcButton *m_butAgree;
	gcButton *m_butCancel;

	gcMiscWebControl *m_pBrowser;

	gcString m_szProviderUrl;

	bool m_bTermsOfService;
};



#endif //DESURA_NEWACCOUNT_H
