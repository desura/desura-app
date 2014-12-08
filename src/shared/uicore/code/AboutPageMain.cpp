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

#include "Common.h"
#include "AboutPageMain.h"

AboutMainPage::AboutMainPage(wxWindow* parent) : gcPanel(parent, wxID_ANY)
{

#ifndef DEBUG
	std::string szAppid = UTIL::OS::getConfigValue(APPID);
	std::string szAppBuild = UTIL::OS::getConfigValue(APPBUILD);
#else
	std::string szAppid("Debug");
	std::string szAppBuild(__DATE__);
#endif

	m_labBuild = new wxStaticText(this, wxID_ANY, gcWString(L"Build {0}.{1}", szAppid, szAppBuild), wxDefaultPosition, wxDefaultSize, 0 );
	m_labBuild->Wrap( 270 );
	m_labInfo = new wxStaticText( this, wxID_ANY, Managers::GetString(L"#AB_ABOUTINFO"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labInfo->Wrap( 270 );

	gcWString copy("Desurium is open source software LGPL v2.1. \nGet source from: http://github.com/lindenlab/desura-app");

	m_labCopyRight = new wxStaticText( this, wxID_ANY, copy.c_str(), wxDefaultPosition, wxSize(-1, 85), 0 );
	m_labCopyRight->Wrap( 270 );


	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 4, 1, 0, 0 );
	fgSizer7->AddGrowableCol( 0 );
	fgSizer7->AddGrowableRow( 1 );
	fgSizer7->SetFlexibleDirection( wxBOTH );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	fgSizer7->Add( m_labBuild, 0, wxLEFT|wxRIGHT, 5 );
	fgSizer7->Add( m_labInfo, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	fgSizer7->Add(5,0,0,0,0);
	fgSizer7->Add( m_labCopyRight, 0, wxLEFT|wxRIGHT|wxEXPAND, 5 );

	this->SetSizer( fgSizer7 );
	this->Layout();

	m_labBuild->SetForegroundColour(wxColor(GetGCThemeManager()->getColor("abouttext", "fg")));
	m_labInfo->SetForegroundColour(wxColor(GetGCThemeManager()->getColor("abouttext", "fg")));
	m_labCopyRight->SetForegroundColour(wxColor(GetGCThemeManager()->getColor("abouttext", "fg")));
}

AboutMainPage::~AboutMainPage()
{
}
