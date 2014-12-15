/*
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)
Copyright (C) 2014 Bad Juju Games, Inc.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA.

Contact us at legal@badjuju.com.

*/

#include "Common.h"
#include "MainApp.h"
#include "BaseInstallPage.h"
#include "usercore/ItemHandleI.h"

#include "ItemForm.h"
#include "usercore/ItemManagerI.h"
#include "Managers.h"
#include "managers/WildcardDelegate.h"

class ItemHandleHelper : public UserCore::Item::Helper::ItemHandleHelperI
{
public:
	ItemHandleHelper()
	{
		m_uiId = 0;
	}

	Event<uint32> onCompleteEvent;
	void onComplete(uint32 status) override
	{
		onCompleteEvent(status);
	}

	Event<gcString> onCompleteStrEvent;
	void onComplete(gcString& string) override
	{
		onCompleteStrEvent(string);
	}

	Event<MCFCore::Misc::ProgressInfo> onMcfProgressEvent;
	void onMcfProgress(MCFCore::Misc::ProgressInfo& info) override
	{
		onMcfProgressEvent(info);
	}

	Event<uint32> onProgressUpdateEvent;
	void onProgressUpdate(uint32 progress) override
	{
		onProgressUpdateEvent(progress);
	}

	Event<gcException> onErrorEvent;
	void onError(gcException e) override
	{
		onErrorEvent(e);
	}

	Event<WCSpecialInfo> onNeedWildCardEvent;
	void onNeedWildCard(WCSpecialInfo& info) override
	{
		onNeedWildCardEvent(info);
	}

	Event<UserCore::Misc::GuiDownloadProvider> onDownloadProviderEvent;
	void onDownloadProvider(UserCore::Misc::GuiDownloadProvider& provider) override
	{
		onDownloadProviderEvent(provider);
	}

	Event<UserCore::Misc::VerifyComplete> onVerifyCompleteEvent;
	void onVerifyComplete(UserCore::Misc::VerifyComplete& info) override
	{
		onVerifyCompleteEvent(info);
	}

	Event<bool> onPauseEvent;
	void onPause(bool state) override
	{
		onPauseEvent(state);
	}

	virtual uint32 getId() override
	{
		return m_uiId;
	}

	void setId(uint32 id) override
	{
		m_uiId = id;
	}

private:
	uint32 m_uiId;

	gc_IMPLEMENT_REFCOUNTING(ItemHandleHelper);
};




using namespace UI::Forms::ItemFormPage;


typedef void (BaseInstallPage::*onCompleteIntFn)(uint32&);
typedef void (BaseInstallPage::*onCompleteStrFn)(gcString&);


 BaseInstallPage::BaseInstallPage(wxWindow* parent)
	 : BasePage(parent)
 {
	 Bind(wxEVT_CLOSE_WINDOW, &BaseInstallPage::onFormClose, this);
 }

 BaseInstallPage::~BaseInstallPage()
 {
	 deregisterHandle();
 }

 void BaseInstallPage::onFormClose(wxCloseEvent& event)
 {
	 deregisterHandle();
	 m_pItemHandle = nullptr;
 }

 void BaseInstallPage::setInfo(DesuraId id, gcRefPtr<UserCore::Item::ItemInfoI> pItemInfo)
{
	setInfo(GetUserCore()->getItemManager()->findItemHandle(id));
}

 void BaseInstallPage::setInfo(gcRefPtr<UserCore::Item::ItemHandleI> pItemHandle)
{
	gcTrace("");

	gcAssert(pItemHandle);

	m_pItemHandle = pItemHandle;
	BasePage::setInfo(pItemHandle->getItemInfo()->getId(), pItemHandle->getItemInfo());

	init();
	registerHandle();
}

void BaseInstallPage::pause(bool state)
{
	if (m_pItemHandle)
		m_pItemHandle->setPaused(state);
}

void BaseInstallPage::nonBlockStop()
{
}

void BaseInstallPage::stop()
{
}

void BaseInstallPage::registerHandle()
{
	gcTrace("");

	if (!m_pItemHandle)
		return;

	ItemForm* inf = dynamic_cast<ItemForm*>(GetParent());
	deregisterHandle();

	m_pIHH = gcRefPtr<ItemHandleHelper>::create();

	m_pIHH->onCompleteEvent += guiDelegate(this, (onCompleteIntFn)&BaseInstallPage::onComplete);
	m_pIHH->onProgressUpdateEvent += guiDelegate(this, &BaseInstallPage::onProgressUpdate);
	m_pIHH->onErrorEvent += guiDelegate(this, &BaseInstallPage::onError);
	m_pIHH->onNeedWildCardEvent += wcDelegate(inf);

	m_pIHH->onMcfProgressEvent += guiDelegate(this, &BaseInstallPage::onMcfProgress);
	m_pIHH->onCompleteStrEvent += guiDelegate(this, (onCompleteStrFn)&BaseInstallPage::onComplete);
	m_pIHH->onDownloadProviderEvent += guiDelegate(this, &BaseInstallPage::onDownloadProvider);

	m_pIHH->onVerifyCompleteEvent += guiDelegate(this, &BaseInstallPage::onVerifyComplete);
	m_pIHH->onPauseEvent += guiDelegate(this, &BaseInstallPage::onPause);

	m_pItemHandle->addHelper(m_pIHH);
	m_pItemHandle->getItemInfo()->getInfoChangeEvent() += guiDelegate(this, &BaseInstallPage::onItemUpdate);
}

void BaseInstallPage::deregisterHandle()
{
	gcTrace("");

	if (m_pItemHandle)
	{
		if (m_pItemHandle->getItemInfo())
		m_pItemHandle->getItemInfo()->getInfoChangeEvent() -= guiDelegate(this, &BaseInstallPage::onItemUpdate);

		if (m_pIHH.get())
			m_pItemHandle->delHelper(m_pIHH);
	}

	m_pIHH.reset();
}


void BaseInstallPage::onComplete(uint32& status)
{
}

void BaseInstallPage::onProgressUpdate(uint32& progress)
{
}

void BaseInstallPage::onError(gcException& e)
{
}


void BaseInstallPage::onMcfProgress(MCFCore::Misc::ProgressInfo& info)
{
}

void BaseInstallPage::onComplete(gcString& str)
{
}

void BaseInstallPage::onDownloadProvider(UserCore::Misc::GuiDownloadProvider& provider)
{
}

void BaseInstallPage::onItemUpdate(UserCore::Item::ItemInfoI::ItemInfo_s& info)
{
}

void BaseInstallPage::onVerifyComplete(UserCore::Misc::VerifyComplete& info)
{
}

void BaseInstallPage::onPause(bool &state)
{
}
