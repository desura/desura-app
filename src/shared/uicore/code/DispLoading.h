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

#ifndef DESURA_DISPLOADING_H
#define DESURA_DISPLOADING_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcControls.h"
#include <wx/animate.h>

class DispLoading : public wxStaticText
{
public:
	DispLoading(wxWindow *parent, wxString text, const wxSize &size = wxDefaultSize);
	~DispLoading();

protected:
	gcImageHandle m_imgHandle;
	wxAnimationCtrl* m_imgAnimation;

	void onPaint( wxPaintEvent& event );
	void onEraseBG( wxEraseEvent& event );
	void onIdle(wxIdleEvent& event);

};

#endif //DESURA_DISPLOADING_H
