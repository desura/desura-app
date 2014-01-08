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

#ifndef DESURA_UDFSETTINGSPANEL_H
#define DESURA_UDFSETTINGSPANEL_H
#ifdef _WIN32
#pragma once
#endif


#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/panel.h>


enum UDF_FLAGS
{
	REMOVE_SIMPLE		= 1<<0,
	REMOVE_CACHE		= 1<<1,
	REMOVE_SETTINGS		= 1<<2,
};

class UDFSettingsPanel : public wxPanel 
{
public:
	UDFSettingsPanel(wxWindow* parent);
	~UDFSettingsPanel();

	uint32 getFlags();

protected:
	wxStaticText* m_labUninstallInfo;
	wxStaticText* m_labUninstallTitle;
	wxStaticText* m_labULocTitle;
	wxTextCtrl* m_tbUninstallLoc;

	wxCheckBox* m_cbRemoveDesura;
	wxCheckBox* m_cbRemoveSettings;
	wxCheckBox* m_cbRemoveCache;
	wxCheckBox* m_cbRemoveSimple;
	wxCheckBox* m_cbRemoveComplex;
		
	
private:
	
};

#endif //DESURA_UDFSETTINGSPANEL_H
