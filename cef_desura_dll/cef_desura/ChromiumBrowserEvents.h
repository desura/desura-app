///////////// Copyright 2010 DesuraNet. All rights reserved. /////////////
//
//   Project     : desura_libcef_dll_wrapper
//   File        : ChromiumBrowserEvents.h
//   Description :
//      [TODO: Write the purpose of ChromiumBrowserEvents.h.]
//
//   Created On: 7/1/2010 11:35:47 PM
//   Created By: Mark Chandler <mailto:mark@moddb.com>
////////////////////////////////////////////////////////////////////////////

#ifndef DESURA_CHROMIUMBROWSEREVENTS_H
#define DESURA_CHROMIUMBROWSEREVENTS_H
#ifdef _WIN32
#pragma once
#endif

#include "include/cef_app.h"
#include "cef_desura_includes/ChromiumBrowserI.h"
#include "include/cef_client.h"

class ChromiumBrowser;


class ChromiumEventInfoI
{
public:
	virtual ChromiumDLL::ChromiumBrowserEventI* GetCallback()=0;
	virtual void SetBrowser(CefRefPtr<CefBrowser> browser)=0;
	virtual CefRefPtr<CefBrowser> GetBrowser()=0;
	virtual void setContext(CefRefPtr<CefV8Context> context)=0;
};

/////////////////////////////////////////////////////////////////////////////////////////////
/// LifeSpanHandler
/////////////////////////////////////////////////////////////////////////////////////////////

class LifeSpanHandler : public CefLifeSpanHandler, public virtual ChromiumEventInfoI
{
public:
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser);
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser);
	virtual bool OnBeforePopup(CefRefPtr<CefBrowser> parentBrowser, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, const CefString& url, CefRefPtr<CefClient>& client, CefBrowserSettings& settings);
};


/////////////////////////////////////////////////////////////////////////////////////////////
/// LoadHandler
/////////////////////////////////////////////////////////////////////////////////////////////

class LoadHandler : public CefLoadHandler, public virtual ChromiumEventInfoI
{
public:
	virtual void OnLoadingStateChange( CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward ) OVERRIDE;
	virtual void OnLoadStart( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame ) OVERRIDE;
	virtual void OnLoadEnd( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode ) OVERRIDE;
	virtual void OnLoadError( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& failedUrl, const CefString& errorText ) OVERRIDE;
};


/////////////////////////////////////////////////////////////////////////////////////////////
/// RequestHandler
/////////////////////////////////////////////////////////////////////////////////////////////

class RequestHandler : public CefRequestHandler, public virtual ChromiumEventInfoI
{
public:
	virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool isRedirect);
};


/////////////////////////////////////////////////////////////////////////////////////////////
/// DisplayHandler
/////////////////////////////////////////////////////////////////////////////////////////////

class DisplayHandler : public CefDisplayHandler, public virtual ChromiumEventInfoI
{
public:
	virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString& message, const CefString& source, int line);
};


/////////////////////////////////////////////////////////////////////////////////////////////
/// KeyboardHandler
/////////////////////////////////////////////////////////////////////////////////////////////

class KeyboardHandler : public CefKeyboardHandler, public virtual ChromiumEventInfoI
{
public:
	virtual bool OnPreKeyEvent( CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event, bool* is_keyboard_shortcut ) OVERRIDE;
};


/////////////////////////////////////////////////////////////////////////////////////////////
/// MenuHandler
/////////////////////////////////////////////////////////////////////////////////////////////

class MenuHandler : public CefContextMenuHandler, public virtual ChromiumEventInfoI
{
public:
	virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model);
	virtual bool OnContextMenuCommand( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags )
	{
		return true;
	}
};


/////////////////////////////////////////////////////////////////////////////////////////////
/// JSDialogHandler
/////////////////////////////////////////////////////////////////////////////////////////////

class JSDialogHandler : public CefJSDialogHandler, public virtual ChromiumEventInfoI
{
public:
	virtual bool OnJSAlert(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& message);
	virtual bool OnJSConfirm(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& message, bool& retval);
	virtual bool OnJSPrompt(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& message, const CefString& defaultValue, bool& retval, CefString& result);
};


