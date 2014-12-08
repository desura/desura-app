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

#ifndef DESURA_GCMESSAGEBOX_H
#define DESURA_GCMESSAGEBOX_H
#ifdef _WIN32
#pragma once
#endif

#include "gcControls.h"
#include "gcDialog.h"
#include <branding/branding.h>

enum
{
	gcID_YES = 1000,
	gcID_OK,
	gcID_NO,
	gcID_CANCEL,
	gcID_APPLY,
	gcID_CLOSE,
};

//! Allows extra buttons to be added to the message box
class HelperButtonsI
{
public:
	//! Get number of extra buttons
	//!
	virtual uint32 getCount()=0;

	//! Get the label for button at index
	//!
	virtual const wchar_t* getLabel(uint32 index)=0;

	//! Get the tool tip for button at index. Can be nullptr
	//!
	virtual const wchar_t* getToolTip(uint32 index)=0;

	//! Perform action for button at index
	//!
	virtual void performAction(uint32 index)=0;
};

//! Shows an error message box. Title and prompt can be language strings (starting with #) that will be looked up
//!
void gcErrorBox(wxWindow *parent, const char* title, const char* prompt, const gcException &e, HelperButtonsI* helper=nullptr);

//! Shows a message box
int gcMessageBox(wxWindow *parent, const wxString& message, const wxString& caption = wxT("Desura"), long style = wxICON_EXCLAMATION|wxOK, HelperButtonsI* helper=nullptr);

class gcImageControl;
class gcButton;

//! Dialog that is the message box it self with an icon and extra buttons
class gcMessageDialog : public gcDialog 
{
public:
	gcMessageDialog(wxWindow* parent, const wxString& message, const wxString& caption = "Message box", long style = wxOK | wxCANCEL, const wxPoint& pos = wxDefaultPosition);
	~gcMessageDialog();

	void addHelper(HelperButtonsI* helper);

protected:
	gcImageControl* m_imgIcon;
	wxStaticText* m_labInfo;
	wxSize getBestTextSize();

	void centerOnParent(wxWindow* p);
	void onButtonClick( wxCommandEvent& event );

	wxSizer *CreateButtonSizer(long flags);
	wxSizer *CreateSeparatedButtonSizer(long flags);

	void onChar(wxKeyEvent& event);

	void onParentClose(wxCloseEvent &e);

private:
	wxBoxSizer* m_bsButtonSizer;

	std::vector<gcButton*> m_bButtonList;
	std::vector<gcButton*> m_vHelperList;
	HelperButtonsI* m_pHelper;
};



#endif //DESURA_GCMESSAGEBOX_H
