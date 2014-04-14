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
#include "MainApp.h"

#include <wx/wx.h>
#include <wx/snglinst.h>
#include <wx/dir.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>

#include "managers/ConCommand.h"
#include "wx_controls/gcMessageBox.h"

#include "managers/CVar.h"

#include <branding/branding.h>

#ifdef NIX
#include "util/UtilLinux.h"
#endif

#ifdef WIN32
#include "util/gcDDE.h"
#include <shobjidl.h>

ITaskbarList3* g_pITBL3= nullptr;
HINSTANCE g_hInstDLL;


#include <signal.h>

void CustomSigAbort(int nSig)
{
	gcAssert(false);
	throw std::exception("sig abort");
}

// This is where windows wants to start a DLL
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	//Do it here as well as this needs to be done once for dynamic run time and once for static run time
	signal(SIGABRT, CustomSigAbort);
	_set_error_mode(_OUT_TO_MSGBOX);

	g_hInstDLL = hinstDLL;
	return TRUE;
}
#endif

void ShutdownWebControl();
MainAppI* g_pMainApp = nullptr;

uint64 g_uiMainThreadId = 0;

uint64 GetMainThreadId()
{
	return g_uiMainThreadId;
}

#ifdef NIX

class ListenThread : public Thread::BaseThread
{
public:
	ListenThread(MainApp* mainApp) : Thread::BaseThread("IPC Listen Thread")
	{

		if (!mainApp)
			return;

		g_pMainApp = mainApp;
		m_bShouldStop = false;

		if ((socketListen = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
		{
			ERROR_OUTPUT("Failed to create socket");
			m_bShouldStop = true;
			return;
		}

		socketLocal.sun_family = AF_UNIX;
		strcpy(socketLocal.sun_path, UTIL::LIN::SOCK_PATH());
		ERROR_OUTPUT(socketLocal.sun_path);
		unlink(socketLocal.sun_path);
		len = strlen(socketLocal.sun_path) + sizeof(socketLocal.sun_family);

		if (bind(socketListen, (struct sockaddr*)&socketLocal, len) == -1)
		{
			ERROR_OUTPUT("Failed to bind socket to file");
			m_bShouldStop = true;
			return;
		}

		if (listen(socketListen, 5) == -1)
		{
			ERROR_OUTPUT("Failed to listen to socket");
			m_bShouldStop = true;
			return;
		}

		/* Wait up to five seconds. */
		tv.tv_sec = 1;
		tv.tv_usec = 0;
	}

protected:
	virtual void run()
	{
		for(;;)
		{
			if (m_bShouldStop)
				return;

			FD_ZERO(&rfds);
			FD_SET(socketListen, &rfds);

			int sretval = select(socketListen + 1, &rfds, nullptr, nullptr, &tv);

			if (sretval == -1) // error
			{
				ERROR_OUTPUT("select() error!");
				gcSleep(500);
				continue;
			}
			else if (sretval == 0) // timeout
			{
				gcSleep(500);
				continue;
			}

			socklen_t t = sizeof(socketRemote);
			if ((socketConnected = accept(socketListen, (struct sockaddr *)&socketRemote, &t)) == -1) {
				ERROR_OUTPUT("Failed to accept connection");
				exit(1);
			}

			n = recv(socketConnected, str, PATH_MAX - 1, 0);
			str[n] = '\0';

			gcString d(str);

			std::vector<std::string> out;
			UTIL::STRING::tokenize(d, out, " ");

			bool handled = false;

			for (size_t x=0; x<out.size(); x++)
			{
				std::string cur = out[x];

				if (cur.size() == 0)
					continue;

				while ((*cur.begin() == '"' && *cur.rbegin() == '"') || (*cur.begin() == '\'' && *cur.rbegin() == '\''))
				{
					cur.erase(cur.begin());
					cur.erase(cur.end()-1);
				}

				if (cur.size() == 0)
				{
					continue;
				}

				if (g_pMainApp)
				{
					g_pMainApp->disableQuietMode();
					g_pMainApp->handleInternalLink(cur.c_str());
					handled = true;
				}
			}

			if (!handled && g_pMainApp)
			{
				g_pMainApp->disableQuietMode();
				g_pMainApp->showMainWindow();
			}

			close(socketConnected);

			gcSleep(500);
		}
	}

	virtual void onStop()
	{
		m_bShouldStop = true;
		unlink(socketLocal.sun_path);
	}

private:
	volatile bool m_bShouldStop;
	int socketListen, socketConnected, t, len;
	struct sockaddr_un socketLocal, socketRemote;
	char str[PATH_MAX];
	MainApp* g_pMainApp;
	struct timeval tv;
	fd_set rfds;
	int n;
};

#include "wx/apptrait.h"

extern bool wxLocaleIsUtf8;

class gcAppTraits : public wxAppTraits
{
public:
	gcAppTraits(wxAppTraits* oldTraits)
	{
		m_pOldTraits = oldTraits;
	}

    virtual bool ShowAssertDialog(const wxString &msg) override
	{
		Warning("wx gcAssert: {0}\n", msg.mb_str());
		return true;
	}

	virtual wxLog* CreateLogTarget() override
	{
		return m_pOldTraits->CreateLogTarget();
	}

	virtual wxMessageOutput* CreateMessageOutput() override
	{
		return m_pOldTraits->CreateMessageOutput();
	}

	virtual wxFontMapper* CreateFontMapper() override
	{
		return m_pOldTraits->CreateFontMapper();
	}

	virtual wxRendererNative* CreateRenderer() override
	{
		return m_pOldTraits->CreateRenderer();
	}

	virtual bool HasStderr() override
	{
		return m_pOldTraits->HasStderr();
	}

	virtual wxEventLoopBase* CreateEventLoop() override
	{
		return m_pOldTraits->CreateEventLoop();
	}

	virtual wxTimerImpl* CreateTimerImpl(wxTimer* timer) override
	{
		return m_pOldTraits->CreateTimerImpl(timer);
	}

	virtual wxPortId GetToolkitVersion(int* a, int* b) const override
	{
		return m_pOldTraits->GetToolkitVersion(a, b);
	}

	virtual bool IsUsingUniversalWidgets() const override
	{
		return m_pOldTraits->IsUsingUniversalWidgets();
	}

	virtual wxString GetDesktopEnvironment() const override
	{
		return m_pOldTraits->GetDesktopEnvironment();
	}

private:
	wxAppTraits* m_pOldTraits;
};


#endif

class DesuraLog : public wxLog
{
public:
	virtual void DoLogString(const wxChar *msg, time_t timestamp) override
	{
		gcString str("{0}\n", msg);

		if (str.find("iCCP: known incorrect sRGB profile") != gcString::npos)
			return;

		//PostMessage(WM_NULL) crashes hard due to issue with posting console message to ui thread with this issue
		if (str.find("PostMessage(WM_NULL)") != gcString::npos)
			return;

		Debug(str);
	}
};

extern ConCommand cc_restart_forceupdate;

#ifdef WIN32
class myDDEConnection : public gcDDEConnection
{
protected:
	void onPoke(const char* item, const char* data, size_t len)
	{
		gcString d;
		d.assign(data, len);

		std::vector<std::string> out;
		UTIL::STRING::tokenize(d, out, " ");

		bool handled = false;

		for (size_t x=0; x<out.size(); x++)
		{
			std::string cur = out[x];

			if (cur.size() == 0)
				continue;

			while ((*cur.begin() == '"' && *cur.rbegin() == '"') || (*cur.begin() == '\'' && *cur.rbegin() == '\''))
			{
				cur.erase(cur.begin());
				cur.erase(cur.end()-1);
			}

			if (cur.size() == 0)
				continue;

			if (cur == "-forceupdate")
			{
				cc_restart_forceupdate();
				handled = true;
			}
			else if (g_pMainApp)
			{
				g_pMainApp->disableQuietMode();
				g_pMainApp->handleInternalLink(cur.c_str());
				handled = true;
			}
		}

		if (!handled && g_pMainApp)
		{
			g_pMainApp->disableQuietMode();
			g_pMainApp->showMainWindow();
		}
	}
};

class myDDEServer : public gcDDEServer
{
public:
	gcDDEConnection* onAcceptConnection(const char* topic)
	{
		return new myDDEConnection();
	}
};

bool WindowsShutdown(wxWindowMSW *win, WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
	if (nMsg == WM_QUERYENDSESSION)
	{
		return true;
	}
	else if (nMsg == WM_ENDSESSION)
	{
		if (wParam && g_pMainApp)
			g_pMainApp->Close();

		return true;
	}

	return false;
}

bool HandleTaskBarMsg(wxWindowMSW *win, WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
	if (!g_pITBL3)
		CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_ALL, IID_ITaskbarList3, (void**)&g_pITBL3);

	return false;
}
#endif

CVar gc_enable_api_debugging("gc_enable_api_debugging", "0", CFLAG_NOSAVE);

extern ConCommand cc_languagetest;

class Desura : public wxApp
{
public:
	Desura()
	{
		m_pChecker = nullptr;
#ifdef WIN32
		m_pServer = nullptr;
#else
		m_pListenThread = nullptr;
#endif
	}

