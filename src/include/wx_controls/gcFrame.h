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

#ifndef DESURA_GCFRAME_H
#define DESURA_GCFRAME_H
#ifdef _WIN32
#pragma once
#endif

#include <wx/frame.h>
#include "guiDelegate.h"
#include "gcManagers.h"

#include "gcCustomFrame.h"

class CVarInfo;

//! A top level frame that uses desura theme and also can have gui events
class gcFrame : public wxGuiDelegateImplementation< gcCustomFrame<wxFrame> >
{
public:
	enum PROGSTATE
	{
		P_NONE,
		P_NORMAL,
		P_ERROR,
		P_PAUSED,
	};

	gcFrame();
	gcFrame(wxWindow *parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, bool delayLoad = false);
	~gcFrame();

	WINDOW_UNREG();

	//! Sets the progress state in the taskbar for windows. Linux this is a noop
	//!
	void setProgressState(PROGSTATE state);
	
	//! Sets the progress percent in the taskbar for windows. Linux this is a noop
	//!	
	void setProgress(uint8 prog);

	//! Sets the size that this window should be and keeps resizing till it gets to this size. 
	//! This was to fix a bug where moving a window that tried to resize it self while moving would 
	//! cancel the resize
	//!
	virtual void setIdealSize(int width, int height);

	//! Enables saving of window position in the CVar db
	//!
	void setupPositionSave(const char* name, bool saveMax = true, int defWidth = -1, int defHeight = -1);
	
	//! Starts the recording of windows position
	//!
	void enablePositionSave(bool state = true);
	
	//! Loads the last saved window position
	//!
	bool loadSavedWindowPos();

	void setMessageBox(wxWindow *pDialog);

protected:
	bool checkAndSetPendingClose();

	void onIdle( wxIdleEvent& event );
	void initGCFrame(bool delayLoad, long style);

	void onMove(wxMoveEvent  &event);
	void onResize(wxSizeEvent &event);
	void onWindowClose(wxCloseEvent& event);

	CVar* getCVar(std::string name, std::string def);

	void onFormMax();
	EventV onFormMaxEvent;

private:
	wxSize m_IdealSize;
	bool m_bEnableIdealSize;

	bool m_bPendingClose = false;
	wxWindow *m_pMessageBox = nullptr;

	CVarInfo* m_pCVarInfo;

	DECLARE_EVENT_TABLE();
};

#endif
