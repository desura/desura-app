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
#include "MenuFiller.h"

MenuFiller::MenuFiller(wxWindow *parent, const char* image, const wxSize& size) : gcFlickerFreePaint<gcPanel>(parent, wxID_ANY, wxDefaultPosition, size)
{
	SetMinSize(size);
	m_imgHandle = GetGCThemeManager()->getImageHandle(image);

	//Bind(wxEVT_ERASE_BACKGROUND, &MenuFiller::onEraseBG, this);
}

MenuFiller::~MenuFiller()
{
}

void MenuFiller::doPaint(wxDC* dc)
{
	if (!m_imgHandle.getImg() || !m_imgHandle->IsOk())
	{
		dc->SetTextForeground(wxColor(0,0,0));
		dc->Clear();
		return;
	}

	dc->SetTextBackground(GetBackgroundColour());
	dc->SetTextForeground(GetForegroundColour());


	wxSize sz = GetSize();
	wxMemoryDC memDC;


	uint32 w = sz.GetWidth();
	uint32 h = sz.GetHeight();

	wxBitmap temp(w, h);
	gcImage::tileImg(temp, wxBitmap(*m_imgHandle.getImg()));

	memDC.SelectObject(temp);

	dc->Blit(0, 0,			// Draw at (100, 100)
			w, h,			// Draw full bitmap
			&memDC,			// Draw from memDC
			0, 0,			// Draw from bitmap origin
			wxCOPY,			// Logical operation
			true);			// Take mask into account

	memDC.SelectObject(wxNullBitmap);
}
