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

#ifndef DESURA_GCUPPROGBAR_H
#define DESURA__GCUPPROGBAR_H
#ifdef _WIN32
#pragma once
#endif

#include <wx/wx.h>
#include <wx/control.h>

#include "gcProgressBar.h"

//! Progress bar for uploads that show an orange part for currently uploading section and blue bar for done
class gcULProgressBar : public gcProgressBar
{
public:
	gcULProgressBar(wxWindow *parent, int id = wxID_ANY );

	//! Sets the current percent to be the completed milestone
	void setMileStone();
	
	//! Reverts back to the last completed milestone
	void revertMileStone();

protected:
	void doHandPaint(wxPaintDC& dc) override;
	void doExtraImgPaint(wxBitmap &img, int w, int h) override;

private:
	uint8 m_uiLastMS;
};

#endif
