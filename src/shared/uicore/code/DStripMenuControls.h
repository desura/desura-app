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

#ifndef DESURA_DSTRIPMENUCONTROLS_H
#define DESURA_DSTRIPMENUCONTROLS_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcControls.h"
#include "StripMenuButton.h"
#include "MenuSeperator.h"

class DStripMenuButton : public StripMenuButton
{
public:
	DStripMenuButton(wxWindow *parent, const char* label, wxSize size = wxDefaultSize);
	void setActive(bool state);

private:
	wxRect m_IgnoreRect;
	bool m_bMouseDown;

	wxColor m_NormColor;
	wxColor m_NonActiveColor;
};

class DMenuSeperator : public MenuSeperator
{
public:
	DMenuSeperator(wxWindow *parent, const wxSize &size = wxDefaultSize) 
		: MenuSeperator(parent, "#menu_separator", size)
	{
	}

	void setActive(bool state)
	{
		if (!isSelected())
			m_imgBG->setImage(state?"#menu_separator":"#menu_separator_nonactive");
	}
};

#endif //DESURA_DSTRIPMENUCONTROLS_H
