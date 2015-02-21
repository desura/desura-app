///////////// Copyright 2010 Desura Pty Ltd. All rights reserved.  /////////////
//
//   Project     : minimal
//   File        : ChromiumBrowser.cpp
//   Description :
//      [TODO: Write the purpose of ChromiumBrowser.cpp.]
//
//   Created On: 5/25/2010 5:40:26 PM
//   Created By: Mark Chandler <mailto:mark@moddb.com>
////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)
#include "windows.h"
#endif

#include "ChromiumBrowser.h"
#include "client_app.h"
#include "include/cef_app.h"

#if defined(_WIN32)
#include "include/cef_sandbox_win.h"
#endif
#include "JavaScriptExtender.h"
#include "SchemeExtender.h"

#include "JavaScriptContext.h"
#include "ChromiumBrowserEvents.h"

#include <locale>
#include <codecvt>


#ifdef OS_LINUX
#include <gtk/gtk.h>
#endif

#if defined __x86_64 || defined __amd64 || defined __x86_64__
	#define NIX64 1
#endif

#if defined( _WIN32 )

// #define	WIN_USE_SANDBOX		1

#endif

ChromiumDLL::LogMessageHandlerFn g_pLogHandler = NULL;

bool logHandler(int level, const std::string& msg)
{
	if (g_pLogHandler)
		return g_pLogHandler(level, msg.c_str());

	return false;
}

#ifdef OS_LINUX
static void gtkFocus(GtkWidget *widget, GdkEvent *event, ChromiumBrowser *data)
{
	if (data)
		data->onFocus();
}
#endif


extern "C"
{
#if defined(_WIN32)
	#if defined( WIN_USE_SANDBOX )
		#pragma comment(lib, "cef_sandbox.lib")
	#endif
#endif

	DLLINTERFACE void CEF_DoMsgLoop()
	{
		CefDoMessageLoopWork();
	}

	DLLINTERFACE bool CEF_Init( bool threaded, const char* cachePath, const char* logPath, const char* userAgent, void* instance )
	{
#if defined(_WIN32)
		CefMainArgs args( static_cast< HINSTANCE >(instance) );
#else
		CefMainArgs args;
#endif
		CefSettings settings;

		std::wstring cachePathW = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes( cachePath );
		cef_string_copy( cachePathW.c_str(), cachePathW.size(), &settings.cache_path );

		std::wstring userAgentW = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes( userAgent );
		cef_string_copy( userAgentW.c_str(), userAgentW.size(), &settings.user_agent );

		settings.multi_threaded_message_loop = threaded;

		CefRefPtr<client::ClientApp> app( new client::ClientApp );

		void* sandbox_info = NULL;

		if ( strlen( logPath ) )
		{
			std::wstring logfileW = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes( logPath );
			cef_string_copy( logfileW.c_str(), logfileW.size(), &settings.log_file );

			settings.log_severity = cef_log_severity_t::LOGSEVERITY_VERBOSE;
		}

		settings.single_process = true;
		settings.context_safety_implementation = -1;
		settings.command_line_args_disabled = true;
		settings.uncaught_exception_stack_size = 20;

#if defined(_WIN32)
	#if defined( WIN_USE_SANDBOX )
			CefScopedSandboxInfo scoped_sandbox;
			sandbox_info = scoped_sandbox.sandbox_info();
			settings.no_sandbox = false;
	#else
			settings.no_sandbox = true;
	#endif
#else
		settings.no_sandbox = true;
#endif

		if (!CefInitialize(args, settings, app.get(), sandbox_info ))
			return false;

#if defined(_WIN32)
		CefAddWebPluginDirectory( "bin\\" );
		CefAddWebPluginPath("pepflashplayer.dll");
#else
		CefAddWebPluginPath("libdesura_flashwrapper.so");
#endif
		CefRefreshWebPlugins();

		return true;
	}

	DLLINTERFACE void CEF_Stop()
	{
		CefShutdown();
	}

	DLLINTERFACE bool CEF_RegisterJSExtender(ChromiumDLL::JavaScriptExtenderI* extender)
	{
		return JavaScriptExtender::Register(extender);
	}

	DLLINTERFACE bool CEF_RegisterSchemeExtender(ChromiumDLL::SchemeExtenderI* extender)
	{
		return SchemeExtender::Register(extender);
	}

	DLLINTERFACE ChromiumDLL::ChromiumBrowserI* CEF_NewChromiumBrowser(int* formHandle, const char* name, const char* defaultUrl)
	{
		return new ChromiumBrowser((WIN_HANDLE)formHandle, defaultUrl);
	}

	DLLINTERFACE void CEF_SetLogHandler(ChromiumDLL::LogMessageHandlerFn logFn)
	{
		g_pLogHandler = logFn;
	}

	DLLINTERFACE void CEF_PostCallback(ChromiumDLL::CallbackI* callback)
	{
		CefRefPtr<TaskWrapper> cb = new TaskWrapper( callback );
		CefPostTask( TID_UI, cb );
	}
}

