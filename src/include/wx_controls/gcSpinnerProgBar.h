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

#ifndef DESURA_GCSPINNERPROGBAR_H
#define DESURA_GCSPINNERPROGBAR_H
#ifdef _WIN32
#pragma once
#endif

#include "gcSpinningBar.h"
#include "gcProgressBar.h"

//! A combination of a spinner bar and progress bar. When progress == 0 it spins other wise it shows progress.
class gcSpinnerProgBar : public gcPanel
{
public:
	gcSpinnerProgBar(wxWindow* parent, wxWindowID id = wxID_ANY,const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	//! Force a swap between spinner and progress
	//!
	void swap();

	void setProgress(uint8 prog);
	uint8 getProgress();
	
	//! Set the text that shows in the middle of the progress bar
	//!
	void setCaption(const gcString& string);

protected:
	gcSpinningBar *m_sbSpinner;
	gcProgressBar *m_pbProgress;

	wxBoxSizer *m_pProgSizer;
	bool m_bSwapped;
};


#endif //DESURA_GCSPINNERPROGBAR_H
