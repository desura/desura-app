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

#ifndef DESURA_GCWEBFAKEBROWSER_H
#define DESURA_GCWEBFAKEBROWSER_H
#ifdef _WIN32
#pragma once
#endif

#include "cef_desura_includes/ChromiumBrowserI.h"

class gcPanel;

class gcWebFakeBrowser : public ChromiumDLL::ChromiumBrowserI
{
public:
	gcWebFakeBrowser(gcPanel* parent);

	virtual void destroy();
	virtual void onFocus(){;}

#ifdef WIN32
	virtual void onPaintBg(){;}
	virtual void onPaint();
	virtual void onResize(){;}
#else
	virtual void CreateBrowser(const std::string &defaultUrl){;}
	virtual void onResize(int x, int y, int width, int height){;}
#endif

	virtual void loadUrl(const char* url){;}
	virtual void loadString(const char* string){;}

	virtual void stop(){;}
	virtual void refresh(bool ignoreCache = false){;}
	virtual void back(){;}
	virtual void forward(){;}

	virtual void zoomIn(){;}
	virtual void zoomOut(){;}
	virtual void zoomNormal(){;}

	virtual void print(){;}
	virtual void viewSource(){;}

	virtual void undo(){;}
	virtual void redo(){;}
	virtual void cut(){;}
	virtual void copy(){;}
	virtual void paste(){;}
	virtual void del(){;}
	virtual void selectall(){;}

	virtual void setEventCallback(ChromiumDLL::ChromiumBrowserEventI* cbe){;}
	virtual void executeJScript(const char* code, const char* scripturl = 0, int startline = 0){;}

	virtual void showInspector(){;}
	virtual void hideInspector(){;}
	virtual void inspectElement(int x, int y){;}

	virtual void scroll(int x, int y, int delta, unsigned int flags){;}
	virtual int* getBrowserHandle(){return 0;}

	virtual ChromiumDLL::JavaScriptContextI* getJSContext(){return nullptr;}

private:
	gcPanel* m_pParent;
	gcWString m_szNoLoadString;
};

#endif //DESURA_GCWEBFAKEBROWSER_H