enum ACTION
{
	A_STOPLOAD,
	A_REFRESH,
	A_BACK,
	A_FORWARD,
	A_ZOOMIN,
	A_ZOOMOUT,
	A_ZOOMNORMAL,
	A_PRINT,
	A_VIEWSOURCE,
	A_UNDO,
	A_REDO,
	A_CUT,
	A_COPY,
	A_PASTE,
	A_DEL,
	A_SELECTALL,
};

class BrowserTask : public CefTask
{
public:
	BrowserTask(CefBrowser* browser, ACTION action)
	{
		m_pBrowser = browser;
		this->AddRef();
		m_Action = action;
	}

	virtual void Execute()
	{
		if (!m_pBrowser)
			return;

		bool handled = true;

		switch (m_Action)
		{
			case A_STOPLOAD:	m_pBrowser->StopLoad();						break;
			case A_REFRESH:		m_pBrowser->ReloadIgnoreCache();			break;
			case A_BACK:		m_pBrowser->GoBack();						break;
			case A_FORWARD:		m_pBrowser->GoForward();					break;
			default:
				handled = false;
				break;
		};

		if (handled)
			return;

		CefRefPtr<CefFrame> frame = m_pBrowser->GetFocusedFrame();

		if (!frame.get())
			return;

		switch (m_Action)
		{
// TODO: Resolve what values/function replaces zoomin/zoomout
//			case A_ZOOMIN:		m_pBrowser->GetHost()->SetZoomLevel(0.0);	break;
//			case A_ZOOMOUT:		m_pBrowser->GetHost()->SetZoomLevel(0.0);	break;
			case A_ZOOMNORMAL:	m_pBrowser->GetHost()->SetZoomLevel(0.0);	break;
			case A_PRINT:		m_pBrowser->GetHost()->Print();				break;
			case A_VIEWSOURCE:	frame->ViewSource(); break;
			case A_UNDO:		frame->Undo();		break;
			case A_REDO:		frame->Redo();		break;
			case A_CUT:			frame->Cut();		break;
			case A_COPY:		frame->Copy();		break;
			case A_PASTE:		frame->Paste();		break;
			case A_DEL:			frame->Delete();	break;
			case A_SELECTALL:	frame->SelectAll(); break;

			default:
				handled = false;
				break;
		};
	}

	IMPLEMENT_REFCOUNTING(BrowserTask);

	ACTION m_Action;
	CefBrowser* m_pBrowser;
};

ChromiumBrowser::ChromiumBrowser(WIN_HANDLE handle, const char* defaultUrl)
{
	m_iLastTask = 0;
	m_hFormHandle = handle;
	m_pBrowser = NULL;

	m_rEventHandler = (CefClient*)new ChromiumBrowserEvents(this);
	init(defaultUrl);
}

ChromiumBrowser::~ChromiumBrowser()
{
	ChromiumBrowserEvents* cbe = (ChromiumBrowserEvents*)m_rEventHandler.get();

	if (cbe)
	{
		cbe->setParent(NULL);
		cbe->setCallBack(NULL);
	}
}

CefBrowserSettings ChromiumBrowser::getBrowserDefaults()
{
	CefBrowserSettings browserDefaults;

// TODO: Resolve these settings (developer_tools_disabled & drag_drop_disabled)
//	browserDefaults.developer_tools_disabled = false;
//	browserDefaults.drag_drop_disabled = true;

	browserDefaults.webgl = STATE_DISABLED;
	browserDefaults.universal_access_from_file_urls = STATE_ENABLED;
	browserDefaults.file_access_from_file_urls = STATE_ENABLED;
	browserDefaults.java = STATE_DISABLED;
	browserDefaults.javascript = STATE_ENABLED;
	browserDefaults.javascript_close_windows = STATE_DISABLED;
	browserDefaults.javascript_open_windows = STATE_DISABLED;

	return browserDefaults;
}

