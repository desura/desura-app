/*
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
///////////// Copyright 2010 Desura Pty Ltd. All rights reserved. /////////////
//
//   Project     : uicore
//   File        : aboutPage_dev.cpp
//   Description :
//      [TODO: Write the purpose of aboutPage_dev.cpp.]
//
//   Created On: 9/6/2009 4:01:14 PM
//   Created By: Mark Chandler <mailto:mark@moddb.com>
////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "AboutPageDev.h"

typedef struct
{
	const char* title;
	const char* name;
	const char* tool;
} AboutInfo_s;

AboutInfo_s g_AboutInfo[] = 
{
//	{"Application Lead",		"Mark Chandler",	"Angry Drawf"},
//	{"Website Lead",			"Scott Reismanis",	"Slave Driver"},
//	{"Interface Design",		"Joshua Collie",	""},
//	#ifdef DESURA_OFFICIAL_BUILD
//	{"Logo && Typography",		"Tim Wilden",		""},
//	#else
//	{"Logo && Typography",		"notshi",		""},
//	#endif
//	{"Network Administrator",	"Greg Macsok",		"Destroyer of Sleep"},
//	{"Windows Game Guru",		"Dave Traeger",		""},
//	{"Linux Game Guru",			"Tim Jung",			""},


	{"",		"",	""},
	{"",			"",	""},
	{"",		"",	""},
	#ifdef DESURA_OFFICIAL_BUILD
	{"",		"",		""},
	#else
	{"",		"",		""},
	#endif
	{"",	"",		""},
	{"",		"",		""},
	{"",			"",			""},
};

AboutDevPage::AboutDevPage(wxWindow* parent) : gcPanel(parent, wxID_ANY)
{

	wxColor col = wxColor(GetGCThemeManager()->getColor("abouttext", "bg"));
	wxColor colInfo = wxColor(GetGCThemeManager()->getColor("abouttext", "fg"));

	m_labInfo = new wxStaticText( this, wxID_ANY, wxT("Thanks to all the indies and mods for distracting us. Without you Desura would have been released years ago."), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_labInfo1 = new wxStaticText( this, wxID_ANY, wxT("And countless others who have provided feedback and lived to tell the tale. Not tested on animals, only gamers."), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	
	m_labInfo->Wrap( 285 );
	m_labInfo1->Wrap( 285 );
	
	m_labInfo->SetForegroundColour( colInfo );
	m_labInfo1->SetForegroundColour( colInfo );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 7, 3, 0, 0 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	for (size_t x=0; x<7; x++)
	{
		wxStaticText* title = new wxStaticText( this, wxID_ANY, g_AboutInfo[x].title);
		wxStaticText* c = new wxStaticText( this, wxID_ANY, wxT(":"));
		wxStaticText* name = new wxStaticText( this, wxID_ANY, g_AboutInfo[x].name);

		name->SetToolTip(g_AboutInfo[x].tool);

		title->SetForegroundColour( col );
		c->SetForegroundColour( col );
		name->SetForegroundColour( col );

		fgSizer4->Add( title, 0, wxLEFT|wxALIGN_RIGHT, 5 );
		fgSizer4->Add( c, 0, wxRIGHT|wxLEFT, 5 );
		fgSizer4->Add( name, 0, wxRIGHT, 5 );
	}

	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer( 3, 1, 0, 0 );
	fgSizer8->AddGrowableCol( 0 );
	fgSizer8->AddGrowableRow( 1 );
	fgSizer8->SetFlexibleDirection( wxBOTH );
	fgSizer8->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	fgSizer8->Add( m_labInfo, 1, wxEXPAND|wxLEFT|wxRIGHT|wxBOTTOM, 5 );
	fgSizer8->Add( fgSizer4, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	fgSizer8->Add( m_labInfo1, 0, wxALL, 5 );
	
	this->SetSizer( fgSizer8 );
	this->Layout();
}

AboutDevPage::~AboutDevPage()
{
}
