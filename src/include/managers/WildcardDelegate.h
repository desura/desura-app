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

#ifndef DESURA_WILDCARD_DELEGATE_H
#define DESURA_WILDCARD_DELEGATE_H
#ifdef _WIN32
#pragma once
#endif

#include "Event.h"
#include "wx_controls/guiDelegate.h"
#include "managers/WildcardManager.h"
#include "MainAppI.h"

extern MainAppI* g_pMainApp;

template <class TObj>
class WildCardDelegate : public DelegateI<WCSpecialInfo&>, public InvokeI
{
public:
	WildCardDelegate(TObj* t)
		: m_pObj(t)
	{
		if (m_pObj)
			m_pObj->registerDelegate(this);
	}

	~WildCardDelegate()
	{
		cancel();
	}

	void operator()(WCSpecialInfo& a) override
	{
		if (a.handled || m_bCanceled)
			return;

		std::function<void()> callback = [&a, this]()
		{
			if (m_bCanceled)
				return;

			g_pMainApp->processWildCards(a, m_pObj);
		};

		auto invoker = std::make_shared<Invoker>(callback);

		wxGuiDelegateEvent event(invoker, m_pObj->GetId());
		m_pObj->GetEventHandler()->AddPendingEvent(event);
		
		std::shared_ptr<Invoker> empty;

		setInvoker(invoker);
		invoker->wait();
		setInvoker(empty);
	}

	DelegateI<WCSpecialInfo&>* clone() override
	{
		return new WildCardDelegate(m_pObj);
	}

	void destroy() override
	{
		delete this;
	}

	void cancel(bool bDeregister = true) override
	{
		std::lock_guard<std::mutex> guard(m_InvokerMutex);

		m_bCanceled = true;

		if (m_pInvoker)
			m_pInvoker->cancel();

		if (m_pObj && bDeregister)
			m_pObj->deregisterDelegate(this);

		m_pObj = nullptr;
	}

	bool equals(DelegateI<WCSpecialInfo&>* di) override
	{
		return di->getCompareHash() == getCompareHash();
	}

	uint64 getCompareHash() const
	{
		return (uint64)m_pObj;
	}

protected:
	void setInvoker(std::shared_ptr<Invoker> &i)
	{
		std::lock_guard<std::mutex> guard(m_InvokerMutex);
		m_pInvoker = i;
	}

private:
	TObj* m_pObj;
	std::atomic<bool> m_bCanceled;
	std::mutex m_InvokerMutex;
	std::shared_ptr<Invoker> m_pInvoker;
};

template <class TObj>
DelegateI<WCSpecialInfo&>* wcDelegate(TObj* pObj)
{
	gcPanel* pan = dynamic_cast<gcPanel*>(pObj);
	gcFrame* frm = dynamic_cast<gcFrame*>(pObj);

	if (!pan && !frm)
	{
		gcAssert(false);
		return nullptr;
	}

	return new WildCardDelegate<TObj>(pObj);
}

#endif
