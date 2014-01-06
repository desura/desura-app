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

#ifndef DESURA_HEADERBUTTON_H
#define DESURA_HEADERBUTTON_H
#ifdef _WIN32
#pragma once
#endif


#include "wxControls/gcControls.h"
#include "wxControls/gcImageHandle.h"

///////////////////////////////////////////////////////////////////////////////
/// Class headerButton
///////////////////////////////////////////////////////////////////////////////
class headerButton : public gcPanel 
{
public:
	headerButton( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 267,42 ), long style = wxTAB_TRAVERSAL );
	~headerButton();

	void setText(const char* text);

	void inverseSortDir();

	void setSelected(bool state);
	bool isSelected(){return m_bSelected;}

	uint8 getSortMode(){return m_iSortMode;}
	void setSortMode(uint8 mode){m_iSortMode = mode;}

	Event<int32> onClickedEvent;

	void setTextColor(wxColor& col){m_labTitle->SetForegroundColour(col);}

protected:
	gcStaticText* m_labTitle;
	gcImageControl* m_imgSortDir;
	
	void OnMouseClick( wxMouseEvent& event );
	
private:
	uint8 m_iSortMode;
	bool m_bSelected;
};

#endif //DESURA_HEADERBUTTON_H
