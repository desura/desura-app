/*
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
