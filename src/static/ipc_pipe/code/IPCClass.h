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

#ifndef DESURA_IPCClass_H
#define DESURA_IPCClass_H
#ifdef _WIN32
#pragma once
#endif

#include "Common.h"
#include "IPCClass.h"
#include "IPCParameterI.h"
#include "IPCParameter.h"
#include "IPCLockable.h"
#include "IPCMessage.h"
#include <atomic>

namespace IPC
{

	class IPCManager;

#if defined(WIN32) && !defined(__MINGW32__)
#define REG_FUNCTION_VOID_T( class, name, trace ) registerFunction( networkFunctionV(this, &class##::##name,  #class "::" #name, trace ), #name );
#define REG_FUNCTION_VOID( class, name ) registerFunction( networkFunctionV(this, &class##::##name,  #class "::" #name  ), #name );
#define REG_FUNCTION( class, name ) registerFunction( networkFunction(this, &class##::##name,  #class "::" #name ), #name );
#else
#define REG_FUNCTION_VOID_T( class, name, trace ) registerFunction( networkFunctionV(this, &class::name,  #class "::" #name, trace ), #name );
#define REG_FUNCTION_VOID( class, name ) registerFunction( networkFunctionV(this, &class::name,  #class "::" #name ), #name );
#define REG_FUNCTION( class, name ) registerFunction( networkFunction(this, &class::name,  #class "::" #name ), #name );
#endif

	char* serializeList(std::vector<IPCParameterI*> &list, uint32 &size);
	uint32 deserializeList(std::vector<IPCParameterI*> &list, const char* buffer, uint32 size);


	class NetworkFunctionI
	{
	public:
		virtual ~NetworkFunctionI(){ ; }

		virtual IPCParameterI* call(char* buff, uint32 size, uint8 numP) = 0;
		virtual IPCParameterI* returnVal(char* buff, uint32 size) = 0;
	};


	class BaseNetworkFunction : public NetworkFunctionI
	{
	protected:
		BaseNetworkFunction(const char* szName, bool bTrace = true)
			: m_strFunctionName(szName)
			, m_bTrace(bTrace)
		{
		}

		template <typename A>
		IPCParameterI* setupParam(std::vector<IPCParameterI*> &vPList, const std::vector<IPCParameter*> &vParamList, uint32 &x)
		{
			IPCParameterI* pParam = IPC::getParameter<A>();
			auto msg = vParamList[x];

			if (msg)
			{
				if (msg->type != pParam->getType())
					gcAssert(false);
				else
					pParam->deserialize(&msg->data, msg->size);
			}

			vPList[x] = pParam;

#ifdef __clang__
			++x;
#else
			--x;
#endif
			return pParam;
		}

		template <typename A>
		IPCParameterI* popElement(const std::vector<IPCParameterI*> &vPList, uint32 &x)
		{
#ifdef __clang__
			size_t nPos = x;
			++x;
			return vPList[nPos];
#else
			return vPList[--x];
#endif
		}

		template <typename ... Args>
		void emptyTemplateFunction(Args && ... args)
		{
		}

		template <typename ... Args>
		IPC::IPCParameterI* processArgs(char* buff, uint32 size, uint8 numP, std::vector<IPCParameterI*> &vPList)
		{
			if (numP != sizeof...(Args))
			{
				gcException e(ERR_INVALID, gcString("Number of arguments {1} calling {0} did not match expected {2}\n", m_strFunctionName, numP, sizeof...(Args)));
				return IPC::newParameter(e);
			}

			std::vector<IPCParameter*> vParamList;

			uint32 done = 0;

			for (int x = 0; x < sizeof...(Args); ++x)
			{
				if (done >= size)
				{
					Warning("Failed to decode IPC argument {0} for {1}, not enough data.", x, m_strFunctionName);

					vParamList.push_back(nullptr);
					vPList.push_back(nullptr);
				}
				else
				{
					auto msg = (IPCParameter*)buff;
					auto s = sizeofStruct(msg);

					done += s;
					buff += s;

					vParamList.push_back(msg);
					vPList.push_back(nullptr);
				}
			}

#ifdef __clang__
			uint32 x = 0;
#else
			uint32 x = vPList.size() - 1;
#endif
			emptyTemplateFunction(setupParam<Args>(vPList, vParamList, x)...);

			return nullptr;
		}

