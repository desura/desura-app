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

#ifndef DESURA_GCJSBINDING_H
#define DESURA_GCJSBINDING_H
#ifdef _WIN32
#pragma once
#endif

#include "gcJSBase.h"

namespace UserCore
{
	class ItemManagerI;

	namespace Item
	{
		class ItemInfoI;
	}
}

class DesuraJSBinding : public DesuraJSBase<DesuraJSBinding>
{
public:
	DesuraJSBinding();
	~DesuraJSBinding();

	static gcRefPtr<UserCore::ItemManagerI> getItemManager();

	EventV onPingEvent;

	static gcString getCacheValue_s(gcString name, gcString defaultV);

protected:
	friend class MainApp;
	friend class LanguageTestDialog;
	static gcRefPtr<UserCore::ItemManagerI> gs_pItemManager;

	JSObjHandle getLocalString(ChromiumDLL::JavaScriptFactoryI *m_pFactory, ChromiumDLL::JavaScriptContextI* context, JSObjHandle object, std::vector<JSObjHandle> &args);

	void* getItemInfoFromId(gcString id);

	std::vector<void*> getDevItems();
	std::vector<void*> getGames();
	std::vector<void*> getMods(gcRefPtr<UserCore::Item::ItemInfoI> game);
	std::vector<void*> getLinks();
	std::vector<void*> getFavorites();
	std::vector<void*> getRecent();
	std::vector<void*> getUploads();
	std::vector<void*> getNewItems();

	gcString getThemeColor(gcString name, gcString id);
	gcString getThemeImage(gcString id);

	gcString base64encode(gcString data);

	gcString getCacheValue(gcString name, gcString defaultV);
	void setCacheValue(gcString name, gcString value);

	int32 getItemFromId(gcString szId);
	gcString getTypeFromId(gcString szId);

	bool isOffline();

	gcString getCVarValue(gcString name);
	bool isValidIcon(gcString url);

	void updateCounts(int32 msg, int32 updates, int32 threads, int32 cart);
	void forceUpdatePoll();

	bool isWindows();
	bool isLinux();
	bool is32Bit();
	bool is64Bit();

	void* addLink(gcString name, gcString exe, gcString args);
	void delLink(gcRefPtr<UserCore::Item::ItemInfoI> item);
	void updateLink(gcRefPtr<UserCore::Item::ItemInfoI> item, gcString args);

	void login(gcString username, gcString loginCookie);
	void loginError(gcString error);

	void ping();
	
};


#endif //DESURA_GCJSBINDING_H
