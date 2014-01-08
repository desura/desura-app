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
#include "CefEvents.h"
#include "gcJSBase.h"
#include "IPCBrowser.h"



class CrumbExtender : public DesuraJSBase<CrumbExtender>
{
public:
	CrumbExtender() : DesuraJSBase("CrumbExtender", "host_binding.js")
	{
		REG_SIMPLE_JS_VOIDFUNCTION( clearCrumbs, CrumbExtender );
		REG_SIMPLE_JS_VOIDFUNCTION( addCrumb, CrumbExtender );
		REG_SIMPLE_JS_VOIDFUNCTION( updateCounts, CrumbExtender );
	}

protected:
	void clearCrumbs()
	{
		GetIPCBrowser()->clearCrumbs();
	}

	void addCrumb(gcString name, gcString val)
	{
		GetIPCBrowser()->addCrumb(name.c_str(), val.c_str());
	}

	void updateCounts(int32 msg, int32 updates, int32 threads, int32 cart)
	{
		GetIPCBrowser()->setCounts(msg, updates, threads, cart);
	}
};

REGISTER_JSEXTENDER(CrumbExtender);



class ObjectExtender : public DesuraJSBase<ObjectExtender>
{
public:
	ObjectExtender() : DesuraJSBase("ObjectExtender", "")
	{
	}

	ObjectExtender(const char* name) : DesuraJSBase(name, "")
	{
	}
};


void EventHandler::HandleJSBinding(ChromiumDLL::JavaScriptObjectI* jsObject, ChromiumDLL::JavaScriptFactoryI* factory)
{

}