		const bool m_bTrace = true;
		const gcString m_strFunctionName;
	};


	template <typename R, typename ... Args>
	class NetworkFunction : public BaseNetworkFunction
	{
	public:
		NetworkFunction(const std::function<R(Args...)> &callback, const char* szName)
			: BaseNetworkFunction(szName)
			, m_fnCallback(callback)
		{
		}

		IPCParameterI* call(char* buff, uint32 size, uint8 numP)
		{
			std::vector<IPCParameterI*> vPList;
			IPCParameterI* ret = processArgs<Args...>(buff, size, numP, vPList);

			if (ret)
				return ret;

			try
			{
				TraceT(m_strFunctionName.c_str(), this, "");

#ifdef __clang__
				uint32 x = 0;
#else
				uint32 x = vPList.size();
#endif
				auto res = callFunction(popElement<Args>(vPList, x)...);

				ret = IPC::getParameter(res, true);
			}
			catch (gcException &e)
			{
				ret = IPC::newParameter(e);
			}

			safe_delete(vPList);

			if (!ret)
				return IPC::getParameter<R>();
			else
				return ret;
		}

		IPCParameterI* returnVal(char* buff, uint32 size)
		{
			IPCParameterI *r = IPC::getParameter<R>();
			r->deserialize(buff, size);
			return r;
		}

	protected:
		template <typename ... Params>
		R callFunction(Params&&...params)
		{
			return m_fnCallback(getParameterValue<Args>(params)...);
		}

	private:
		const gcString m_strFunctionName;
		std::function<R(Args...)> m_fnCallback;
	};


	template <typename ... Args>
	class NetworkFunctionVoid : public BaseNetworkFunction
	{
	public:
		NetworkFunctionVoid(const std::function<void(Args...)> &callback, const char* szName, bool bTrace = true)
			: BaseNetworkFunction(szName, bTrace)
			, m_fnCallback(callback)
		{
		}

		IPCParameterI* call(char* buff, uint32 size, uint8 numP)
		{
			std::vector<IPCParameterI*> vPList;
			IPCParameterI* ret = processArgs<Args...>(buff, size, numP, vPList);

			if (ret)
				return ret;

			try
			{
				if (m_bTrace)
					TraceT(m_strFunctionName.c_str(), this, "");

#ifdef __clang__
				uint32 x = 0;
#else
				uint32 x = vPList.size();
#endif
				callFunction(popElement<Args>(vPList, x)...);

			}
			catch (gcException &e)
			{
				safe_delete(vPList);
				return IPC::newParameter(e);
			}

			safe_delete(vPList);
			return new IPC::PVoid();
		}

		IPCParameterI* returnVal(char* buff, uint32 size)
		{
			return new PVoid();
		}

	protected:
		template <typename ... Params>
		void callFunction(Params&&...params)
		{
			m_fnCallback(getParameterValue<Args>(params)...);
		}

	private:
		std::function<void(Args...)> m_fnCallback;
	};



	template <typename TObj, typename ... Args>
	NetworkFunctionI* networkFunctionV(TObj* pObj, void(TObj::*func)(Args...), const char* szName, bool bTrace = true)
	{
		std::function<void(Args...)> fnCallback = [pObj, func](Args...args) -> void
		{
			(*pObj.*func)(args...);
		};

		return new NetworkFunctionVoid<Args...>(fnCallback, szName, bTrace);
	}

