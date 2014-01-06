/*
Desura is the leading indie game distribution platform
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef DESURA_DESURASERVICEERROR_H
#define DESURA_DESURASERVICEERROR_H
#ifdef _WIN32
#pragma once
#endif

#include <branding/branding.h>
#include "wx_controls/gcControls.h"

class DSEThread;

class DesuraServiceError : public gcDialog 
{
public:
	DesuraServiceError(wxWindow* parent);
	~DesuraServiceError();

	void run();

	EventV onFinishEvent;
	Event<gcException> onErrorEvent;

	int ShowModal();

protected:
	friend class LanguageTestDialog;

	wxStaticText* m_labInfo;
	gcButton* m_butReconnect;
	gcButton* m_butLogout;

	void onButtonClicked( wxCommandEvent& event );
	
	void onFinish();
	void onError(gcException& e);

	DSEThread* m_pThread;

private:
	DECLARE_EVENT_TABLE();
};

#endif //DESURA_DESURASERVICEERROR_H
