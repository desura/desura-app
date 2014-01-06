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

#ifndef DESURA_PASSREMINDER_H
#define DESURA_PASSREMINDER_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcControls.h"


class PassReminderThread;

class PasswordReminder : public gcDialog
{
public:
	PasswordReminder(wxWindow* parent);
	~PasswordReminder();
	
#ifdef NIX
	bool Show(const bool show = true);
#endif
protected:
	void onTextBoxEnter( wxCommandEvent& event );
	void onButtonClicked( wxCommandEvent& event );
	bool isEmailValid();

	void onComplete();
	void onError(gcException& e);

private:
	wxStaticText* m_staticText1;
	wxStaticText* m_staticText2;

	gcTextCtrl* m_tbEmail;
	
	gcButton* m_butSend;
	gcButton* m_butCancel;

	PassReminderThread *m_pThread;
};

#endif //DESURA_PASSREMINDER_H
