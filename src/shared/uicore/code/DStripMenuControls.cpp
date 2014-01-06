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

#include "Common.h"
#include "DStripMenuControls.h"
#include "wx_controls/gcManagers.h"
#include "managers/CVar.h"

CVar gc_buttontol("gc_butMouseTol", "16", CFLAG_USER);

DStripMenuButton::DStripMenuButton(wxWindow *parent, const char* label, wxSize size) 
	: StripMenuButton(parent, gcWString(label).c_str(), size)
{
	init("#menu_bg", "#menu_overlay");
	m_bMouseDown = false;

	m_NormColor = this->GetForegroundColour();
	m_NonActiveColor = wxColor(GetGCThemeManager()->getColor("label", "na-fg"));
}

void DStripMenuButton::setActive(bool state)
{
	setNormalCol(state?m_NormColor:m_NonActiveColor);

	if (!isSelected())
		m_imgBg = GetGCThemeManager()->getImageHandle(state?"#menu_bg":"#menu_bg_nonactive");

	this->invalidatePaint();
}
