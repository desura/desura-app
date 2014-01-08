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

#ifndef DESURA_GCIMGBUTTONCOUNT_H
#define DESURA_GCIMGBUTTONCOUNT_H
#ifdef _WIN32
#pragma once
#endif

#include "gcImageButton.h"
#include "gcImageHandle.h"
class gcImage;

//! An image button that also shows a count from 0 to 9 or + if greater than 9
class gcImgButtonCount : public gcImageButton
{
public:
	gcImgButtonCount( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER|wxTAB_TRAVERSAL );
	~gcImgButtonCount();

	void setCount(uint8 count)
	{
		m_uiCount = count;
		invalidatePaint();
	}

protected:
	virtual void doPaint(wxDC* dc);
	void refreshImage();

private:
	uint8 m_uiCount;
	gcImageHandle m_imgCount;
};



#endif //DESURA_GCIMGBUTTONCOUNT_H
