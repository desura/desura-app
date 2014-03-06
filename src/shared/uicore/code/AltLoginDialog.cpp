/*
Desura is the leading indie game distribution platform
Copyright (C) 2013 Mark Chandler (Lindenlab)

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
#include "AltLoginDialog.h"
#include "MainApp.h"

AltLoginDialog::AltLoginDialog(wxWindow* parent, const char* szProvider, const char* szApiUrl)
	: gcDialog(parent, wxID_ANY, wxT("#ALTLOGIN_TITLE"), wxDefaultPosition, wxSize(500, 665), wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL) 
{
	SetTitle(gcString(Managers::GetString(L"#ALTLOGIN_TITLE"), szProvider));

	GetWebCore();

	gcString strUrl;
	
	if (!gcString(szApiUrl).empty())
		strUrl = gcString("http://www.{0}/members/loginext/{1}", szApiUrl, szProvider);
	else
		strUrl = gcString("http://www.desura.com/members/loginext/{0}", szProvider);

	m_pBrowser = new gcWebControl(this, strUrl.c_str());

	wxFlexGridSizer* fgSizer2 = new wxFlexGridSizer(2, 1, 0, 0);
	fgSizer2->AddGrowableCol(0);
	fgSizer2->AddGrowableRow(0);
	fgSizer2->SetFlexibleDirection(wxBOTH);
	fgSizer2->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	fgSizer2->Add(m_pBrowser, 1, wxEXPAND | wxTOP | wxRIGHT | wxLEFT, 5);

	this->SetSizer(fgSizer2);
	this->Layout();

	gcString strProvider(szProvider);

	if (strProvider == "google")
		SetSize(wxSize(900, 525));
	else if (strProvider == "facebook")
		SetSize(wxSize(1050, 600));
	else if (strProvider == "steam")
		SetSize(wxSize(1000, 850));

	CenterOnParent();
}