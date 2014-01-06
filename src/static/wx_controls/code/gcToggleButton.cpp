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
#include "gcToggleButton.h"

gcToggleButton::gcToggleButton( wxWindow* parent, wxWindowID id, const wxString& text, const wxPoint& pos, const wxSize& size, long style ) : gcButton( parent, id, text, pos, size, style )
{
	m_szToggleOn = ("#button_toggle_up");
	m_szToggleOff = ("#button_toggle_down");

	m_bToggled = false;

	Bind(wxEVT_LEFT_DOWN, &gcToggleButton::onMouseDown, this);

	m_TextColor = GetForegroundColour();

	refreshImage();
}

gcToggleButton::~gcToggleButton()
{

}

STATE gcToggleButton::getState()
{
	STATE state = gcButton::getState();

	if (state != BS_DISABLED)
	{
		if (m_bToggled)
			return BS_TOGGLEON;
		else
			return BS_TOGGLEOFF;
	}

	return state;
}

void gcToggleButton::setImage()
{
	switch (getLastState())
	{
	case BS_TOGGLEON:	
		gcButton::setImage(m_szToggleOn);		
		break;

	case BS_TOGGLEOFF:	
		gcButton::setImage(m_szToggleOff);		
		break;

	default:			
		gcButton::setImage();				
		break;
	};

	if (m_bToggled)
		SetForegroundColour(m_TextColor);
	else
		SetForegroundColour(wxColour(160,160,160,255));
}

void gcToggleButton::onMouseDown( wxMouseEvent& event )
{
	if (IsEnabled())
	{
		m_bToggled = !m_bToggled;
		refreshImage();
	}

	event.Skip();
}