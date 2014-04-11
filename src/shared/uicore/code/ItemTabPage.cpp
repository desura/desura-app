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
#include "ItemTabPage.h"
#include "ItemToolBarControl.h"
#include "Managers.h"
#include "MainApp.h"

#include "gcWebHost.h"
#include "gcWebControl.h"
#include "gcJSBinding.h"

#include "cef_desura_includes/ChromiumBrowserI.h"



DesuraJSBinding *GetJSBinding();


class JSEventMap
{
public:
	ChromiumDLL::JSObjHandle findEventFunction(const gcString &name, ChromiumDLL::JSObjHandle root)
	{
		std::lock_guard<std::mutex> al(m_EventLock);

		if (!s_bMapValid)
			return nullptr;

		if (g_EventMap.find(name) != g_EventMap.end())
			return g_EventMap[name];

		if (!root.get() || root->isNull())
			return nullptr;

		if (!g_EventMap["__root__"])
			g_EventMap["__root__"] = root;

		if (!findAndAdd("__root__", "__desura__", "desura"))
			return nullptr;

		if (!findAndAdd("__desura__", "__events__", "events"))
			return nullptr;

		if (!findAndAdd("__events__", "__internal__", "internal"))
			return nullptr;

		ChromiumDLL::JSObjHandle ret = g_EventMap["__internal__"]->getValue(name.c_str());
		g_EventMap[name] = ret;

		return ret;
	}

	void reset()
	{
		std::lock_guard<std::mutex> al(m_EventLock);
		g_EventMap.clear();
	}

	bool isVaild()
	{
		std::lock_guard<std::mutex> al(m_EventLock);
		return s_bMapValid;
	}

	void setValid(bool bState)
	{
		std::lock_guard<std::mutex> al(m_EventLock);
		s_bMapValid = bState;
	}

protected:
	bool findAndAdd(const char* parentKey, const char* childKey, const char* childName)
	{
		if (!g_EventMap[parentKey])
			return false;

		if (!g_EventMap[childKey])
			g_EventMap[childKey] = g_EventMap[parentKey]->getValue(childName);

		if (!g_EventMap[childKey])
			return false;

		return true;
	}

private:
	std::mutex m_EventLock;
	std::map<gcString, ChromiumDLL::JSObjHandle> g_EventMap;
	bool s_bMapValid = false;
};

static JSEventMap g_JSEventMap;


void BrowserUICallback(ChromiumDLL::CallbackI* callback);

class JSCallback : public ChromiumDLL::CallbackI
{
public:
	JSCallback(ChromiumDLL::JavaScriptContextI* context, gcString name, const char* arg1, const char* arg2)
	{
		m_pContext = context;
		m_szName = name;
		m_uiNumArgs = 0;

		if (arg1)
		{
			m_szArg1 = arg1;
			m_uiNumArgs = 1;
		}

		if (arg2)
		{
			m_szArg2 = arg2;
			m_uiNumArgs = 2;
		}	

		if (m_szName == "onItemListUpdated")
			s_bGlobalItemUpdate = true;
	}

	void destroy() override
	{
		if (m_pContext)
			m_pContext->destroy();

		delete this;
	}

	void run() override
	{
		try
		{
			doRun();
		}
		catch (...)
		{
			Warning("JSCallback {0} threw exception", m_szName);
		}

		if (m_szName == "onItemListUpdated")
			s_bGlobalItemUpdate = false;
	}

protected:
	void doRun()
	{
		if (!g_JSEventMap.isVaild())
			return;

		if (m_szName == "onItemUpdate" && s_bGlobalItemUpdate)
			return;

		if (!m_pContext)
			return;

		m_pContext->enter();
		ChromiumDLL::JSObjHandle funct = g_JSEventMap.findEventFunction(m_szName, m_pContext->getGlobalObject());

		if (funct.get())
		{
			ChromiumDLL::JSObjHandle* argv = nullptr;

			if (m_uiNumArgs > 0)
				argv = new ChromiumDLL::JSObjHandle[m_uiNumArgs];

			if (m_uiNumArgs >= 1)
				argv[0] = m_pContext->getFactory()->CreateString(m_szArg1.c_str());

			if (m_uiNumArgs >= 2)
				argv[1] = m_pContext->getFactory()->CreateString(m_szArg2.c_str());

			ChromiumDLL::JavaScriptFunctionArgs args;
			args.function = nullptr;
			args.context = m_pContext;
			args.argc = m_uiNumArgs;
			args.argv = argv;
			args.factory = nullptr;
			args.object = nullptr;

			ChromiumDLL::JSObjHandle ret = funct->executeFunction(&args);
			delete [] argv;
		}

		m_pContext->exit();
	}

private:
	uint32 m_uiNumArgs;

