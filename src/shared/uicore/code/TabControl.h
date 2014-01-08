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

#ifndef DESURA_TabControl_H
#define DESURA_TabControl_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcControls.h"
#include "TabButton.h"
///////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
/// Class tabPage
///////////////////////////////////////////////////////////////////////////////
class TabControl : public gcPanel 
{
public:
	TabControl( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
	~TabControl();

	void addPage(const char* tabName, int32 id);
	void clearAll();
	void setActivePage(uint32 index);
	void setActivePage_ID(int32 index);

	Event<int32> onSelectPageEvent;

	tabButton* getTab(int32 id);

protected:
	wxBoxSizer* bSizerTabButtons;

private:
	std::vector<tabButton*> m_vTabInfo;
	uint32 m_iIndex;
};

#endif //DESURA_TabControl_H
