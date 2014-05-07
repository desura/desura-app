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

#ifndef DESURA_GUIDELEGATE_H
#define DESURA_GUIDELEGATE_H
#ifdef _WIN32
#pragma once
#endif

#include <wx/wx.h>
#include "Event.h"
#include "util_thread/BaseThread.h"

#include <type_traits>
#include <memory>
#include <atomic>
#include <list>

class gcPanel;
class gcDialog;
class gcScrolledWindow;
class gcFrame;
class gcTaskBarIcon;


enum MODE
{
	MODE_PENDING,
	MODE_PENDING_WAIT,
	MODE_PROCESS,
};

uint64 GetMainThreadId();


class EventHelper
{
public:
	void done()
	{
		if (m_bDone)
			return;

		m_bDone = true;
		m_WaitCond.notify();
	}

	void wait()
	{
		while (!m_bDone)
			m_WaitCond.wait(0, 500);
	}
	
	bool isDone() const
	{
		return m_bDone;
	}

private:
	Thread::WaitCondition m_WaitCond;
	std::atomic<bool> m_bDone;
};

class Invoker
{
public:
	Invoker(std::function<void()> &fnCallback)
		: m_fnCallback(fnCallback)
	{
	}

	~Invoker()
	{
		cancel();
	}

	void invoke()
	{
		gcAssert(!m_bCallbackHit);

		std::lock_guard<std::mutex> guard(m_Lock);
		m_bCallbackHit = true;

		if (m_fnCallback)
			m_fnCallback();

		m_pHelper.done();
	}

	void cancel()
	{
		if (m_bCallbackHit)
			return;

		std::lock_guard<std::mutex> guard(m_Lock);

		if (!m_bCallbackHit)
		{
			m_fnCallback = std::function<void()>();
			m_pHelper.done();
		}
	}

	void wait()
	{
		m_pHelper.wait();
	}

private:
	std::atomic<bool> m_bCallbackHit;
	std::mutex m_Lock;
	EventHelper m_pHelper;
	std::function<void()> m_fnCallback;
};

class wxGuiDelegateEvent : public wxNotifyEvent
{
public:
	wxGuiDelegateEvent();
	wxGuiDelegateEvent(std::shared_ptr<Invoker> &invoker, int winId);
	wxGuiDelegateEvent(const wxGuiDelegateEvent& event);

	~wxGuiDelegateEvent();

	wxEvent *Clone() const override;
	void invoke();

private:
	std::shared_ptr<Invoker> m_pDelegate;
	DECLARE_DYNAMIC_CLASS(wxGuiDelegateEvent);
};

wxDECLARE_EVENT(wxEVT_GUIDELEGATE, wxGuiDelegateEvent);

template <typename T>
class wxGuiDelegateImplementation : public T
{
public:
	wxGuiDelegateImplementation(wxWindow *parent)
		: T(parent)
	{
		this->Bind(wxEVT_GUIDELEGATE, &wxGuiDelegateImplementation::onEventCallBack, this);
	}