	gcString m_szArg1;
	gcString m_szArg2;

	ChromiumDLL::JavaScriptContextI* m_pContext;
	gcString m_szName;

	static bool s_bGlobalItemUpdate;
};

bool JSCallback::s_bGlobalItemUpdate = false;

ItemTabPage::ItemTabPage(wxWindow* parent, gcWString homePage) 
	: HtmlTabPage(parent, homePage, ITEMS)
	, m_PingTimer(this)
{
	m_bNotifiedOfLowSpace = false;
	m_pItemControlBar = new ItemToolBarControl(parent);
	m_pItemControlBar->onSearchEvent += guiDelegate(this, &ItemTabPage::onSearchStr);

	m_pWebControl = nullptr;

	if (m_pWebControl)
	{
		m_pWebControl->onPageStartEvent -= delegate(&m_pControlBar->onPageStartLoadingEvent);
		m_pWebControl->onPageLoadEvent -= delegate(&m_pControlBar->onPageEndLoadingEvent);

		m_pWebControl->onPageStartEvent += delegate(&m_pItemControlBar->onPageStartLoadingEvent);
		m_pWebControl->onPageLoadEvent += delegate(&m_pItemControlBar->onPageEndLoadingEvent);
	}

	killControlBar();

	m_pItemControlBar->onButtonClickedEvent += guiDelegate(this, &ItemTabPage::onButtonClicked);
	g_JSEventMap.setValid(true);

	Bind(wxEVT_TIMER, &ItemTabPage::onPingTimer, this);
}

ItemTabPage::~ItemTabPage()
{
	m_PingTimer.Stop();

	g_JSEventMap.setValid(false);
	g_JSEventMap.reset();

	m_pItemControlBar->onButtonClickedEvent -= guiDelegate(this, &ItemTabPage::onButtonClicked);
	m_pItemControlBar->onSearchEvent -= guiDelegate(this, &ItemTabPage::onSearchStr);

	m_pWebControl->onPageStartEvent -= delegate(&m_pItemControlBar->onPageStartLoadingEvent);
	m_pWebControl->onPageLoadEvent -= delegate(&m_pItemControlBar->onPageEndLoadingEvent);

	if (GetUserCore())
	{
		std::vector<UserCore::Item::ItemInfoI*> aList;
		GetUserCore()->getItemManager()->getAllItems(aList);

		for (size_t x=0; x<aList.size(); x++)
			*aList[x]->getInfoChangeEvent() -= guiDelegate(this, &ItemTabPage::onItemUpdate);

		*GetUserCore()->getLowSpaceEvent() -= guiDelegate(this, &ItemTabPage::onLowDiskSpace);
		*GetUserCore()->getForcedUpdatePollEvent() -= guiDelegate(this, &ItemTabPage::onUpdatePoll);
	}

	if (GetUploadMng())
	{
		size_t count = GetUploadMng()->getCount();

		for (size_t x=0; x<count; x++)
		{
			UserCore::Misc::UploadInfoThreadI* item = GetUploadMng()->getItem(x);

			gcString key = item->getKey();
			*item->getUploadProgressEvent() -= guiExtraDelegate(this, &ItemTabPage::onUploadProgress, key);
			*item->getActionEvent() -= guiExtraDelegate(this, &ItemTabPage::onUploadAction, key);

			item->getUploadProgressEvent()->flush();
			item->getActionEvent()->flush();
		}

		*GetUploadMng()->getUpdateEvent() -= guiDelegate(this, &ItemTabPage::onUploadUpdate);
		GetUploadMng()->getUpdateEvent()->flush();
	}
}

