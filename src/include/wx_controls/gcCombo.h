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


#ifndef DESURA_GCCOMBO_H
#define DESURA_GCCOMBO_H
#ifdef _WIN32
#pragma once
#endif

#include <wx/wx.h>

//! Combo box with desura theme
class gcComboBox : public wxComboBox
{
public:
	gcComboBox(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& value = L"", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, int n = 0, const wxString choices[] = nullptr, long style = 0);

	void applyTheme();
};

#endif //DESURA_GCCOMBO_H
