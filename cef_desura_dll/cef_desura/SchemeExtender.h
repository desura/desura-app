///////////// Copyright 2010 Desura Pty Ltd. All rights reserved.  /////////////
//
//   Project     : desura_libcef_dll_wrapper
//   File        : SchemeExtender.h
//   Description :
//      [TODO: Write the purpose of SchemeExtender.h.]
//
//   Created On: 6/17/2010 4:32:18 PM
//   Created By: Mark Chandler <mailto:mark@moddb.com>
////////////////////////////////////////////////////////////////////////////

#ifndef DESURA_SCHEMEEXTENDER_H
#define DESURA_SCHEMEEXTENDER_H
#ifdef _WIN32
#pragma once
#endif

#include "cef_desura_includes/ChromiumBrowserI.h"
#include "include/cef_app.h"
#include "include/cef_scheme.h"

class SchemeExtender : public CefResourceHandler, public ChromiumDLL::SchemeCallbackI
{
public:
	static bool Register(ChromiumDLL::SchemeExtenderI* se);

	SchemeExtender(ChromiumDLL::SchemeExtenderI* se);
	~SchemeExtender();

	virtual bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback);
	virtual void Cancel();

	virtual void GetResponseHeaders( CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl );
	virtual bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback);

	virtual void responseReady();
	virtual void dataReady();
	virtual void cancel();

	IMPLEMENT_REFCOUNTING( SchemeExtender );

private:
	ChromiumDLL::SchemeExtenderI* m_pSchemeExtender;
	CefRefPtr<CefCallback> m_Callback;
	bool m_NeedRedirect;
};


#endif //DESURA_SCHEMEEXTENDER_H