/////////////////////////////////////////////////////////////////////////////////////////////
/// DownloadHandler
/////////////////////////////////////////////////////////////////////////////////////////////

class DownloadHandler : public CefDownloadHandler, public virtual ChromiumEventInfoI
{
public:
	virtual void OnBeforeDownload( CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, const CefString& suggested_name, CefRefPtr<CefBeforeDownloadCallback> callback ) OVERRIDE;
	virtual void OnDownloadUpdated( CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, CefRefPtr<CefDownloadItemCallback> callback ) OVERRIDE;
};


/////////////////////////////////////////////////////////////////////////////////////////////
/// RenderHandler
/////////////////////////////////////////////////////////////////////////////////////////////

class RenderHandler : public CefRenderHandler, public virtual ChromiumEventInfoI
{
public:
	virtual bool GetViewRect( CefRefPtr<CefBrowser> browser, CefRect& rect ) OVERRIDE;
	virtual void OnPaint( CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height ) OVERRIDE;
};


/////////////////////////////////////////////////////////////////////////////////////////////
/// ChromiumBrowserEvents
/////////////////////////////////////////////////////////////////////////////////////////////

class ChromiumBrowserEvents : 
	public CefClient
	, public virtual ChromiumEventInfoI
	, public LifeSpanHandler
	, public LoadHandler
	, public RequestHandler
	, public DisplayHandler
	, public KeyboardHandler
	, public MenuHandler
	, public JSDialogHandler
	, public DownloadHandler
	, public RenderHandler
	{
public:
	ChromiumBrowserEvents(ChromiumBrowser* pParent);

	void setCallBack(ChromiumDLL::ChromiumBrowserEventI* cbe);
	void setParent(ChromiumBrowser* parent);

	virtual ChromiumDLL::ChromiumBrowserEventI* GetCallback();
	virtual void SetBrowser(CefRefPtr<CefBrowser> browser);
	virtual CefRefPtr<CefBrowser> GetBrowser();
	virtual void setContext(CefRefPtr<CefV8Context> context);

	// Wrap
	virtual CefRefPtr<CefRenderHandler>			GetRenderHandler()			{ return (CefRenderHandler*) this; }

	virtual CefRefPtr<CefLifeSpanHandler>		GetLifeSpanHandler()		{ return (CefLifeSpanHandler*) this; }
	virtual CefRefPtr<CefLoadHandler>			GetLoadHandler()			{ return (CefLoadHandler*) this; }
	virtual CefRefPtr<CefRequestHandler>		GetRequestHandler()			{ return (CefRequestHandler*) this; }
	virtual CefRefPtr<CefDisplayHandler>		GetDisplayHandler()			{ return (CefDisplayHandler*) this; }
	virtual CefRefPtr<CefKeyboardHandler>		GetKeyboardHandler()		{ return (CefKeyboardHandler*) this; }
	virtual CefRefPtr<CefContextMenuHandler>	GetMenuHandler()			{ return (CefContextMenuHandler*) this; }
	virtual CefRefPtr<CefJSDialogHandler>		GetJSDialogHandler()		{ return (CefJSDialogHandler*) this; }
	virtual CefRefPtr<CefDownloadHandler>		GetDownloadHandler()		{ return (CefDownloadHandler*) this; }

	static ChromiumBrowserEvents* ChromiumBrowserEvents::GetChromiumContextEvents( CefRefPtr<CefBrowser> browser );

	// Wrap
	virtual bool OnProcessMessageReceived( CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message ) OVERRIDE
	{
		// Handle IPC messages from the render process...

		return false;
	}


private:
	CefRefPtr<CefBrowser> m_Browser;
	ChromiumBrowser* m_pParent;
	ChromiumDLL::ChromiumBrowserEventI *m_pEventCallBack;

	IMPLEMENT_REFCOUNTING(ChromiumBrowserEvents);
};


#endif //DESURA_CHROMIUMBROWSEREVENTS_H
