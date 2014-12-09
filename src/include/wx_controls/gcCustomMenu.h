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

#ifndef DESURA_GCCUSTOMMENU_H
#define DESURA_GCCUSTOMMENU_H
#ifdef _WIN32
#pragma once
#endif

#include "wx/menu.h"

class MenuItemInfo;

//! Menu item that uses desura theme to paint it self (only on windows at this stage)
class gcMenuItem : public wxMenuItem
{
public:
	gcMenuItem(wxMenu *parentMenu = nullptr,
				int id = wxID_SEPARATOR,
				const wxString& name = wxEmptyString,
				const wxString& help = wxEmptyString,
				wxItemKind kind = wxITEM_NORMAL,
				wxMenu *subMenu = nullptr);

#ifdef WIN32
	// virtual functions to implement drawing (return true if processed)
	virtual bool OnMeasureItem(size_t *pwidth, size_t *pheight);
	virtual bool OnDrawItem(wxDC& dc, const wxRect& rc, wxODAction act, wxODStatus stat);
#endif

	virtual void setExtended()
	{
		m_bExtendedItems = true;
	}

private:
	bool m_bExtendedItems;
};

//! Menu that uses desura theme to paint it self (only on windows at this stage)
class gcMenu : public wxMenu
{
public:
	gcMenu(bool autoDel=false);

   // append any kind of item (normal/check/radio/separator)
	wxMenuItem* Append(int itemid, const wxString& text = wxEmptyString, const wxString& help = wxEmptyString, wxItemKind kind = wxITEM_NORMAL)
	{
		return DoAppend(new gcMenuItem(this, itemid, text, help, kind));
	}

	// append a separator to the menu
	wxMenuItem* AppendSeparator()
	{ 
		return Append(wxID_SEPARATOR); 
	}

	// append a check item
	wxMenuItem* AppendCheckItem(int itemid, const wxString& text, const wxString& help = wxEmptyString)
	{
		return Append(itemid, text, help, wxITEM_CHECK);
	}

	// append a radio item
	wxMenuItem* AppendRadioItem(int itemid, const wxString& text, const wxString& help = wxEmptyString)
	{
		return Append(itemid, text, help, wxITEM_RADIO);
	}

	// append a submenu
	wxMenuItem* AppendSubMenu(wxMenu *submenu, const wxString& text, const wxString& help = wxEmptyString)
	{
		return DoAppend(new gcMenuItem(this, wxID_ANY, text, help, wxITEM_NORMAL, submenu));
	}

	wxMenuItem* Append(wxMenuItem *item) 
	{ 
		return DoAppend(item); 
	}

	void Append(int itemid, const wxString& text, const wxString& help, bool isCheckable)
	{
		Append(itemid, text, help, isCheckable ? wxITEM_CHECK : wxITEM_NORMAL);
	}

	wxMenuItem* Append(int itemid, const wxString& text, wxMenu *submenu, const wxString& help = wxEmptyString)
	{
		return DoAppend(new gcMenuItem(this, itemid, text, help, wxITEM_NORMAL, submenu));
	}

	void layoutMenu();
};


#endif
