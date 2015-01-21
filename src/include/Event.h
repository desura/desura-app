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

//   Description :
//      Event Class is a multi purpose event system where by other classes/functions or events
//		can register interest in the event (by delegates) and receive notifications when the
//		event triggers.
//
//		To create a new event just use the template class Event for parameter events or EventV
//		for void events.
//
//		I.e. Event<int> myIntEvent; Has one int parameter
//			 EventV myVoidEvent;	Has no parameters
//
//		To register interest in an event use the overloaded += operator. I.e. for:
//			*Object		event += delegate( this, &Class::Function );
//			*Function	event += delegate( &Function );
//			*Event		event += delegate( &otherEvent );
//
//		Functions must be in the form: void Name([Type]& [pname]) where type is the same type
//		used to define the event or for void Events: void Name()
//
//		To trigger an event use the operator(). I.e. myIntEvent( intVar ) or myVoidEvent()


#ifndef DESURA_EVENT_H
#define DESURA_EVENT_H
#ifdef _WIN32
#pragma once
#endif

#include <vector>
#include <algorithm>

#ifndef UNKNOWN_ITEM
#define UNKNOWN_ITEM 0xFFFFFFFF
#endif

#ifdef NIX
#include <typeinfo>
#endif

#include <functional>
#include <mutex>

class VoidEventArg
{
};

template <typename ... Args>
class DelegateI
{
public:
	virtual void operator()(Args&... a) = 0;
	virtual bool equals(DelegateI<Args...>* d) = 0;
	virtual DelegateI<Args...>* clone() = 0;
	virtual uint64 getCompareHash() const = 0;
	virtual void destroy() = 0;

	//override this to auto remove invalid delegates
	virtual bool isValid()
	{
		return true;
	}

protected:
	virtual ~DelegateI(){};
};

class InvokeI
{
public:
	virtual void cancel(bool bDeregister = true) = 0;

protected:
	virtual ~InvokeI(){}
};

template <typename TArg, typename TDel>
class EventBase
{
public:
	EventBase()
	: m_bCancel(false)
	, m_pCurDelegate(nullptr)
	, m_vPendingDelegates()
	, m_vDelegates()
	, m_PendingLock()
	, m_Lock()
	{
		assertType();
	}

	EventBase(const EventBase<TArg, TDel>& e)
	{
		assertType();

		for (size_t x=0; x<e.m_vDelegates.size(); x++)
			m_vDelegates.push_back(e.m_vDelegates[x]->clone());

		m_bCancel = false;
		m_pCurDelegate = nullptr;
	}

	virtual ~EventBase()
	{
		try
		{
			reset();
		}
		catch (...)
		{
		}
	}

	void assertType()
	{
		//shouldnt be using char* and wchar_t* in events as they can leak memory. Use gcString instead
		gcAssert(typeid(TArg) != typeid(char*));
		gcAssert(typeid(TArg) != typeid(wchar_t*));
		gcAssert(typeid(TArg) != typeid(const char*));
		gcAssert(typeid(TArg) != typeid(const wchar_t*));
	}

	void operator()(TArg a)
	{
		//cant use this with void event
		gcAssert(typeid(TArg) != typeid(VoidEventArg));

		std::lock_guard<std::recursive_mutex> guard(m_Lock);
		migratePending();

		for (auto d : m_vDelegates)
		{
			if (!d)
				continue;

			m_pCurDelegate = d;
			m_pCurDelegate->operator()(a);
			m_pCurDelegate = nullptr;

			if (m_bCancel)
				break;
		}

		migratePending();
	}

	void operator()()
	{
		std::lock_guard<std::recursive_mutex> guard(m_Lock);
		migratePending();

		for (auto d : m_vDelegates)
		{
			if (!d)
				continue;

			m_pCurDelegate = d;
			m_pCurDelegate->operator()();
			m_pCurDelegate = nullptr;

			if (m_bCancel)
				break;
		}

		migratePending();
	}

	EventBase<TArg, TDel>& operator=(const EventBase<TArg, TDel>& e)
	{
		std::lock_guard<std::recursive_mutex> guard(m_Lock);

		for (auto d : m_vDelegates)
			d->destroy();

		m_vDelegates.clear();

		for (auto d : e.m_vDelegates)
			m_vDelegates.push_back(d->clone());

		return *this;
	}

