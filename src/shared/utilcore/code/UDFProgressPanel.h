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

#ifndef DESURA_UDFPROGRESSPANEL_H
#define DESURA_UDFPROGRESSPANEL_H
#ifdef _WIN32
#pragma once
#endif

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/gauge.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>

#include "wx_controls/gcControls.h"
#include "wx_controls/gcSpinnerProgBar.h"

namespace UserCore
{
	class UserI;
}

class UninstallAllThread;

class UDFProgressPanel : public gcPanel 
{
public:
	UDFProgressPanel(wxWindow* parent, UserCore::UserI* user, uint32 flags);
	~UDFProgressPanel();

protected:
	wxStaticText* m_labTotal;
	wxStaticText* m_labCurrent;

	gcSpinnerProgBar* m_pbTotal;
	gcSpinnerProgBar* m_pbCurrent;

	wxTextCtrl* m_tbInfo;

	void onProgress(std::pair<uint32, uint32> &prog);
	void onLog(gcString &msg);
	void onComplete();

private:
	UninstallAllThread* m_pThread;
};

#endif //DESURA_UDFPROGRESSPANEL_H
