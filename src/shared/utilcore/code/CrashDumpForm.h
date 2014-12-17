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

#ifndef DESURA_CRASHDUMPFORM_H
#define DESURA_CRASHDUMPFORM_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcFrame.h"

class CrashDumpThread;
class gcButton;
class gcPanel;

class CrashDumpForm : public gcFrame
{
public:
	CrashDumpForm(wxWindow* parent);
	~CrashDumpForm();

	void pageOne();
	void pageTwo();
	void pageThree();

protected:
	gcButton* m_button1;
	gcButton* m_button2;
	gcButton* m_button3;
	gcPanel* m_pPanel;

	void onButtonClicked(wxCommandEvent &event);
	void onClose(wxCloseEvent &event);

	void onStart();
	void showFile();
	void upload();

	void onError(gcException& e);
	void onComplete(gcString& path);



private:
	bool m_bComplete;
	gcString m_szPath;

};


#endif //DESURA_CRASHDUMPFORM_H