	EventBase<TArg, TDel>& operator+=(const EventBase<TArg, TDel>& e)
	{
		std::lock_guard<std::recursive_mutex> guard(m_Lock);

		for (auto d : e.m_vDelegates)
		{
			if (findInfo(d) == UNKNOWN_ITEM)
				m_vDelegates.push_back(d->clone());
		}

		return *this;
	}

	EventBase<TArg, TDel>& operator-=(const EventBase<TArg, TDel>& e)
	{
		std::lock_guard<std::recursive_mutex> guard(m_Lock);

		std::vector<size_t> del;

		for (size_t x=0; x<e.m_vDelegates.size(); x++)
		{
			TDel* d = e.m_vDelegates[x];
			size_t index = findInfo(d);

			if (index == UNKNOWN_ITEM)
				continue;

			del.push_back(index);
		}

		std::sort(del.begin(), del.end());

		for (size_t x=del.size(); x>0; x--)
			m_vDelegates.erase(m_vDelegates.begin()+del[x]);

		return *this;
	}


	EventBase<TArg, TDel>& operator+=(TDel* d)
	{
		if (!d)
			return *this;

		{
			std::lock_guard<std::recursive_mutex> guard(m_PendingLock);
			m_vPendingDelegates.push_back(std::pair<bool, TDel*>(true, d->clone()));
		}

		d->destroy();

		std::unique_lock<std::recursive_mutex> uLock(m_Lock, std::defer_lock);

		if (uLock.try_lock())
			migratePending();

		return *this;
	}

	EventBase<TArg, TDel>& operator-=(TDel* d)
	{
		if (!d)
			return *this;

		{
			std::lock_guard<std::recursive_mutex> guard(m_PendingLock);
			m_vPendingDelegates.push_back(std::pair<bool, TDel*>(false, d->clone()));
		}

		d->destroy();

		std::unique_lock<std::recursive_mutex> uLock(m_Lock, std::defer_lock);

		if (uLock.try_lock())
			migratePending();

		return *this;
	}

	void reset()
	{
		if (!this)
			return;

		std::lock_guard<std::recursive_mutex> guard(m_Lock);

		m_bCancel = true;
		InvokeI* i = dynamic_cast<InvokeI*>(m_pCurDelegate);

		if (i)
			i->cancel();

		for (auto d : m_vDelegates)
		{
			if (d)
				d->destroy();
		}

		m_vDelegates.clear();

		{
			std::lock_guard<std::recursive_mutex> guard(m_PendingLock);
			for (auto p : m_vPendingDelegates)
			{
				if (p.second)
					p.second->destroy();
			}

			m_vPendingDelegates.clear();
		}

		m_bCancel = false;
	}

	void flush()
	{
		std::lock_guard<std::recursive_mutex> guard(m_Lock);
		migratePending();
	}

protected:
	size_t findInfo(TDel* d)
	{
		for (size_t x=0; x<m_vDelegates.size(); x++)
		{
			if (m_vDelegates[x]->equals(d))
				return x;
		}

		return UNKNOWN_ITEM;
	}

	void migratePending()
	{
		std::lock_guard<std::recursive_mutex> guard(m_PendingLock);

		removeInvalidDelegates();

		for (auto p : m_vPendingDelegates)
		{
			if (p.first)
			{
				if (findInfo(p.second) == UNKNOWN_ITEM)
					m_vDelegates.push_back(p.second);
				else
				{
					gcAssert(false);
					p.second->destroy();
				}
			}
			else
			{
				size_t index = findInfo(p.second);
				p.second->destroy();

				if (index != UNKNOWN_ITEM)
				{
					m_vDelegates[index]->destroy();
					m_vDelegates.erase(m_vDelegates.begin() + index);
				}
			}
		}

		m_vPendingDelegates.clear();
	}

	void removeInvalidDelegates()
	{
		std::vector<TDel*> vTemp = m_vDelegates;
		m_vDelegates.clear();

		for (auto p : vTemp)
		{
			if (!p || !p->isValid())
				p->destroy();
			else
				m_vDelegates.push_back(p);
		}
	}

private:
	std::recursive_mutex m_Lock;
	std::recursive_mutex m_PendingLock;

	std::vector<TDel*> m_vDelegates;
	std::vector<std::pair<bool, TDel*> > m_vPendingDelegates;

	bool m_bCancel = false;
	TDel* m_pCurDelegate = nullptr;
};





