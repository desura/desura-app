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
#include "gcProgressBar.h"

#include "gcImage.h"
#include "gcImageHandle.h"

#include "gcManagers.h"

gcProgressBar::gcProgressBar( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : gcPanel(parent, id, pos, size, wxSIMPLE_BORDER)
{
	SetBackgroundColour( wxColour( 125, 255, 125 ) );
 
	Connect(wxEVT_PAINT, wxPaintEventHandler(gcProgressBar::onPaint));
	Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler( gcProgressBar::onEraseBg ) );

	m_uiProg = 0;

	m_imgProg = GetGCThemeManager()->getImageHandle("#progressbar");
}

gcProgressBar::~gcProgressBar()
{

}

void gcProgressBar::setProgress(uint8 prog)
{
	uint8 oldProg = m_uiProg;
	m_uiProg = std::min(std::max((int)prog, 0), 100);

	if (m_uiProg != oldProg)
	{
		Refresh();
	}
}

void gcProgressBar::setCaption(const gcString& string)
{
	gcString oldCapt = m_szCaption;

	m_szCaption = gcString(string);

	if (m_szCaption != oldCapt)
		Refresh();
}

void gcProgressBar::onPaint( wxPaintEvent& event )
{
	wxPaintDC dc(this);

	if (GetSize().GetWidth() == 0 || GetSize().GetHeight() == 0)
		return;

	wxFont font(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier 10 Pitch"));
	dc.SetFont(font);
	dc.SetTextBackground(GetBackgroundColour());

	if (!m_imgProg.getImg() || !m_imgProg->IsOk())
	{
		doHandPaint(dc);
	}
	else
	{
		doImgPaint(dc);
	}

	if (!m_szCaption.empty())
	{
		wxSize textSize = dc.GetTextExtent(m_szCaption.c_str());

		uint32 w = GetSize().GetWidth();
		uint32 h = GetSize().GetHeight()-2;
		uint32 x = (w-textSize.GetWidth())/2;
		uint32 y = (h-textSize.GetHeight())/2;

		wxColour black(0, 0, 0);

		//dc.SetPen(wxPen(black,1)); 
		//dc.SetBrush(wxBrush(black));
		dc.SetTextForeground(black);
		dc.DrawText(m_szCaption.c_str(), x, y);
	}
}


void gcProgressBar::doHandPaint(wxPaintDC& dc)
{
	wxSize size = GetSize();
	uint32 w = size.GetWidth();
	uint32 h = size.GetHeight()-2;

	uint32 wp = w*m_uiProg/100;

	wxColour green(0, 255, 0);
	wxColour white(255, 255, 255);
	wxColour black(0, 0, 0);

	dc.SetPen(wxPen(green,1)); 
	dc.SetBrush(wxBrush(green));
	dc.DrawRectangle(0,0, wp, h);

	dc.SetPen(wxPen(white,1)); 
	dc.SetBrush(wxBrush(white));
	dc.DrawRectangle(wp,0, w, h);
}



void gcProgressBar::doImgPaint(wxPaintDC& dc)
{
	int h = GetSize().GetHeight();
	int w = GetSize().GetWidth();
#ifdef WIN32 // unused AFAIK
	int ih = m_imgProg->GetSize().GetHeight();
#endif
	int iw = m_imgProg->GetSize().GetWidth();

	uint32 wp = w*m_uiProg/100;

	wxImage scaled = m_imgProg->Scale(iw, h);
	wxBitmap norm = GetGCThemeManager()->getSprite(scaled, "progressbar", "Norm");
	wxBitmap nedge = GetGCThemeManager()->getSprite(scaled, "progressbar", "NormEdge");
	wxBitmap empty = GetGCThemeManager()->getSprite(scaled, "progressbar", "Empty");

	wxBitmap   tmpBmp(w, h);
	wxMemoryDC tmpDC(tmpBmp);

	tmpDC.SetBrush(wxBrush(wxColor(255,0,255)));
	tmpDC.SetPen( wxPen(wxColor(255,0,255),1) );
	tmpDC.DrawRectangle(0,0,w,h);


	uint32 neWidth = nedge.GetWidth();
	wxColor c(255,0,255);

	if (wp == 0)
	{
		//dont do any thing
	}
	else if (wp <= neWidth)
	{
		wxBitmap left = nedge.ConvertToImage().GetSubImage(wxRect(neWidth-wp,0,neWidth,h));
		tmpDC.DrawBitmap(left, 0, 0, true);
	}
	else
	{
		wxBitmap left(wp-neWidth, h);
		
		gcImage::tileImg(left, norm, &c);

		tmpDC.DrawBitmap(left, 0, 0, true);
		tmpDC.DrawBitmap(nedge, wp-neWidth, 0, true);
	}

	wxBitmap right(w-wp, h);
	gcImage::tileImg(right, empty, &c);
	tmpDC.DrawBitmap(right, wp,0,true);

	tmpDC.SelectObject(wxNullBitmap);
	dc.DrawBitmap(tmpBmp, 0,0, true);
}