	virtual ~Desura()
	{
		safe_delete(m_pChecker);
#ifdef WIN32
		safe_delete(m_pServer);
#else
		if (m_pListenThread)
			m_pListenThread->stop();

		safe_delete(m_pListenThread);
#endif
	}

	virtual bool OnInit() override
	{
		g_uiMainThreadId = Thread::BaseThread::GetCurrentThreadId();

		gcString link;
		auto bLanguageTest = false;

		for (int x=0; x<argc; x++)
		{
			if (argv[x].find("desura://") == 0)
				link = argv[x].char_str().operator const char *();

			if (argv[x] == "--debugapi")
				gc_enable_api_debugging.setValue(true);

			if (argv[x] == "--languagetest")
				bLanguageTest = true;
		}

#ifdef WIN32
		m_pChecker = new wxSingleInstanceChecker(wxT(PRODUCT_NAME));

		// If using a single instance, use IPC to
		// communicate with the other instance
		if (!m_pChecker->IsAnotherRunning())
		{
			// Create a new server
			m_pServer = new myDDEServer();
			m_pServer->create("Desura");
		}
		else
		{
			wxMessageBox(wxT("Another instance is all ready running."), PRODUCT_NAME_CATW(L" Error"));
			return false;
		}
#endif

		//needed for mfc loading otherwise it asserts out
		SetExitOnFrameDelete(true);

#ifdef WIN32
		if (UTIL::WIN::getOSId() >= WINDOWS_7)
		{
			m_wmTBC = RegisterWindowMessage(L"TaskbarButtonCreated");
			wxWindow::MSWRegisterMessageHandler(m_wmTBC, &HandleTaskBarMsg);
		}

		wxWindow::MSWRegisterMessageHandler(WM_QUERYENDSESSION, &WindowsShutdown);
		wxWindow::MSWRegisterMessageHandler(WM_ENDSESSION, &WindowsShutdown);
#endif

		wxInitAllImageHandlers();

		MainApp* ma = new MainApp();
		g_pMainApp = ma; 

		ma->Init(argc, argv);

		if (bLanguageTest)
		{
			ma->m_iMode = APP_MODE::MODE_OFFLINE;
			cc_languagetest();
			return false;
		}

		ma->run();
		wxLog::SetActiveTarget(new DesuraLog());

#ifdef NIX
		SetTopWindow(ma);
		m_pListenThread = new ListenThread(ma);
		m_pListenThread->start();

		//house keeping
		UTIL::FS::delFile(UTIL::LIN::expandPath("~/.desura_lock").c_str());
		UTIL::FS::delFile(UTIL::LIN::expandPath("~/.desura_socket").c_str());
		UTIL::FS::delFile(UTIL::LIN::expandPath("~/.desura_autologin").c_str());

		UTIL::FS::delFile(UTIL::LIN::expandPath("~/.desura/.socket").c_str());
		UTIL::FS::delFile(UTIL::LIN::expandPath("~/.desura/.autologin").c_str());
#endif

		if (link.size() > 0)
			g_pMainApp->handleInternalLink(link.c_str());

		return true;
	}

