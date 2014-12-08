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
#include "FlashDebug.h"


class RunThread : public Thread::BaseThread
{
public:
	RunThread(HWND hwnd) : Thread::BaseThread("FlashThread")
	{
		m_Hwnd = hwnd;
	}

	void run()
	{
		gcSleep(5000);

#ifdef WIN32
		FLASHWINFO info;

		memset(&info, 0, sizeof(FLASHWINFO));
		info.cbSize = sizeof(FLASHWINFO);
		info.hwnd = m_Hwnd;
		info.dwFlags = FLASHW_TRAY|FLASHW_TIMERNOFG;
		info.uCount = 20;

		FlashWindowEx(&info);
#endif

	}

	HWND m_Hwnd;
};


FlashDebug::FlashDebug(wxWindow* parent) : gcFrame(parent, wxID_ANY, "Flash Debug", wxDefaultPosition, wxSize( 700,468 ), wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 2, 2, 0, 0 );

	wxButton* m_button1 = new wxButton( this, wxID_ANY, wxT("MyButton"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_button1, 0, wxALL, 5 );

	this->SetSizer( gSizer1 );
	this->Layout();

	this->Centre( wxBOTH );

	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &FlashDebug::onButtonClicked, this);
}

FlashDebug::~FlashDebug()
{

}

void FlashDebug::onButtonClicked(wxCommandEvent &event)
{
	RunThread* thread = new RunThread((HWND)this->GetHWND());
	thread->start();
}
