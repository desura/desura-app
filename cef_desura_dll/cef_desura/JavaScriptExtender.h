///////////// Copyright 2010 Desura Pty Ltd. All rights reserved.  /////////////
//
//   Project     : desura_libcef_dll_wrapper
//   File        : JavaScriptExtender.h
//   Description :
//      [TODO: Write the purpose of JavaScriptExtender.h.]
//
//   Created On: 5/28/2010 2:01:13 PM
//   Created By: Mark Chandler <mailto:mark@moddb.com>
////////////////////////////////////////////////////////////////////////////

#ifndef DESURA_JAVASCRIPTEXTENDER_H
#define DESURA_JAVASCRIPTEXTENDER_H
#ifdef _WIN32
#pragma once
#endif

#include "cef_desura_includes/ChromiumBrowserI.h"
#include "include/cef_app.h"

class V8HandleBaseWrapper : public CefBase
{
public:
	V8HandleBaseWrapper(CefRefPtr<CefV8Handler> object)
	{
		m_pObject = object;
		this->AddRef();
	}

	IMPLEMENT_REFCOUNTING(V8HandleBaseWrapper);

	CefRefPtr<CefV8Handler> m_pObject;
};


class JavaScriptExtender : public CefV8Handler
{
public:
	static bool Register( ChromiumDLL::JavaScriptExtenderI* jse );

	JavaScriptExtender( ChromiumDLL::JavaScriptExtenderI* jse );
	~JavaScriptExtender();

	virtual bool Execute( const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception );

	ChromiumDLL::JavaScriptExtenderI* m_pJSExtender;

	IMPLEMENT_REFCOUNTING( JavaScriptExtender );
};


class JavaScriptWrapper : public ChromiumDLL::JavaScriptExtenderI
{
public:
	JavaScriptWrapper();
	JavaScriptWrapper( CefRefPtr<CefV8Handler> obj );

	virtual void destroy();
	virtual ChromiumDLL::JavaScriptExtenderI* clone();
	virtual ChromiumDLL::JSObjHandle execute( ChromiumDLL::JavaScriptFunctionArgs *args );

	virtual const char* getName();
	virtual const char* getRegistrationCode();

	virtual CefRefPtr<CefV8Handler> getCefV8Handler();
	virtual CefRefPtr<CefBase> getCefBase();

private:
	CefRefPtr<CefV8Handler> m_pObject;
};


#endif //DESURA_JAVASCRIPTEXTENDER_H