void ItemTabPage::onFind()
{
	if (m_pItemControlBar)
		m_pItemControlBar->focusSearch();
}

void ItemTabPage::postEvent(const char* name, const char* arg1, const char* arg2)
{
	gcWebControl* webCtrl = dynamic_cast<gcWebControl*>(m_pWebControl);

	if (!webCtrl)
		return;

	BrowserUICallback(new JSCallback(webCtrl->getJSContext(), name, arg1, arg2));
}

BaseToolBarControl* ItemTabPage::getToolBarControl()
{
	return m_pItemControlBar;
}

void ItemTabPage::newBrowser(const char* homeUrl)
{
	if (m_pWebControl)
		return;

	gcWebControl* host = new gcWebControl(this, homeUrl);

	m_pWebPanel = host;
	m_pWebControl = host; 
}

void ItemTabPage::constuctBrowser()
{
	HtmlTabPage::constuctBrowser();

	m_pWebControl->onPageStartEvent += delegate(&m_pItemControlBar->onPageStartLoadingEvent);
	m_pWebControl->onPageLoadEvent += delegate(&m_pItemControlBar->onPageEndLoadingEvent);
	m_pWebControl->onPageLoadEvent += delegate(this, &ItemTabPage::doneLoading);

	if (!GetUserCore())
		return;

	UserCore::ItemManagerI *im = GetUserCore()->getItemManager();

	if (im)
	{
		*im->getOnUpdateEvent() += guiDelegate(this, &ItemTabPage::onItemsUpdate);
		*im->getOnRecentUpdateEvent() += guiDelegate(this, &ItemTabPage::onRecentUpdate);
		*im->getOnFavoriteUpdateEvent() += guiDelegate(this, &ItemTabPage::onFavoriteUpdate);
		*im->getOnNewItemEvent() += guiDelegate(this, &ItemTabPage::onNewItem);

		onItemsUpdate();
	}

	*GetUserCore()->getLowSpaceEvent() += guiDelegate(this, &ItemTabPage::onLowDiskSpace);
	*GetUserCore()->getForcedUpdatePollEvent() += guiDelegate(this, &ItemTabPage::onUpdatePoll);
	*GetUploadMng()->getUpdateEvent() += guiDelegate(this, &ItemTabPage::onUploadUpdate);
	onUploadUpdate();

	*GetUserCore()->getLoginItemsLoadedEvent() += guiDelegate(this, &ItemTabPage::onLoginItemsLoaded);

	GetJSBinding()->onPingEvent += guiDelegate(this, &ItemTabPage::onPing);
	m_PingTimer.Start(15 * 1000);
}


void ItemTabPage::reset()
{
	postEvent("onTabClicked", "game");
}

void ItemTabPage::onSearchText(const wchar_t* value)
{
	postEvent("onSearch", gcString(value).c_str());
}

void ItemTabPage::onButtonClicked(int32& id)
{ 
	switch (id)
	{
	case BUTTON_EXPAND:
		postEvent("onExpand");
		break;

	case BUTTON_CONTRACT:
		postEvent("onContract");
		break;

	case BUTTON_GAME:
		postEvent("onTabClicked", "game");
		break;
		
	case BUTTON_FAV:
		postEvent("onTabClicked", "fav");
		break;

	case BUTTON_TOOL:
		postEvent("onTabClicked", "tool");
		break;

	case BUTTON_DEV:
		postEvent("onTabClicked", "dev");
		break;
	}
}

void ItemTabPage::doneLoading()
{
	g_JSEventMap.reset();

	if (GetUserCore() && GetUserCore()->isDelayLoading())
		postEvent("onDelayLoad");
}

void ItemTabPage::onLoginItemsLoaded()
{
	postEvent("onDelayLoadDone");
	postEvent("onItemListUpdated");
}