	template <typename TObj, typename R, typename ... Args>
	NetworkFunctionI* networkFunction(TObj* pObj, R(TObj::*func)(Args...), const char* szName)
	{
		std::function<R(Args...)> fnCallback = [pObj, func](Args...args) -> R
		{
			return (*pObj.*func)(args...);
		};

		return new NetworkFunction<R, Args...>(fnCallback, szName);
	}










	class IPCEventI;
	class IPCManagerI;



	//! Represents a Inter process communication class
	//!
	class IPCClass : public IPCLockable
	{
	public:
		//! Constructor
		//! 
		//! @param mang Manager
		//! @param id Class id
		//! @param itemId Active item id (used for filtering)
		//!
		IPCClass(IPCManagerI* mang, uint32 id, DesuraId itemId);

		//! DeConstructor
		//!
		~IPCClass();

		//! Gets the class id
		//!
		//! @return Class id
		//!
		uint32 getId(){ return m_uiId; }

		//! Send a message to the other side. Needs to be public due to event delegate
		//!
		//! @param type Message type
		//! @param buff Message buffer
		//! @param size Message size
		//!
		void sendMessage(uint8 type, const char* buff, uint32 size);

		//! Send a message to this side from IPC thread. Needs to be public due to event delegate
		//!
		//! @param type Message type
		//! @param buff Message buffer
		//! @param size Message size
		//!
		void sendLoopbackMessage(uint8 type, const char* buff, uint32 size);

		//! Gets the item id for filtering
		//!
		//! @return Item Id
		//!
		DesuraId getItemId(){ return m_uiItemId; }

		//! Deletes this class and the same class on the other side
		//!
		void destroy();


		//! Calls a function on the other side of the IPC. Needs to be public due to helper function. Shouldnt be called directly
		//!
		//! @param name Function name
		//! @param async Do an async function call (immediate void return)
		//! @param a Parameter one
		//! @param b Parameter two
		//! @param c Parameter three
		//! @param d Parameter four
		//! @param e Parameter five
		//! @param f Parameter six
		//! @return Result
		//!
		IPCParameterI* callFunction(const char* name, bool async = false, IPCParameterI* a = nullptr, IPCParameterI* b = nullptr, IPCParameterI* c = nullptr, IPCParameterI* d = nullptr, IPCParameterI* e = nullptr, IPCParameterI* f = nullptr);

		//! Calls a function on the other side of the IPC. Needs to be public due to helper function. Shouldnt be called directly
		//!
		//! @param name Function name
		//! @param async Do an async function call (immediate void return)
		//! @param pList Parameter list
		//! @return Result
		//!
		virtual IPCParameterI* callFunction(const char* name, bool async, std::vector<IPCParameterI*> &pList);


		//! Calls a function on this side from IPC thread. Needs to be public due to helper function. Shouldnt be called directly
		//!
		//! @param name Function name
		//! @param async Do an async function call (immediate void return)
		//! @param a Parameter one
		//! @param b Parameter two
		//! @param c Parameter three
		//! @param d Parameter four
		//! @param e Parameter five
		//! @param f Parameter six
		//! @return Result
		//!
		IPCParameterI* callLoopback(const char* name, bool async = false, IPCParameterI* a = nullptr, IPCParameterI* b = nullptr, IPCParameterI* c = nullptr, IPCParameterI* d = nullptr, IPCParameterI* e = nullptr, IPCParameterI* f = nullptr);

		//! Calls a function on this side from IPC thread. Needs to be public due to helper function. Shouldnt be called directly
		//!
		//! @param name Function name
		//! @param async Do an async function call (immediate void return)
		//! @param pList Parameter list
		//! @return Result
		//!
		virtual IPCParameterI* callLoopback(const char* name, bool async, std::vector<IPCParameterI*> &pList);



	protected:
		//! Registers a IPC callable function
		//!
		//! @param funct Function callback handle
		//! @param name Function name
		//!
		virtual void registerFunction(NetworkFunctionI* funct, const char* name);