	wxGuiDelegateImplementation(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
		: T(parent, id, pos, size, style)
	{
		this->Bind(wxEVT_GUIDELEGATE, &wxGuiDelegateImplementation::onEventCallBack, this);
	}

	wxGuiDelegateImplementation(wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
		: T(parent, id, title, pos, size, style)
	{
		this->Bind(wxEVT_GUIDELEGATE, &wxGuiDelegateImplementation::onEventCallBack, this);
	}

	~wxGuiDelegateImplementation()
	{
		this->Unbind(wxEVT_GUIDELEGATE, &wxGuiDelegateImplementation::onEventCallBack, this);
		cleanUpEvents();
	}

	void registerDelegate(InvokeI* d)
	{
		doDeregisterDelegate(d);

		std::lock_guard<std::mutex> guard(m_ListLock);
		m_vDelgateList.push_back(d);
	}

	void deregisterDelegate(InvokeI* d)
	{
		bool bFound = doDeregisterDelegate(d);
		gcAssert(bFound);
	}

	void cleanUpEvents()
	{
		std::lock_guard<std::mutex> guard(m_ListLock);

		for (auto d : m_vDelgateList)
			d->cancel(false);

		m_vDelgateList.clear();
	}

private:
	bool doDeregisterDelegate(InvokeI* d)
	{
		std::lock_guard<std::mutex> guard(m_ListLock);

		auto it = std::remove(begin(m_vDelgateList), end(m_vDelgateList), d);

		if (it == end(m_vDelgateList))
			return false;

		m_vDelgateList.erase(it, end(m_vDelgateList));
		return true;
	}

	void onEventCallBack(wxGuiDelegateEvent& event)
	{
		event.invoke();
	}

	std::mutex m_ListLock;
	std::vector<InvokeI*> m_vDelgateList;
};


class RefParamsI
{
public:
	virtual ~RefParamsI(){}
};

template <typename T>
class RefParam : public RefParamsI
{
public:
	typedef typename std::remove_const<typename std::remove_reference<T>::type>::type NonConstType;

	RefParam(T t)
		: m_T(t)
	{
	}

	virtual ~RefParam(){}

	NonConstType m_T;
};

template <typename TObj, typename ... Args>
class GuiDelegate : public DelegateBase<Args...>, public InvokeI
{
public:
	GuiDelegate(std::function<void(Args&...)> callback, uint64 compareHash, TObj *pObj, MODE mode, const char* szFile, uint32 nLine)
		: DelegateBase<Args...>(callback, compareHash)
		, m_Mode(mode)
		, m_pObj(pObj)
#ifdef DEBUG
		, m_szFile(szFile)
		, m_nLine(nLine)
#endif
	{
		gcAssert(m_pObj);

		if (m_pObj)
			m_pObj->registerDelegate(this);
	}

	~GuiDelegate()
	{
		cancel();
	}

	bool isValid() override
	{
		return !m_bCanceled;
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

		cancelPendingInvokers();

		if (m_pObj && bDeregister)
			m_pObj->deregisterDelegate(this);

		m_pObj = nullptr;
	}

	DelegateI<Args...>* clone() override
	{
#ifdef DEBUG
		return new GuiDelegate(DelegateBase<Args...>::m_fnCallback, DelegateBase<Args...>::getCompareHash(), m_pObj, m_Mode, m_szFile, m_nLine);
#else
		return new GuiDelegate(DelegateBase<Args...>::m_fnCallback, DelegateBase<Args...>::getCompareHash(), m_pObj, m_Mode, "", 0);
#endif
	}

	void callback(Args& ... args)
	{
		if (m_bCanceled)
			return;

		DelegateBase<Args...>::operator()(args...);
	}

	void operator()(Args&... args) override
	{	
		if (m_bCanceled)
			return;

		if (m_Mode == MODE_PENDING)
		{
			std::function<void()> pcb = std::bind(&GuiDelegate<TObj, Args...>::callback, this, args...);

			auto invoker = std::make_shared<Invoker>(pcb);
			addPendingInvoker(invoker);

			auto event = new wxGuiDelegateEvent(invoker, m_pObj->GetId());
			m_pObj->GetEventHandler()->QueueEvent(event);
		}
		else if (m_Mode == MODE_PROCESS || Thread::BaseThread::GetCurrentThreadId() == GetMainThreadId())
		{
			DelegateBase<Args...>::operator()(args...);
		}
		else if (m_Mode == MODE_PENDING_WAIT)
		{
			std::function<void()> pcb = std::bind(&GuiDelegate<TObj, Args...>::callback, this, std::ref(args)...);

			auto invoker = std::make_shared<Invoker>(pcb);
			auto event = new wxGuiDelegateEvent(invoker, m_pObj->GetId());
			m_pObj->GetEventHandler()->QueueEvent(event);

			setInvoker(invoker);
			invoker->wait();
			setInvoker(std::shared_ptr<Invoker>());
		}
	}

protected:
	void setInvoker(const std::shared_ptr<Invoker> &i)
	{
		std::lock_guard<std::mutex> guard(m_InvokerMutex);
		m_pInvoker = i;
	}

	void addPendingInvoker(const std::shared_ptr<Invoker> &i)
	{
		std::lock_guard<std::mutex> guard(m_InvokerMutex);
		removePendingExpiredInvokers();
		m_vPendingInvokers.push_back(i);
	}

	void cancelPendingInvokers()
	{
		removePendingExpiredInvokers();

		for (auto i : m_vPendingInvokers)
		{
			auto invoker = i.lock();

			if (!invoker)
				continue;

			invoker->cancel();
		}

		m_vPendingInvokers.clear();
	}

	void removePendingExpiredInvokers()
	{
		m_vPendingInvokers.remove_if([](std::weak_ptr<Invoker> &invoker){
			return invoker.expired();
		});
	}

private:
	MODE m_Mode;
	TObj *m_pObj = nullptr;
	std::atomic<bool> m_bCanceled;
	std::mutex m_InvokerMutex;
	std::shared_ptr<Invoker> m_pInvoker;

	std::list<std::weak_ptr<Invoker>> m_vPendingInvokers;

#ifdef DEBUG
	const char* m_szFile = nullptr;
	uint32 m_nLine = 0;
#endif
};


template <typename TObj>
inline bool validateForm(TObj* pObj)
{
	gcPanel* pan = dynamic_cast<gcPanel*>(pObj);
	gcFrame* frm = dynamic_cast<gcFrame*>(pObj);
	gcDialog* dlg = dynamic_cast<gcDialog*>(pObj);
	gcScrolledWindow* swin = dynamic_cast<gcScrolledWindow*>(pObj);
	gcTaskBarIcon* gtbi = dynamic_cast<gcTaskBarIcon*>(pObj);

	return (pan || frm || dlg || swin || gtbi);
}

#ifdef DEBUG
#define guiDelegate( ... ) guiDelegateImpl(__FILE__, __LINE__, __VA_ARGS__)
#define guiExtraDelegate( ... ) guiExtraDelegateImpl(__FILE__, __LINE__, __VA_ARGS__)
#else
#define guiDelegate( ... ) guiDelegateImpl("", 0, __VA_ARGS__)
#define guiExtraDelegate( ... ) guiExtraDelegateImpl("", 0, __VA_ARGS__)
#endif


template <typename TObj, typename ... Args>
DelegateI<Args...>* guiDelegateImpl(const char* szFile, uint32 nLine, TObj * pObj, void (TObj::*fnCallback)(Args...), MODE mode = MODE_PENDING)
{
	if (!validateForm(pObj))
	{
		gcAssert(false);
		return nullptr;
	}

	std::function<void(Args...)> callback = [pObj, fnCallback](Args...args)
	{
		(*pObj.*fnCallback)(args...);
	};

	return new GuiDelegate<TObj, Args...>(callback, MakeUint64(pObj, (void*)&typeid(fnCallback)), pObj, mode, szFile, nLine);
}

template <typename TObj, typename ... Args, typename TExtra>
DelegateI<Args...>* guiExtraDelegateImpl(const char* szFile, uint32 nLine, TObj* pObj, void (TObj::*fnCallback)(TExtra, Args...), TExtra tExtra, MODE mode = MODE_PENDING)
{
	if (!validateForm(pObj))
	{
		gcAssert(false);
		return nullptr;
	}

	std::function<void(Args...)> callback = [pObj, fnCallback, tExtra](Args...args)
	{
		(*pObj.*fnCallback)(tExtra, args...);
	};

	return new GuiDelegate<TObj, Args...>(callback, MakeUint64(pObj, (void*)&typeid(fnCallback)), pObj, mode, szFile, nLine);
}

#endif //DESURA_GUIDELEGATE_H
