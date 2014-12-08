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
#include "gcStaticText.h"
#include "gcManagers.h"

gcStaticText::gcStaticText( wxWindow* parent, wxWindowID id , wxString text, const wxPoint& pos, const wxSize& size, long style) : gcFlickerFreePaint<wxStaticText>( parent, id, text, pos, size, style|wxTRANSPARENT_WINDOW )
{
	m_szLastLabel = text;
	Managers::LoadTheme(this, "label");
}

void gcStaticText::doPaint(wxDC* dc)
{
	wxColor bg = GetParent()->GetBackgroundColour();
	wxColor fg = GetForegroundColour();

	dc->SetFont(GetFont());
	dc->SetTextForeground(fg);
	dc->SetTextBackground(bg);

	wxSize size = GetSize();
	long style = GetWindowStyle();

	long x=0;
	long y=0;

	if (bg.Alpha() != 0)
	{
		dc->SetPen(wxPen(bg)); 
		dc->SetBrush(wxBrush(bg));
		dc->DrawRectangle(size);
	}

	if (style & wxALIGN_RIGHT)
	{
		x = size.GetWidth() - dc->GetTextExtent(GetLabel()).GetWidth();
	}
	else if (style & wxALIGN_CENTER)
	{
		x = (size.GetWidth() - dc->GetTextExtent(GetLabel()).GetWidth())/2;
	}

	if (style & wxALIGN_CENTER_VERTICAL)
	{
		y = (size.GetHeight() - dc->GetTextExtent(GetLabel()).GetHeight())/2;
	}

	dc->DrawText(this->GetLabel(), x, y);
}

void gcStaticText::SetLabel(const wxString& label)
{
	if (m_szLastLabel == label)
		return;

	m_szLastLabel = label;

	gcFlickerFreePaint<wxStaticText>::SetLabel(label);
	this->invalidatePaint();
}