		//! Registers a IPC event
		//!
		//! @param e Event callback handle
		//! @param name Event name
		//!
		void registerEvent(IPCEventI* e, const char* name);


		//! Sets the item id for filtering. Passes this onto every constucted class
		//!
		//! @param itemId Item id
		//!
		void setItemId(DesuraId itemId){ m_uiItemId = itemId; }

		IPCManagerI* m_pManager;	//!< Manager


		//! Handles a new message from other side
		//!
		//! @param type Message type
		//! @param buff Message buffer
		//! @param size Message size
		//!
		virtual void messageRecived(uint8 type, const char* buff, uint32 size);

		//! Handles a function call from the other side
		//!
		//! @param buff Message buffer
		//! @param size Message size
		//! @param async Do an async function call (no return)
		//!
		virtual void handleFunctionCall(const char* buff, uint32 size, bool async = false);

		//! Handles a function return from the other side
		//!
		//! @param buff Message buffer
		//! @param size Message size
		//!
		virtual void handleFunctionReturn(const char* buff, uint32 size);

		//! Handles a event trigger from the other side
		//!
		//! @param buff Message buffer
		//! @param size Message size
		//!
		virtual void handleEventTrigger(const char* buff, uint32 size);

	protected:
		IPCParameterI* doHandleFunctionCall(const char* buff, uint32 size, uint32 &nFunctionId, uint32 &nFunctionHash);

	private:
		const uint32 m_uiId;
		DesuraId m_uiItemId;

		std::atomic<int> m_nKillCount;				
		::Thread::WaitCondition m_KillCondition;

		std::map<uint32, NetworkFunctionI*> m_mFunc;
		std::map<uint32, IPCEventI*> m_mEvent;

		friend class IPCManager;
		friend class ProcessThread;
	};








	//need to reg the event on the receiver end
#define REG_EVENT( event ) registerEvent( IPC::IPCEventHandle( &event ), #event );
#define REG_EVENT_VOID( event ) registerEvent( new IPC::IPCEventV( &event ), #event );

	//need to link the event on the sender end. Do not link on both ends other wise it will get in a loop
#define LINK_EVENT( event, type ) event += new IPC::IPCDelegate< type& >( this , #event );
#define LINK_EVENT_VOID( event ) event += new IPC::IPCDelegate<>( this , #event );


	class IPCEventI
	{
	public:
		virtual ~IPCEventI(){ ; }
		virtual void trigger(std::vector<IPCParameterI*> &vParams) = 0;
	};


	template <typename E>
	class IPCEvent : public IPCEventI
	{
	public:
		IPCEvent(Event<E>* e)
		{
			m_pEvent = e;
		}

		void trigger(std::vector<IPCParameterI*> &vParams)
		{
			if (vParams.size() != 1)
			{
				gcAssert(false);
				return;
			}

			E ret = IPC::getParameterValue<E>(vParams[0]);
			m_pEvent->operator()(ret);
		}

	private:
		Event<E> *m_pEvent;
	};


	class IPCEventV : public IPCEventI
	{
	public:
		IPCEventV(EventV* e)
		{
			m_pEvent = e;
		}

		void trigger(std::vector<IPCParameterI*> &vParams)
		{
			m_pEvent->operator()();
		}

	private:
		EventV *m_pEvent;
	};

	template <typename T>
	IPCEventI* IPCEventHandle(Event<T> *e)
	{
		return new IPC::IPCEvent<T>(e);
	}



	template <typename ... Args>
	void emptyTemplateFunction(Args && ... args)
	{
	}

	template <typename ... Args>
	class IPCDelegate : public DelegateI<Args...>
	{
	public:
		IPCDelegate(IPCClass* c, const char* name)
		{
			m_uiHash = UTIL::MISC::RSHash_CSTR(name);
			m_pClass = c;
		}

		IPCDelegate(IPCClass* c, uint32 hash)
		{
			m_uiHash = hash;
			m_pClass = c;
		}

