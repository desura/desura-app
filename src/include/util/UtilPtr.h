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

#ifndef DESURA_UTILPTR_H
#define DESURA_UTILPTR_H




class gcRefBase
{
public:
	virtual int addRef() = 0;
	virtual int delRef() = 0;
	virtual int getRefCt() = 0;

protected:
	virtual ~gcRefBase() {}
};

#include <atomic>

class gcRefCount : public gcRefBase
{
public:
	int addRef() override
	{
		return ++m_nRef;
	}

	int delRef() override
	{
		return --m_nRef;
	}

	int getRefCt() override
	{
		return m_nRef;
	}

private:
	std::atomic<int> m_nRef;
};


#define gc_IMPLEMENT_REFCOUNTING(ClassName)				\
	public:												\
	int addRef() { return m_RefCount.addRef(); }    \
	int delRef() {                                 \
	int retval = m_RefCount.delRef();          \
	if (retval == 0)                            \
	delete this;                            \
	return retval;                              \
}                                               \
	int getRefCt() { return m_RefCount.getRefCt(); }\
	private:											\
	gcRefCount m_RefCount;

#define gc_REFPTR_DESTRUCTOR(ClassName) 

template <class T>
class gcRefPtr
{
public:
	typedef void(*gcRefBaseFn)(T*, bool);

	gcRefPtr()
	{
	}

	gcRefPtr(T* p)
		: m_pPtr(p)
	{
		m_pRefFn = [](T* pPtr, bool bAdd)
		{
			if (!pPtr)
				return;

			if (bAdd)
				pPtr->addRef();
			else
				pPtr->delRef();
		};

		m_pRefFn(p, true);
	}

	gcRefPtr(T* p, gcRefBaseFn refBaseFn)
		: m_pPtr(p)
		, m_pRefFn(refBaseFn)
	{
		m_pRefFn(p, true);
	}

	gcRefPtr(const gcRefPtr<T>& r)
		: m_pPtr(r.m_pPtr)
		, m_pRefFn(r.m_pRefFn)
	{
		m_pRefFn(m_pPtr, true);
	}

	~gcRefPtr()
	{
		m_pRefFn(m_pPtr, false);
	}

	T* get() const
	{
		return m_pPtr;
	}

	operator T*() const
	{
		gcAssert(m_pPtr);
		return m_pPtr;
	}

	template <typename U>
	operator gcRefPtr<U>()
	{
		return gcRefPtr<U>(m_pPtr, (gcRefPtr<U>::gcRefBaseFn)m_pRefFn);
	}

	T* operator->() const
	{
		gcAssert(m_pPtr);
		return m_pPtr;
	}

	gcRefPtr<T>& operator=(T* p)
	{
		m_pRefFn(p, true);
		m_pRefFn(m_pPtr, false);
		m_pPtr = p;
		return *this;
	}

	gcRefPtr<T>& operator=(const gcRefPtr<T>& r)
	{
		return *this = r.m_pPtr;
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
	static gcRefPtr<T> dyn_cast(gcRefPtr<U> &u)
	{
		T* pT = dynamic_cast<T*>(u.get());

		if (pT)
			return gcRefPtr<T>(pT, (gcRefBaseFn)u.m_pRefFn);

		return gcRefPtr<T>();
	}

	int getRefCt() const
	{
		if (m_pPtr)
			return m_pPtr->getRefCt();

		return 0;
	}

private:
	template <typename U>
	friend class gcRefPtr;

	T* m_pPtr = nullptr;
	gcRefBaseFn m_pRefFn = nullptr;
};

#define gc_MOCK_REFCOUNTING(ClassName)		\
	int addRef() { return 1; }				\
	int delRef() { return 1; }				\
	int getRefCt() { return 1; }


#endif