#ifdef OS_WIN

void ChromiumBrowser::init(const char *defaultUrl)
{
	m_WinInfo.style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_TABSTOP;
	m_WinInfo.height = 500;
	m_WinInfo.width = 500;
	m_WinInfo.parent_window = m_hFormHandle;

	std::wstring nameW = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes( "DesuraCEFBrowser" );
	cef_string_copy( nameW.c_str(), nameW.size(), &m_WinInfo.window_name );

	CefBrowserHost::CreateBrowser( m_WinInfo, m_rEventHandler, defaultUrl, getBrowserDefaults(), CefRequestContext::GetGlobalContext() );
}

#else

class CreateTask : public CefTask
{
public:
	CreateTask(ChromiumBrowser* browser, const std::string& defaultUrl)
	{
		m_pBrowser = browser;
		m_szDefaultUrl = defaultUrl;
	}

	void Execute()
	{
		m_pBrowser->initCallback(m_szDefaultUrl);
	}

	virtual int AddRef(){return 1;}
	virtual int Release(){return 1;}
	virtual int GetRefCt(){return 1;}

	ChromiumBrowser *m_pBrowser;
	std::string m_szDefaultUrl;
};

void ChromiumBrowser::init(const char *defaultUrl)
{
	CefPostTask(TID_UI, new CreateTask(this, defaultUrl));
}

void ChromiumBrowser::initCallback(const std::string& defaultUrl)
{
	m_WinInfo.SetAsChild(GTK_WIDGET(m_hFormHandle));

	m_pBrowser = CefBrowser::CreateBrowserSync(m_WinInfo, m_rEventHandler, defaultUrl.c_str(), getBrowserDefaults());
	g_signal_connect(GTK_WIDGET(m_hFormHandle), "button-press-event", G_CALLBACK(gtkFocus), this);
	gtk_widget_show_all(GTK_WIDGET(m_hFormHandle));
}
#endif






void ChromiumBrowser::loadString(const char* string)
{
	if (m_pBrowser && m_pBrowser->GetMainFrame())
	{
		m_pBrowser->GetMainFrame()->LoadString(string, "http://local");
	}
	else
	{
		if (string)
			m_szBuffer = string;
		m_iLastTask = 1;
	}
}

void ChromiumBrowser::loadUrl(const char* url)
{
	if (m_pBrowser)
	{
		m_pBrowser->GetMainFrame()->LoadURL(url);
	}
	else
	{
		m_szBuffer = url;
		m_iLastTask = 2;
	}
}


void ChromiumBrowser::stop()
{
	CefPostTask(TID_UI, new BrowserTask(m_pBrowser, A_STOPLOAD));
}

void ChromiumBrowser::refresh(bool ignoreCache)
{
	CefPostTask(TID_UI, new BrowserTask(m_pBrowser, A_REFRESH));
}

void ChromiumBrowser::back()
{
	CefPostTask(TID_UI, new BrowserTask(m_pBrowser, A_BACK));
}

void ChromiumBrowser::forward()
{
	CefPostTask(TID_UI, new BrowserTask(m_pBrowser, A_FORWARD));
}

void ChromiumBrowser::zoomIn()
{
	CefPostTask(TID_UI, new BrowserTask(m_pBrowser, A_ZOOMIN));
}

void ChromiumBrowser::zoomOut()
{
	CefPostTask(TID_UI, new BrowserTask(m_pBrowser, A_ZOOMOUT));	
}

void ChromiumBrowser::zoomNormal()
{
	CefPostTask(TID_UI, new BrowserTask(m_pBrowser, A_ZOOMNORMAL));
}

void ChromiumBrowser::print()
{
	CefPostTask(TID_UI, new BrowserTask(m_pBrowser, A_PRINT));
}

void ChromiumBrowser::viewSource()
{
	CefPostTask(TID_UI, new BrowserTask(m_pBrowser, A_VIEWSOURCE));	
}

void ChromiumBrowser::undo()
{
	CefPostTask(TID_UI, new BrowserTask(m_pBrowser, A_UNDO));
}

void ChromiumBrowser::redo()
{
	CefPostTask(TID_UI, new BrowserTask(m_pBrowser, A_REDO));
}

void ChromiumBrowser::cut()
{
	CefPostTask(TID_UI, new BrowserTask(m_pBrowser, A_CUT));
}


void ChromiumBrowser::copy()
{
	CefPostTask(TID_UI, new BrowserTask(m_pBrowser, A_COPY));	
}

