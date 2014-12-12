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

#include "Common.h"
#include "BaseTabPage.h"

#include "Managers.h"

BaseTabPage::BaseTabPage( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : gcPanel( parent, id, pos, size, style )
{
	SetBackgroundColour( wxColor(GetGCThemeManager()->getColor("tabpage","bg")) );

	m_iProgress = 0;
	m_bShowProg = false;
}

BaseTabPage::~BaseTabPage()
{

}

void BaseTabPage::setSelected(bool state)
{
	Show(state);
}

void BaseTabPage::setProgress(const char* caption, int32 prog)
{
	m_szProgCap = caption;
	m_iProgress = prog;

	if (prog < 0 || prog > 100)
		m_bShowProg = false;
	else
		m_bShowProg = true;

	uint32 id = this->GetId();
	progressUpdateEvent(id);
}
