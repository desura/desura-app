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

#ifndef DESURA_UTILPTR_H
#define DESURA_UTILPTR_H

namespace UTIL
{
	namespace OS
	{
		class StackTrace;
	}
}


class gcRefBase
{
public:
	virtual int addRef() = 0;
	virtual int delRef() = 0;
	virtual int getRefCt() = 0;

#if defined(DEBUG) && defined(WIN32)
	virtual void addStackTrace(void* pObj) = 0;
	virtual void delStackTrace(void* pObj) = 0;
	virtual void dumpStackTraces() = 0;
#endif

protected:
	virtual ~gcRefBase() {}
};

#include <atomic>
#include <memory>

class gcRefCount final
{
public:
	gcRefCount(){}
	gcRefCount(const gcRefCount&) = delete;

	int addRef()
	{
		return ++m_nRef;
	}

	int delRef()
	{
		return --m_nRef;
	}

	int getRefCt()
	{
		return m_nRef;
	}

#if defined(DEBUG) && defined(WIN32)
	void addStackTrace(void* pObj);
	void delStackTrace(void* pObj);
	void dumpStackTraces();
#endif

private:
	std::atomic<int> m_nRef = { 0 };

#if defined(DEBUG) && defined(WIN32)
	std::mutex m_Lock;
	std::map<void*, std::shared_ptr<UTIL::OS::StackTrace>> m_mStackTraces;
#endif
};

#if defined(DEBUG) && defined(WIN32)
#define gc_IMPLEMENT_REFCOUNTING_DEBUG(ClassName)	\
	void addStackTrace(void* pObj) { return m_RefCount.addStackTrace(pObj); } \
	void delStackTrace(void* pObj) { return m_RefCount.delStackTrace(pObj); } \
	void dumpStackTraces(){ m_RefCount.dumpStackTraces(); }
#else
#define gc_IMPLEMENT_REFCOUNTING_DEBUG(ClassName)
#endif


#define gc_IMPLEMENT_REFCOUNTING(ClassName)				\
	public:												\
	int addRef() { return m_RefCount.addRef(); }		\
	int delRef() {										\
	int retval = m_RefCount.delRef();					\
	if (retval == 0)									\
	delete this;										\
	return retval;										\
}														\
	int getRefCt() { return m_RefCount.getRefCt(); }	\
	gc_IMPLEMENT_REFCOUNTING_DEBUG(ClassName)			\
	private:											\
	gcRefCount m_RefCount;

template <class T>
class gcRefPtr
{
public:
	typedef void(*gcRefBaseFn)(void*, T*, bool);

	gcRefPtr()
	{
	}

	gcRefPtr(std::nullptr_t)
	{
	}

	gcRefPtr(T* p)
		: m_pPtr(p)
	{
		if (m_pPtr)
		{
			m_pRefFn = generateCleanupFunct();
			m_pRefFn(this, p, true);
		}
	}

	gcRefPtr(T* p, gcRefBaseFn refBaseFn)
		: m_pPtr(p)
		, m_pRefFn(refBaseFn)
	{
		gcAssert(!m_pPtr || (m_pPtr && m_pRefFn));

		if (m_pRefFn)
			m_pRefFn(this, m_pPtr, true);
	}

	gcRefPtr(const gcRefPtr<T>& r)
		: m_pPtr(r.m_pPtr)
		, m_pRefFn(r.m_pRefFn)
	{
		gcAssert(!m_pPtr || (m_pPtr && m_pRefFn));

		if (m_pRefFn)
			m_pRefFn(this, m_pPtr, true);
	}

	~gcRefPtr()
	{
		if (m_pRefFn)
			m_pRefFn(this, m_pPtr, false);
	}

	T* get() const
	{
		return m_pPtr;
	}

	template <typename U>
	operator gcRefPtr<U>()
	{
		return gcRefPtr<U>(m_pPtr);
	}

	T* operator->() const
	{
		gcAssert(m_pPtr);
		return m_pPtr;
	}

	T& operator*() const
	{
		gcAssert(m_pPtr);
		return *m_pPtr;
	}

	gcRefPtr<T>& operator=(T* p)
	{
		if (m_pPtr == p)
			return *this;

		if (!m_pRefFn)
			m_pRefFn = generateCleanupFunct();

		m_pRefFn(this, p, true);
		m_pRefFn(this, m_pPtr, false);
		m_pPtr = p;
		return *this;
	}

	gcRefPtr<T>& operator=(const gcRefPtr<T>& r)
	{
		return *this = r.m_pPtr;
	}

	operator bool() const
	{
		return !!m_pPtr;
	}

	void swap(T** pp)
	{
		T* p = m_pPtr;
		m_pPtr = *pp;
		*pp = p;
	}

	void swap(gcRefPtr<T>& r)
	{
		swap(&r.m_pPtr);
	}

	template <typename ... Args>
	static gcRefPtr<T> create(Args&&... args)
	{
		return gcRefPtr<T>(new T(std::forward<Args>(args)...));
	}

	template <typename U>
	static gcRefPtr<T> dyn_cast(const gcRefPtr<U> &u)
	{
		return gcRefPtr<T>(dynamic_cast<T*>(u.get()));
	}

	int getRefCt() const
	{
		if (m_pPtr)
			return m_pPtr->getRefCt();

		return 0;
	}

	void reset()
	{
		if (m_pPtr)
			m_pRefFn(this, m_pPtr, false);

		m_pPtr = nullptr;
		m_pRefFn = nullptr;
	}

	bool operator==(T *ptr) const
	{
		return m_pPtr == ptr;
	}

	bool operator==(const gcRefPtr<T> &ptr) const
	{
		return m_pPtr == ptr.m_pPtr;
	}

	template <typename U>
	bool operator==(const gcRefPtr<U> &ptr) const
	{
		return *this == gcRefPtr<T>(ptr.m_pPtr);
	}

	bool operator!=(const gcRefPtr<T> &ptr) const
	{
		return m_pPtr != ptr.m_pPtr;
	}

	template <typename U>
	bool operator!=(const gcRefPtr<U> &ptr) const
	{
		return *this != gcRefPtr<T>(ptr.m_pPtr);
	}

private:
	static gcRefBaseFn generateCleanupFunct()
	{
		return [](void* pRefPtr, T* pPtr, bool bAdd)
		{
			if (!pPtr)
				return;

			if (bAdd)
			{
#if defined(DEBUG) && defined(WIN32)
				pPtr->addStackTrace(pRefPtr);
#endif
				pPtr->addRef();
			}
			else
			{
#if defined(DEBUG) && defined(WIN32)
				pPtr->delStackTrace(pRefPtr);
#endif
				pPtr->delRef();
			}
		};
	}

	template <typename U>
	friend class gcRefPtr;

	T* m_pPtr = nullptr;
	gcRefBaseFn m_pRefFn = nullptr;
};

#endif

