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

#ifndef DESURA_GCIMAGEBUTTON_H
#define DESURA_GCIMAGEBUTTON_H
#ifdef _WIN32
#pragma once
#endif

#include "gcImageControl.h"

//! Button that shows an image
class gcImageButton : public gcImageControl 
{
public:
	gcImageButton( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL|wxFULL_REPAINT_ON_RESIZE );
	~gcImageButton();

	//! Sets the normal button state image path
	//!
	void setDefaultImage(const char* img);

	//! Sets the mouse hover button state image path
	//!	
	void setHoverImage(const char* img);

	//! Sets the focused button state image path
	//!	
	void setFocusImage(const char* img);

	//! Sets the disabled button state image path
	//!	
	void setDisabledImage(const char* img);

	bool Enable(bool enable = true);

	void setImage(const char* image);

	bool isHovering(){return m_bHovering;}

protected:
	virtual void refreshImage();

	virtual void onMouseDown( wxMouseEvent& event );
	virtual void onMouseUp( wxMouseEvent& event );
	virtual void onMouseOver( wxMouseEvent& event );
	virtual void onMouseOut( wxMouseEvent& event );
	virtual void onMouseEvent( wxMouseEvent& event );
	virtual void onUpdateUI( wxUpdateUIEvent& event);
	virtual void onMouseCaptureLost(wxMouseCaptureLostEvent &event);

	virtual void updateMouse();
	virtual void updateMouse(wxMouseEvent& event);
	virtual void doPaint(wxDC* dc);

	virtual bool mouseOverButton(wxMouseEvent& event);

	bool m_bHovering;
	bool m_bMouseDown;
	bool m_bMouseDownAndOver;

private:
	gcString m_szImage;
	gcString m_szHoverImage;
	gcString m_szFocusImage;
	gcString m_szDisabledImage;
	gcString m_szCurImage;
};

#endif //DESURA_GCIMAGEBUTTON_H
