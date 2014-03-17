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

#ifndef DESURA_CONSOLE_H
#define DESURA_CONSOLE_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcControls.h"
#include <wx/richtext/richtextctrl.h>

#include "util_thread/BaseThread.h"
#include "Color.h"
#include "Managers.h"

#include <atomic>
#include <thread>

typedef struct
{
	gcWString str;
	Color col;
	std::thread::id id;
} ConsoleText_s;


///////////////////////////////////////////////////////////////////////////////
/// Class Console
///////////////////////////////////////////////////////////////////////////////
class Console : public gcFrame 
{
public:
	Console(wxWindow* parent);
	~Console();

	WINDOW_UNREG();

	void appendText(gcWString text, Color col = Color(0));
	
	void processCommand();
	void conDump();
	void applyTheme();
	void setSize();
	void postShowEvent();

	//Used to ignore unit test thread
	static void ignoreThisThread();

	static void setTracer(TracerI *pTracer);
	static void trace(const char* szMessage, std::map<std::string, std::string> *pmArgs);

protected:
	void setupAutoComplete();

	wxRichTextCtrl* m_rtDisplay;
	gcComboBox* m_tbInfo;
	gcButton* m_butSubmit;
	
	void onWindowClose( wxCloseEvent& event );
	void onSubmitClicked( wxCommandEvent& event );
	void onKeyDown( wxKeyEvent& event );	

	void onShow(uint32&);
	void onConsoleText(ConsoleText_s& text);

	Event<ConsoleText_s> consoleTextEvent;
	Event<uint32> showEvent;

	wxBoxSizer* m_pSizer;

private:
	bool m_bCenterOnParent;

	static std::atomic<std::thread::id> s_IgnoredThread;
};

#endif //DESURA_LOG_FORM_H