void ItemTabPage::onItemsUpdate()
{
	std::vector<UserCore::Item::ItemInfoI*> aList;
	GetUserCore()->getItemManager()->getAllItems(aList);

	for (auto game : aList)
	{
		*game->getInfoChangeEvent() -= guiDelegate(this, &ItemTabPage::onItemUpdate);
		*game->getInfoChangeEvent() += guiDelegate(this, &ItemTabPage::onItemUpdate);
	}

	postEvent("onItemListUpdated");
}

void ItemTabPage::onRecentUpdate(DesuraId &id)
{
	postEvent("onRecentUpdate", id.toString().c_str());
}

void ItemTabPage::onFavoriteUpdate(DesuraId &id)
{
	postEvent("onFavoriteUpdate", id.toString().c_str());
}

void ItemTabPage::onItemUpdate(UserCore::Item::ItemInfoI::ItemInfo_s& info)
{
	postEvent("onItemUpdate", info.id.toString().c_str(), gcString("{0}", info.changeFlags).c_str());
}

void ItemTabPage::onUploadUpdate()
{
	size_t count = GetUploadMng()->getCount();

	for (size_t x=0; x<count; x++)
	{
		UserCore::Misc::UploadInfoThreadI* item = GetUploadMng()->getItem(x);

		gcString key = item->getKey();

		if (item->isDeleted())
		{
			*item->getUploadProgressEvent()  -= guiExtraDelegate(this, &ItemTabPage::onUploadProgress, key);
			*item->getActionEvent() -= guiExtraDelegate(this, &ItemTabPage::onUploadAction, key);
		}
		else
		{
			*item->getUploadProgressEvent()  += guiExtraDelegate(this, &ItemTabPage::onUploadProgress, key);
			*item->getActionEvent() += guiExtraDelegate(this, &ItemTabPage::onUploadAction, key);
		}
	}

	postEvent("onUploadUpdate", "all");
}

void ItemTabPage::onUploadProgress(gcString hash, UserCore::Misc::UploadInfo& info)
{
	uint32 prog = 0;

	m_UploadMutex.lock();
	prog = m_vUploadProgress[hash];
	m_vUploadProgress[hash] = info.percent;
	m_UploadMutex.unlock();

	if (prog != info.percent)
		postEvent("onUploadProgress", hash.c_str());
}

void ItemTabPage::onUploadAction(gcString hash)
{
	postEvent("onUploadUpdate", hash.c_str());
}

void ItemTabPage::onUpdatePoll(std::tuple<gcOptional<bool>, gcOptional<bool>, gcOptional<bool>> &info)
{
	auto forcedUpdate = std::get<0>(info);

	if (forcedUpdate && *forcedUpdate)
		postEvent("onUpdatePoll", "all");
}

void ItemTabPage::onNewItem(DesuraId &id)
{
	postEvent("onNewItemAdded", id.toString().c_str());
}

void ItemTabPage::onSearchStr(gcString &text)
{
	postEvent("onSearch", text.c_str());
}

void ItemTabPage::onShowAlert(const gcString &text, uint32 time)
{
	postEvent("onShowAlert", text.c_str(), gcString("{0}", time).c_str());
}

void ItemTabPage::onLowDiskSpace(std::pair<bool,char> &info)
{
	if (m_bNotifiedOfLowSpace)
		return;

	m_bNotifiedOfLowSpace = true;

#ifdef WIN32
	gcString text;
	
	if (info.first)
		text = gcString(Managers::GetString("#IF_LOWSPACE_SYS"), info.second);
	else
		text = gcString(Managers::GetString("#IF_LOWSPACE_CACHE"), info.second);

	onShowAlert(text, 0);
#endif
}

void ItemTabPage::onPingTimer(wxTimerEvent&)
{
	if (!m_bPingBack)
	{
		Warning("Item tab page did not ping back after 15 seconds\n");
		m_pWebControl->refresh();
	}
	else
	{
		postEvent("onPing");
	}
		
	m_bPingBack = false;
}

void ItemTabPage::onPing()
{
	m_bPingBack = true;
}