	virtual int OnExit() override
	{
		ERROR_OUTPUT("OnExit wxAPP");

		safe_delete(m_pChecker);
		ShutdownWebControl();

#ifdef WIN32
		safe_delete(m_pServer);

		if (UTIL::WIN::getOSId() >= WINDOWS_7)
		{
			wxWindow::MSWUnregisterMessageHandler(m_wmTBC, &HandleTaskBarMsg);

			if (g_pITBL3)
			{
				 g_pITBL3->Release();
				 g_pITBL3 = nullptr;
			}
		}

		wxWindow::MSWUnregisterMessageHandler(WM_ENDSESSION, &WindowsShutdown);
		wxWindow::MSWUnregisterMessageHandler(WM_QUERYENDSESSION, &WindowsShutdown);
#endif

		g_pMainApp = nullptr;

		return wxApp::OnExit();
	}

#ifdef WIN32
	virtual void ExitMainLoop() override
	{
		// instead of existing wxWidgets main loop, terminate the MFC one
		::PostQuitMessage(0);
	}
#endif	

	virtual void OnAssert(const wxChar *file, int line, const wxChar *cond, const wxChar *msg) override
	{
	}

#ifdef NIX
	virtual wxAppTraits* CreateTraits() override
	{
		return new gcAppTraits(wxApp::CreateTraits());
	}
#endif

	bool OnExceptionInMainLoop() override
	{
		auto e = std::current_exception();

		try
		{
			std::rethrow_exception(e);
		}
		catch (const gcException &e)
		{
			gcAssert(false);
			Warning("OnExceptionInMainLoop gcException: {0}\n", e.what());
		}
		catch (const std::exception &e)
		{
			gcAssert(false);
			Warning("OnExceptionInMainLoop std::exception: {0}\n", e.what());
		}
		catch (...)
		{
			gcAssert(false);
			Warning("OnExceptionInMainLoop Unknown exception\n");
		}
		
		return wxApp::OnExceptionInMainLoop();
	}

private:
	wxSingleInstanceChecker *m_pChecker;
#ifdef WIN32
	DWORD m_wmTBC;
	myDDEServer* m_pServer;
#else
	ListenThread* m_pListenThread;
#endif
};

IMPLEMENT_APP_NO_MAIN(Desura)

CONCOMMAND(exitApp, "exit")
{
	if (g_pMainApp)
		g_pMainApp->Close();
}
