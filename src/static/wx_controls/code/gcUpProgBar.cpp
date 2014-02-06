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
#include "gcUpProgBar.h"
#include "gcManagers.h"





gcULProgressBar::gcULProgressBar(wxWindow *parent, int id) : gcProgressBar(parent, id, wxDefaultPosition, wxSize( -1,22 ))
{
	m_uiLastMS = 0;
}

void gcULProgressBar::setMileStone()
{
	m_uiLastMS = getProgress();
	Refresh();
}

void gcULProgressBar::revertMileStone()
{
	setProgress( m_uiLastMS );
	Refresh();
}


void gcULProgressBar::doHandPaint(wxPaintDC& dc)
{
	wxSize size = GetSize();
	uint32 w = size.GetWidth();
	uint32 h = size.GetHeight()-2;

	uint32 wm = w*m_uiLastMS/100;
	uint32 wp = w*getProgress()/100;

	wxColour red(255, 0, 0);
	wxColour green(0, 255, 0);
	wxColour white(255, 255, 255);
	wxColour black(0, 0, 0);

	dc.SetPen(wxPen(green,1)); 
	dc.SetBrush(wxBrush(green));
	dc.DrawRectangle(0,0, wm, h);

	dc.SetPen(wxPen(red,1)); 
	dc.SetBrush(wxBrush(red));
	dc.DrawRectangle(wm,0, wp, h);

	dc.SetPen(wxPen(white,1)); 
	dc.SetBrush(wxBrush(white));
	dc.DrawRectangle(wp,0, w, h);
}

void gcULProgressBar::doExtraImgPaint(wxBitmap &img, int w, int h)
{
	int iw = getImage()->GetSize().GetWidth();

	uint32 wm = w*m_uiLastMS/100;
	uint32 wp = 0;
	
	uint32 t = w*getProgress()/100;

	if (t > wm)
		wp = t - wm;
	
	if (wp == 0)
		return;

	wxImage scaled = getImage()->Scale(iw, h);
	wxBitmap err = GetGCThemeManager()->getSprite(scaled, "progressbar", "Error");
	wxBitmap eedge = GetGCThemeManager()->getSprite(scaled, "progressbar", "ErrorEdge");

	wxBitmap   tmpBmp(wp, h);
	wxMemoryDC tmpDC(tmpBmp);

	tmpDC.SetBrush(wxBrush(wxColor(255,0,255)));
	tmpDC.SetPen( wxPen(wxColor(255,0,255),1) );
	tmpDC.DrawRectangle(0,0,w,h);

	uint32 eeWidth = eedge.GetWidth();

	if (wp <= eeWidth)
	{
		wxBitmap left = eedge.ConvertToImage().GetSubImage(wxRect(eeWidth-wp,0,eeWidth,h));
		tmpDC.DrawBitmap(left, 0, 0, true);
	}
	else
	{
		wxBitmap left(wp-eeWidth, h);
		wxColor c(255,0,255);
		gcImage::tileImg(left, err, &c);

		tmpDC.DrawBitmap(left, 0, 0, true);
		tmpDC.DrawBitmap(eedge, wp-eeWidth, 0, true);
	}

	tmpDC.SelectObject(img);
	tmpDC.DrawBitmap(tmpBmp, wm, 0, true);
	tmpDC.SelectObject(wxNullBitmap);
}