template <typename ... Args>
class DelegateBase : public DelegateI<Args...>
{
public:
	DelegateBase(std::function<void(Args&...)> callback, uint64 compareHash)
		: m_fnCallback(callback)
		, m_ullCompareHash(compareHash)
	{
	}

	bool equals(DelegateI<Args...>* di) override
	{
		return di->getCompareHash() == m_ullCompareHash;
	}

	DelegateI<Args...>* clone() override
	{
		return new DelegateBase<Args...>(m_fnCallback, m_ullCompareHash);
	}

	void operator()(Args&... a) override
	{
		m_fnCallback(a...);
	}

	uint64 getCompareHash() const override
	{
		return m_ullCompareHash;
	}

	void destroy() override
	{
		delete this;
	}

protected:
	std::function<void(Args&...)> m_fnCallback;
	const uint64 m_ullCompareHash;
};














template <typename TArg>
class Event : public EventBase<TArg&, DelegateI<TArg&> >
{
public:
	Event() : EventBase<TArg&, DelegateI<TArg&> >()
	{
	}

	Event(const EventBase<TArg&, DelegateI<TArg&> >& e) : EventBase<TArg, DelegateI<TArg&> >(e)
	{
	}
};

template <typename TArg>
class EventC : public EventBase<TArg, DelegateI<TArg> >
{
public:
	EventC()
		: EventBase<TArg, DelegateI<TArg> >()
	{
	}

	EventC(const EventBase<TArg, DelegateI<TArg> >& e)
		: EventBase<TArg, DelegateI<TArg> >(e)
	{
	}
};

typedef EventBase<VoidEventArg, DelegateI<>> EventV;




inline uint64 MakeUint64(void* a, void* b)
{
	return ((uint64)a) + (((uint64)b) << 32ull);
}



template <typename TObj, typename ... Args>
DelegateI<Args...>* delegate(TObj* pObj, void (TObj::*fnCallback)(Args...))
{
	std::function<void(Args...)> callback = [pObj, fnCallback](Args ... args)
	{
		(*pObj.*fnCallback)(args...);
	};

	return new DelegateBase<Args...>(callback, MakeUint64(pObj, (void*)&typeid(fnCallback)));
}

template <typename ... Args>
DelegateI<Args&...>* delegate(void(*fnCallback)(Args&...))
{
	std::function<void(Args&...)> callback = [fnCallback](Args&... args)
	{
		fnCallback(args...);
	};

	return new DelegateBase<Args&...>(callback, (uint64)fnCallback);
}


template <typename ... Args>
DelegateI<Args...>* delegate(void(*fnCallback)(Args...))
{
	std::function<void(Args&...)> callback = [fnCallback](Args&... args)
	{
		fnCallback(args...);
	};

	return new DelegateBase<Args&...>(callback, (uint64)fnCallback);
}

template <typename ... Args>
DelegateI<Args&...>* delegate(Event<Args...>* e)
{
	std::function<void(Args&...)> callback = [e](Args& ... args)
	{
		(*e)(args...);
	};

	return new DelegateBase<Args&...>(callback, (uint64)e);
}

template <typename ... Args>
DelegateI<Args...>* delegate(EventC<Args...>* e)
{
	std::function<void(Args&...)> callback = [e](Args& ... args)
	{
		(*e)(args...);
	};

	return new DelegateBase<Args...>(callback, (uint64)e);
}

inline DelegateI<>* delegate(EventV* e)
{
	std::function<void()> callback = [e]()
	{
		(*e)();
	};

	return new DelegateBase<>(callback, (uint64)e);
}

template <typename ... Args>
DelegateI<Args...>* delegate(std::function<void(Args...)> &fnCallback, uint64 hash)
{
	return new DelegateBase<Args...>(fnCallback, hash);
}

//template <typename ... Args>
//DelegateI<Args&...>* delegate(std::function<void(Args&...)> &fnCallback, uint64 ident)
//{
//	return new DelegateBase<Args...&>(fnCallback, ident);
//}

template <typename TObj, typename ... Args, typename Extra>
DelegateI<Args&...>* extraDelegate(TObj* pObj, void (TObj::*fnCallback)(Extra, Args&...), Extra extra)
{
	std::function<void(Args&...)> callback = [pObj, fnCallback, extra](Args& ... args)
	{
		(*pObj.*fnCallback)(extra, args...);
	};

	return new DelegateBase<Args&...>(callback, MakeUint64(pObj, (void*)&typeid(fnCallback)));
}


#endif //DESURA_EVENT_H
