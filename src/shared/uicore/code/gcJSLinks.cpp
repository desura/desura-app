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
#include "gcJSLinks.h"

#include "MainApp.h"

REGISTER_JSEXTENDER(DesuraJSLinks);

DesuraJSLinks::DesuraJSLinks() : DesuraJSBase("links", "native_binding_links.js")
{
	REG_SIMPLE_JS_VOIDFUNCTION( switchTab, DesuraJSLinks );
	REG_SIMPLE_JS_VOIDFUNCTION( loadUrl, DesuraJSLinks );

	REGISTER_JS_FUNCTION( internalLink, DesuraJSLinks );
}



void DesuraJSLinks::switchTab(int32 tabId)
{
	if (tabId == -1 || tabId >= END_PAGE)
		return;

	g_pMainApp->showPage((PAGE)tabId);
}

void DesuraJSLinks::loadUrl(int32 tabId, gcString url)
{
	if (tabId == -1)
		return;
	
	if (tabId >= END_PAGE || tabId != ITEMS)
		return;
	
	if (url.size() == 0)
		return;

	g_pMainApp->loadUrl(url.c_str(), (PAGE)tabId);
}

JSObjHandle DesuraJSLinks::internalLink(ChromiumDLL::JavaScriptFactoryI *factory, ChromiumDLL::JavaScriptContextI* context, JSObjHandle object, std::vector<JSObjHandle> &args)
{
	if (args.size() < 2)
		return factory->CreateUndefined();

	gcString szId;
	int32 action =0;

	FromJSObject(szId, args[0]);
	FromJSObject(action, args[1]);

	DesuraId id(Safe::atoll(szId.c_str()));
	std::vector<std::string> argList;

	for (size_t x=2; x<args.size(); x++)
	{
		gcString extra;

		try
		{
			FromJSObject(extra, args[x]);
			argList.push_back(extra);
		}
		catch (gcException)
		{
		}
	}

	if (action != -1 && action < ACTION_LAST)
		g_pMainApp->handleInternalLink(id, action, argList);

	return factory->CreateUndefined();
}