		bool equals(DelegateI<Args...>* di) override
		{
			return di->getCompareHash() == getCompareHash();
		}

		DelegateI<Args...>* clone() override
		{
			return new IPCDelegate<Args...>(m_pClass, m_uiHash);
		}

		void destroy() override
		{
			delete this;
		}

		uint64 getCompareHash() const override
		{
			return MakeUint64(m_pClass, (void*)m_uiHash);
		}

		void operator()(Args&... a)
		{
			std::deque<IPCParameterI*> dqParams;
			emptyTemplateFunction<Args...>(pushParms<Args>(a, dqParams)...);

			gcAssert(dqParams.size() <= 1);

			std::vector<IPCParameterI*> vParams(begin(dqParams), end(dqParams));

			uint32 size = 0;
			const char* data = serializeList(vParams, size);

			char* buff = new char[size + IPCEventTriggerSIZE];

			IPCEventTrigger *et = (IPCEventTrigger*)buff;
			et->size = size;
			et->eventHash = m_uiHash;

			memcpy(&et->data, data, size);

			m_pClass->sendMessage(MT_EVENTTRIGGER, (const char*)et, sizeofStruct(et));

			safe_delete(buff);
			safe_delete(data);
			safe_delete(vParams);
		}

	protected:
		template <typename U>
		U pushParms(U &u, std::deque<IPCParameterI*> &vParams)
		{
			vParams.push_front(IPC::getParameter(u));
			return u;
		}

	private:
		uint32 m_uiHash;
		IPCClass* m_pClass;
	};





	template <typename R>
	R handleReturn(IPC::IPCParameterI* r)
	{
		if (r->getType() == PException::getTypeS())
		{
			gcException gce((gcException*)r->getValue());
			safe_delete(r);
			throw gce;
		}

		if (r->getType() == PVoid::getTypeS())
			throw gcException();

		R temp = IPC::getParameterValue<R>(r, true);
		safe_delete(r);
		return temp;
	}

	inline void handleReturnV(IPC::IPCParameterI* r)
	{
		if (r->getType() == PException::getTypeS())
		{
			gcException gce((gcException*)r->getValue());
			safe_delete(r);
			throw gce;
		}

		safe_delete(r);
		return;
	}


	template <typename R, typename ... Args>
	R functionCall(IPC::IPCClass* cl, const char* name, Args& ... args)
	{
		IPC::IPCParameterI* r = cl->callFunction(name, false, IPC::getParameter(args)...);
		return handleReturn<R>(r);
	}


	template <typename ... Args>
	void functionCallV(IPC::IPCClass* cl, const char* name, Args& ... args)
	{
		IPC::IPCParameterI* r = cl->callFunction(name, false, IPC::getParameter(args)...);
		handleReturnV(r);
	}

	template <typename ... Args>
	void functionCallAsync(IPC::IPCClass* cl, const char* name, Args&& ... args)
	{
		try
		{
			IPC::IPCParameterI* r = cl->callFunction(name, true, IPC::getParameter(args)...);
			handleReturnV(r);
		}
		catch (gcException &e)
		{
			if (std::string("message") != name)
				WarningS("Unhandled exception calling {0} async: {1}", name, e);
		}
		catch (...)
		{
			if (std::string("message") != name)
				WarningS("Unhandled unknown exception calling {0} async.", name);
		}
	}

	template <typename ... Args>
	void loopbackCallAsync(IPC::IPCClass* cl, const char* name, Args& ... args)
	{
		try
		{
			IPC::IPCParameterI* r = cl->callLoopback(name, true, IPC::getParameter(args)...);
			handleReturnV(r);
		}
		catch (gcException &e)
		{
			if (std::string("message") != name)
				WarningS("Unhandled exception calling {0} async: {1}", name, e);
		}
		catch (...)
		{
			if (std::string("message") != name)
				WarningS("Unhandled unknown exception calling {0} async.", name);
		}
	}
}

#endif //DESURA_IPCClass_H