void ChromiumBrowser::paste()
{
	CefPostTask(TID_UI, new BrowserTask(m_pBrowser, A_PASTE));	
}

void ChromiumBrowser::del()
{
	CefPostTask(TID_UI, new BrowserTask(m_pBrowser, A_DEL));
}

void ChromiumBrowser::selectall()
{
	CefPostTask(TID_UI, new BrowserTask(m_pBrowser, A_SELECTALL));
}



void ChromiumBrowser::setEventCallback(ChromiumDLL::ChromiumBrowserEventI* cbeI)
{
	ChromiumBrowserEvents* cbe = (ChromiumBrowserEvents*)m_rEventHandler.get();

	if (cbe)
		cbe->setCallBack(cbeI);
}

void ChromiumBrowser::executeJScript(const char* code, const char* scripturl, int startline)
{
	if (!m_pBrowser || !m_pBrowser->GetMainFrame() || !code)
		return;

	m_pBrowser->GetMainFrame()->ExecuteJavaScript(code, scripturl?scripturl:"", startline);
}

void ChromiumBrowser::onFocus()
{
	if (m_pBrowser)
		m_pBrowser->GetHost()->SetFocus(true);
}

#if defined(_WIN32)
void ChromiumBrowser::onPaintBg()
{
	// Dont erase the background if the browser window has been loaded
	// (this avoids flashing)
}


void ChromiumBrowser::onPaint()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(m_hFormHandle, &ps);
	EndPaint(m_hFormHandle, &ps);
}

void ChromiumBrowser::onResize()
{
	HWND hWnd = m_hFormHandle;

	if(m_pBrowser && m_pBrowser->GetHost()->GetWindowHandle())
	{
		// Resize the browser window and address bar to match the new frame
		// window size
		RECT rect;
		::GetClientRect(hWnd, &rect);

		HDWP hdwp = BeginDeferWindowPos(1);
		hdwp = DeferWindowPos(hdwp, m_pBrowser->GetHost()->GetWindowHandle(), NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
		EndDeferWindowPos(hdwp);
	}
}

#else
void ChromiumBrowser::onResize(int x, int y, int width, int height)
{
	GtkAllocation a;
	a.x = x;
	a.y = y;
	a.width = width;
	a.height = height;

	gtk_widget_size_allocate(GTK_WIDGET(m_hFormHandle), &a);
	gtk_widget_set_size_request(GTK_WIDGET(m_hFormHandle), width, height);
}
#endif

void ChromiumBrowser::setBrowser(CefBrowser* browser)
{
	m_pBrowser = browser;

	if (m_iLastTask == 1)
	{
		loadString(m_szBuffer.c_str());
	}
	else if (m_iLastTask == 2)
	{
		loadUrl(m_szBuffer.c_str());
	}

	m_szBuffer = "";

#ifdef WIN32
	onResize();
#endif
}

void ChromiumBrowser::showInspector()
{
// TODO: Need CefPoint argument
//	if (m_pBrowser)
//		m_pBrowser->GetHost()->ShowDevTools( m_WinInfo, m_rEventHandler, getBrowserDefaults() );
}

void ChromiumBrowser::hideInspector()
{
	if (m_pBrowser)
		m_pBrowser->GetHost()->CloseDevTools();
}

void ChromiumBrowser::inspectElement(int x, int y)
{
// TODO: ChromiumBrowser::InspectElement
//	if (m_pBrowser)
//		m_pBrowser->GetHost()->InspectElement(x, y);
}

void ChromiumBrowser::scroll(int x, int y, int delta, unsigned int flags)
{
// TODO: Resolve how delta is applied (doesn't look it's used yet)
//	if (m_pBrowser)
//		m_pBrowser->MouseWheelEvent(x, y, delta, flags);
//		m_pBrowser->GetHost()->SendMouseWheelEvent(const CefMouseEvent& event, int deltaX, int deltaY);
}

int* ChromiumBrowser::getBrowserHandle()
{
	if (m_pBrowser)
		return (int*)m_pBrowser->GetHost()->GetWindowHandle();

	return 0;
}

ChromiumDLL::JavaScriptContextI* ChromiumBrowser::getJSContext()
{
	if (m_pBrowser)
		return new JavaScriptContext(m_rContext);

	return NULL;
}

void ChromiumBrowser::setContext(CefRefPtr<CefV8Context> context)
{
	m_rContext = context;
}
