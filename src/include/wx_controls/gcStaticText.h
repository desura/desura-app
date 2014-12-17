/*
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)
Copyright (C) 2014 Bad Juju Games, Inc.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA.

Contact us at legal@badjuju.com.
*/

#ifndef DESURA_GCSTATICTEXT_H
#define DESURA_GCSTATICTEXT_H
#ifdef _WIN32
#pragma once
#endif

#include <wx/stattext.h>
#include "gcFlickerFreePaint.h"

//! Static text that doesnt flicker on repaint
class gcStaticText : public gcFlickerFreePaint<wxStaticText>
{
public:
	gcStaticText( wxWindow* parent, wxWindowID id = wxID_ANY, wxString text = wxString(L"Default"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxFULL_REPAINT_ON_RESIZE);

	virtual void SetLabel(const wxString& label);

protected:
	virtual void doPaint(wxDC* dc);

private:
	wxString m_szLastLabel;
};


#endif
