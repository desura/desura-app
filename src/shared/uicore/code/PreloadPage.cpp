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
#include "PreloadPage.h"


namespace UI
{
namespace Forms
{
namespace ItemFormPage
{


PreloadPage::PreloadPage(wxWindow* parent, const char* action, const char* id) : BaseInstallPage(parent)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	gcWString lab;

	if (!action || !id)
		lab = gcWString(Managers::GetString(L"#IF_PREPARE_INFO_NOITEM"));
	else
		lab = gcWString(Managers::GetString(L"#IF_PREPARE_INFO"), action, id);

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 4, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableRow( 2 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	gcStaticText *labInfo = new gcStaticText(this, wxID_ANY, lab, wxDefaultPosition, wxDefaultSize, 0 );
	gcSpinningBar *pbProgress = new gcSpinningBar(this, wxID_ANY, wxDefaultPosition, wxSize(-1,22 ));

	fgSizer1->Add( labInfo, 0, wxLEFT|wxRIGHT|wxTOP, 5 );
	fgSizer1->Add( pbProgress, 1, wxEXPAND|wxALL, 5 );
	fgSizer1->Add(0, 0, 0, wxEXPAND, 5);
	
	this->SetSizer( fgSizer1 );
	this->Layout();
	this->setParentSize(-1, 140);
}

void PreloadPage::onError(gcException &e)
{
	this->GetParent()->Close();
}

}
}
}