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
#include "gcSpinnerProgBar.h"



gcSpinnerProgBar::gcSpinnerProgBar(wxWindow* parent, wxWindowID id,const wxPoint& pos, const wxSize& size) : gcPanel(parent, id, pos, size)
{
	m_sbSpinner = new gcSpinningBar(this, wxID_ANY, wxDefaultPosition, size);
	m_pbProgress = new gcProgressBar(this, wxID_ANY, wxDefaultPosition, size);
	
	m_pProgSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pProgSizer->Add(m_sbSpinner, 1, wxEXPAND, 5);

	m_pbProgress->Show(false);
	m_bSwapped = false;

	this->SetSizer(m_pProgSizer);
	this->Layout();
}

void gcSpinnerProgBar::swap()
{
	m_bSwapped = true;
		
	m_pbProgress->Show(true);
	m_sbSpinner->Show(false);

	m_pProgSizer->Clear(false);
	m_pProgSizer->Add(m_pbProgress, 1, wxEXPAND, 5);

	this->Layout();
}

void gcSpinnerProgBar::setProgress(uint8 prog)
{
	if (!m_bSwapped && prog > 0)
		swap();

	m_pbProgress->setProgress(prog);
}

uint8 gcSpinnerProgBar::getProgress()
{
	return m_pbProgress->getProgress();
}

void gcSpinnerProgBar::setCaption(const gcString& string)
{
	m_pbProgress->setCaption(string);
}