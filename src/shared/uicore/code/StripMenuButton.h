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

#ifndef DESURA_STRIPMENUBUTTON_H
#define DESURA_STRIPMENUBUTTON_H
#ifdef _WIN32
#pragma once
#endif

#include "BaseMenuButton.h"
#include "wx_controls/gcImageHandle.h"
#include "wx_controls/gcFlickerFreePaint.h"

class StripMenuButton : public gcFlickerFreePaint<wxPanel>, public BaseMenuButton
{
public:
	StripMenuButton(wxWindow *parent, wxString text, const wxSize &size = wxDefaultSize);
	~StripMenuButton();

	void init(const char* image, const char* overlay = nullptr, int32 spacing = -1);

	void setOffset(uint32 val){m_uiOffset = val;}
	uint32 getOffset(){return m_uiOffset;}

	void setBold(bool state){m_bBold = state;}

	void setNormalCol(wxColor col){m_colNormal = col;SetForegroundColour( m_colNormal );}
	void setHoverCol(wxColor col){m_colHover = col;}

	wxString GetLabel();
	wxSize GetTextSize();

	void setColors(wxColor normal, wxColor hover);
	void showFocusBox(wxColor focusColor);

protected:
	void doPaint(wxDC* dc);

	void paintBGImage(wxDC* dc);
	void paintLabel(wxDC* dc);

	void onMouseDown( wxMouseEvent& event );
	void onMouseUp( wxMouseEvent& event );
	void onMouseOver( wxMouseEvent& event );
	void onMouseOut( wxMouseEvent& event );
	void onMouseEvent( wxMouseEvent& event );
	void onUpdateUI( wxUpdateUIEvent& event);

	void onBlur(wxFocusEvent& event);
	void onFocus(wxFocusEvent& event);

	void onSelected();
	void updateMouse();
	void sendClickCommand();

	virtual bool handleEvent(wxMouseEvent& event);

	gcImageHandle m_imgBg;
	gcImageHandle m_imgOverlay;

private:
	uint32 m_uiOffset;

	bool m_bHovering;
	bool m_bBold;
	bool m_bHasFocus;
	bool m_bUsesFocus;

	wxColor m_colHover;
	wxColor m_colNormal;
	wxColor m_colFocus;

	wxString m_szLabel;
};


#endif //DESURA_STRIPMENUBUTTON_H
