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
///////////// Copyright 2010 DesuraNet. All rights reserved. /////////////
//
//   Project     : dumpgen
//   File        : CrashAlert.h
//   Description :
//      [TODO: Write the purpose of CrashAlert.h.]
//
//   Created On: 9/24/2010 7:37:23 PM
//   Created By:  <mailto:>
////////////////////////////////////////////////////////////////////////////

#ifndef DESURA_CRASHALERT_H
#define DESURA_CRASHALERT_H
#ifdef _WIN32
#pragma once
#endif

#include "Resource.h"
#include "DesuraWnd.h"

// CrashAlert frame

class CrashAlert : public Desurium::CDesuraDialog
{
public:
	CrashAlert();
	virtual ~CrashAlert();

protected:
	void OnCommand(HWND hWnd, int nId) override;
	bool OnInitDialog() override;

private:
	HICON m_hIcon;

public:
	void OnBnClickedYes();
};

#endif //DESURA_CRASHALERT_H






