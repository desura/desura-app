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

#include "common.h"
#include "UICoreI.h"

#include <wx/wx.h>
#include <wx/snglinst.h>
#include <wx/evtloop.h>

#include <shlobj.h>

extern wxFrame* g_pMainApp;
extern "C" CEXPORT UICoreI* GetInterface();

WXDLLIMPEXP_BASE void wxSetInstance(HINSTANCE hInst);
HHOOK hIISHook = nullptr;

LRESULT CALLBACK CallWndProc(int nCode,WPARAM wParam, LPARAM lParam)
{
	CWPSTRUCT* msg = (CWPSTRUCT*)lParam;

	//this is needed here as it doesnt process this event properly.
	if (msg->message == WM_QUERYENDSESSION)
	{
		GetInterface()->closeMainWindow();

		if (hIISHook != nullptr)
			UnhookWindowsHookEx(hIISHook);

		hIISHook = nullptr;
	}

	return CallNextHookEx(hIISHook, nCode, wParam, lParam);
}



class UICore : public UICoreI
{
public:
	UICore()
	{
		m_bExitCodeSet = false;
		m_iExitCode = 0;
	}

	~UICore()
	{
		if (hIISHook != nullptr)
			UnhookWindowsHookEx(hIISHook);

		hIISHook = nullptr;
	}

	void setDesuraVersion(const char* version)
	{
	}

	bool initWxWidgets(HINSTANCE hInst, int CmdShow, int argc, char** argv)
	{
		hIISHook = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)CallWndProc, 0, GetCurrentThreadId());

		wxSetInstance(hInst);
		wxApp::m_nCmdShow = CmdShow;
		wxEntry(argc, argv);

		if (!wxTheApp || !wxTheApp->CallOnInit())
			return false;

		return true;
	}

	void exitApp(int* result)
	{
		if ( wxTheApp )
			wxTheApp->OnExit();

		wxEntryCleanup();

		if (m_bExitCodeSet)
			*result = m_iExitCode;
	}

	bool preTranslateMessage(MSG *msg)
	{
		wxEventLoop *evtLoop = static_cast<wxEventLoop*>(wxEventLoop::GetActive());

		if ( evtLoop && evtLoop->PreProcessMessage(msg))
			return true;

		return false;
	}

	BOOL onIdle()
	{
		return (wxTheApp && wxTheApp->ProcessIdle());
	}

	HWND getHWND()
	{
		return (HWND)g_pMainApp->GetHWND();
	}

	void setRestartFunction(RestartFP rfp)
	{
	}

	void setCrashDumpSettings(DumpSettingsFP dsfp)
	{
	}

	void setCrashDumpLevel(DumpLevelFP dlfp)
	{
	}

	void closeMainWindow()
	{
		if (g_pMainApp)
			g_pMainApp->Close(true);
	}

	void restart(const char* args = nullptr)
	{
	}

	const char* getAppVersion()
	{
		return nullptr;
	}

	void setExitCode(int32 exitCode)
	{
		m_bExitCodeSet = true;
		m_iExitCode = exitCode;
	}

	int runUnitTests(int argc, char** argv)
	{
		return -1;
	}

	void setTracer(TracerI *) override
	{

	}

private:
	bool m_bExitCodeSet;
	int32 m_iExitCode;
};

UICore g_pUICore;


void SetExitCode(int32 code)
{
	g_pUICore.setExitCode(code);
}

extern "C"
{
	CEXPORT UICoreI* GetInterface()
	{
		return &g_pUICore;
	}
}
