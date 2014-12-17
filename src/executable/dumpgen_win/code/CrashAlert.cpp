/*
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

// CrashAleart.cpp : implementation file
//

#include "CrashAlert.h"

using namespace Desurium;


// CrashAleart
CrashAlert::CrashAlert()
	: CDesuraDialog(IDD_DIALOG1)
{
	m_hIcon = LoadIcon(IDI_ICON1);
}

CrashAlert::~CrashAlert()
{
}

bool CrashAlert::OnInitDialog()
{
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	return true;
}

void CrashAlert::OnCommand(HWND hWnd, int nId)
{
	EndDialog(nId);
}
