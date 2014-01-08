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

#ifndef DESURA_TABPANEL_H
#define DESURA_TABPANEL_H
#ifdef _WIN32
#pragma once
#endif

#include "TabHandler.h"

class TabPanelI
{
public:
	virtual void setTabHandler(TabHandler* tb)=0;
	virtual TabHandler* getTabHandler()=0;

	virtual void onKeyDown( wxKeyEvent& event )=0;

	virtual void onLeft()=0;
	virtual void onRight()=0;

	virtual void setFocus()=0;
};

template <class T>
class TabPanel : public TabPanelI, public T 
{
public:
	TabPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxWANTS_CHARS )
		: T( parent, id, pos, size, style )
	{
		m_pTabHandler = nullptr;
	}


	TabPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = "", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxWANTS_CHARS )
		: T( parent, id, title, pos, size, style )
	{
		m_pTabHandler = nullptr;
	}

	~TabPanel()
	{
	}

	void setTabHandler(TabHandler* tb)
	{
		m_pTabHandler = tb;
	}

	TabHandler* getTabHandler()
	{
		return m_pTabHandler;
	}


	virtual void onKeyDown( wxKeyEvent& event )
	{
		if (m_pTabHandler)
			m_pTabHandler->onKeyDown(this, event);
		event.Skip();
	}
	
	virtual void onLeftDown( wxMouseEvent& event )
	{
		SetFocus();
		event.Skip();
	}


	virtual void onLeft(){}
	virtual void onRight(){}

	void setFocus()
	{
		SetFocus();
	}	

private:
	TabHandler* m_pTabHandler;



private:
	DECLARE_EVENT_TABLE();
};







template <class T>
const wxEventTable TabPanel<T>::sm_eventTable = 
{ 
	&T::sm_eventTable, 
	&TabPanel<T>::sm_eventTableEntries[0] 
};

template <class T>
const wxEventTable *TabPanel<T>::GetEventTable() const
{ 
	return &TabPanel<T>::sm_eventTable; 
} 

template <class T>
wxEventHashTable TabPanel<T>::sm_eventHashTable(TabPanel<T>::sm_eventTable);

template <class T>
wxEventHashTable &TabPanel<T>::GetEventHashTable() const
{ 
	return TabPanel<T>::sm_eventHashTable; 
}

template <class T>
const wxEventTableEntry TabPanel<T>::sm_eventTableEntries[] = 
{
	EVT_KEY_DOWN( TabPanel<T>::onKeyDown )
	EVT_LEFT_DOWN( TabPanel<T>::onLeftDown )
	wxEventTableEntry(wxEVT_NULL, 0, 0, 0, 0)
};


#endif //DESURA_TABPANEL_H
