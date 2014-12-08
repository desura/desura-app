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

#ifndef DESURA_GCBUTTON_H
#define DESURA_GCBUTTON_H
#ifdef _WIN32
#pragma once
#endif


#include <wx/button.h>

#include "gcImageHandle.h"
#include "gcFlickerFreePaint.h"

enum STATE
{
	BS_NORMAL,
	BS_DEPRESSED,
	BS_HOVER,
	BS_TOGGLEON,
	BS_TOGGLEOFF,
	BS_DISABLED,
	BS_FOCUSED,
};

#ifdef WIN32
	typedef wxButton ButtonBase;
#else
	typedef wxPanel ButtonBase; 
#endif

//! A normal button with text that is in the desura theme and has no flickering painting
class gcButton : public gcFlickerFreePaint<ButtonBase> 
{
public:
	gcButton(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& text = wxString(""), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER|wxTAB_TRAVERSAL);
	gcButton();
	~gcButton();

	void Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& text = wxString(""), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER|wxTAB_TRAVERSAL);

	virtual bool Enable(bool enable = true);
	virtual void SetLabel(const wxString& label);

#ifdef WIN32
	virtual bool MSWOnDraw(WXDRAWITEMSTRUCT *item);
	virtual bool MSWCommand(WXUINT param, WXWORD WXUNUSED(id));
#else
	wxString GetLabel() const;
	void SetDefault();
	virtual wxSize DoGetBestSize() const;
#endif

protected:
	virtual void doPaint(wxDC* dc);

	void refreshImage(bool force = false);

	virtual STATE getState();
	virtual void setImage();

	void onMouseDown(wxMouseEvent& event);
	void onMouseUp(wxMouseEvent& event);
	void onMouseEvent(wxMouseEvent& event);

	void onBlur(wxFocusEvent& event);
	void onFocus(wxFocusEvent& event);

	void onMouseCaptureLost(wxMouseCaptureLostEvent &event);

	void updateMouse();

	void setImage(const char* image);
	bool isMouseWithinButton();

	void onChar(wxKeyEvent& event);

	const char *m_szImage;
	const char *m_szHoverImage;
	const char *m_szDepressedImage;
	const char *m_szDisabledImage;
	const char *m_szFocusImage;
	const char *m_szCurImage;

	gcImageHandle m_imgHandle;

	bool m_bShowDepressed;
	bool m_bHovering;
	bool m_bDepressed;
	bool m_bFocus;
	bool m_bEnabled;

	bool m_bClearBG;
	bool m_bTransEnabled;

	STATE getLastState();

private:
	STATE m_sLastState;

#ifdef WIN32
	void init(const wxSize& size);
#else
	void init(const wxSize& size, const wxString& text);
	wxString m_szText;
#endif
};

#endif
