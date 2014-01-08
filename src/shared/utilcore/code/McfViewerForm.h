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

#ifndef DESURA_MCFVIEWERFORM_H
#define DESURA_MCFVIEWERFORM_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcFrame.h"
#include "wx_controls/gcButton.h"

#include "mcfcore/MCFMain.h"

#include <wx/sizer.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/treectrl.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/filedlg.h>
#include <wx/menu.h>



///////////////////////////////////////////////////////////////////////////////
/// Class OpenMCF_Form
///////////////////////////////////////////////////////////////////////////////
class McfViewerForm : public gcFrame 
{
public:
	McfViewerForm(wxWindow* parent, gcString mcf);
	~McfViewerForm();

	void loadMcf(const char* path);

protected:
	void onButtonClicked(wxCommandEvent& event);
	void onClose(wxCloseEvent &event);

	wxTreeItemId recCreateNodes(const wchar_t* path, size_t pathSize, wxTreeItemId lastNode, bool search = true);
	wxTreeItemId FindNode(const wchar_t* str, wxTreeItemId lastNode);

private:
	McfHandle m_pHandle;

	wxTreeCtrl* m_tcFileTree;

	wxStaticText* m_labId;
	wxStaticText* m_labVersion;
	wxStaticText* m_labType;
	wxStaticText* m_labMCFVers;
	wxStaticText* m_labPatch;
	wxStaticText* m_labBranch;

	gcButton* m_butOpen;
};


#endif //DESURA_MCFVIEWERFORM_H
