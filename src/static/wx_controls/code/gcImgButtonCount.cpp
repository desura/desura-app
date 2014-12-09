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
#include "gcImgButtonCount.h"
#include "gcImage.h"
#include "gcManagers.h"

gcImgButtonCount::gcImgButtonCount( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : gcImageButton( parent, id, pos, size, style )
{
	m_uiCount = 0;
	m_imgCount = GetGCThemeManager()->getImageHandle("#count_background");
}

gcImgButtonCount::~gcImgButtonCount()
{
}

void gcImgButtonCount::doPaint(wxDC* dc)
{
	gcImageButton::doPaint(dc);

	if (m_uiCount == 0 || !m_imgCount.getImg() || !m_imgCount->IsOk())
	{
		return;
	}

	gcWString text(L"+");

	if (m_uiCount < 10)
	{
		text = gcWString(L"{0}", m_uiCount);
		wxFont font(6, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
		dc->SetFont(font);
	}
	else
	{
		wxFont font(6, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
		dc->SetFont(font);
	}

	if (isHovering())
		dc->SetTextForeground(wxColor(GetGCThemeManager()->getColor("countbubble", "hov-fg")));
	else
		dc->SetTextForeground(wxColor(GetGCThemeManager()->getColor("countbubble", "fg")));


	wxSize sz = GetSize();

	int x = sz.GetWidth()-15;
	int y = 4;

	int w = m_imgCount->GetWidth();
	int h = m_imgCount->GetHeight();

	int tx = x+(w - dc->GetTextExtent(text).GetWidth())/2;
	int ty = y+(h - dc->GetTextExtent(text).GetHeight())/2;

	wxBitmap temp(m_imgCount->Copy());

	wxMemoryDC memDC(temp);

	dc->Blit(x, y,			// Draw at (100, 100)
			w, h,			// Draw full bitmap
			&memDC,			// Draw from memDC
			0, 0,			// Draw from bitmap origin
			wxCOPY,			// Logical operation
			true);			// Take mask into account

	memDC.SelectObject(wxNullBitmap);

	dc->DrawText(text, tx, ty);
}

void gcImgButtonCount::refreshImage()
{	
	gcImageButton::refreshImage();

	if (isHovering())
		m_imgCount = GetGCThemeManager()->getImageHandle("#count_background_hover");
	else
		m_imgCount = GetGCThemeManager()->getImageHandle("#count_background");
}
