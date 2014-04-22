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

#include "util_thread/BaseThread.h"

#include "IPCClass.h"
#include "IPCMessage.h"
#include "IPCManager.h"
#include "IPCParameter.h"


namespace IPC
{

typedef struct
{
	char* data;
	uint32 size;
} PS_s;

char* serializeList(std::vector<IPCParameterI*> &pList, uint32 &rsize)
{
	rsize = 0;

	for (auto p : pList)
		rsize += IPCParameterSIZE + p->getSerializeSize();

	char* buff = new char[rsize];
	char* temp = buff;

	for (auto p : pList)
	{
		IPCParameter* pStruct = (IPCParameter*)temp;

		pStruct->size = p->getSerializeSize();
		pStruct->type = p->getType();

		temp += IPCParameterSIZE;
		p->serialize(temp);
		temp += pStruct->size;
	}

	return buff;
}

uint32 deserializeList(std::vector<IPCParameterI*> &list, const char* buffer, uint32 size)
{
	uint32 sizeLeft = size;
	const char* tempB = buffer;

	while (sizeLeft > 0)
	{
		auto tempP = (IPCParameter*)tempB;
		IPCParameterI* p = getParameter(tempP->type, &tempP->data, tempP->size);

		list.push_back(p);

		tempB += tempP->size + IPCParameterSIZE;
		sizeLeft -= tempP->size + IPCParameterSIZE;
	}

	return size - sizeLeft;
}







IPCClass::IPCClass(IPCManagerI* mang, uint32 id, DesuraId itemId)
	: m_uiItemId(itemId)
	, m_pManager(mang)
	, m_uiId(id)
{
}

IPCClass::~IPCClass()
{
	safe_delete(m_mFunc);
	safe_delete(m_mEvent);
}

IPCParameterI* IPCClass::callFunction(const char* name, bool async, IPCParameterI* a, IPCParameterI* b, IPCParameterI* c, IPCParameterI* d, IPCParameterI* e, IPCParameterI* f)
{
	std::vector<IPCParameterI*> pList;

	if (a)
		pList.push_back(a);
	if (b)
		pList.push_back(b);
	if (c)
		pList.push_back(c);
	if (d)
		pList.push_back(d);
	if (e)
		pList.push_back(e);
	if (f)
		pList.push_back(f);

	IPCParameterI* res = callFunction(name, async, pList);
	safe_delete(pList);

	return res;
}

IPCParameterI* IPCClass::callFunction(const char* name, bool async, std::vector<IPCParameterI*> &pList)
{
	static gcString s_strMessage("message"); 

	if (s_strMessage != name)
	{
		gcString traceName("{0}::{1}", typeid(this).name(), name);
		TraceT(traceName.c_str(), this, "Async: {0}, ArgC: {1}", async, pList.size());
	}

	IPCFunctionCall *fch;

	uint32 tsize;
	char* data = serializeList(pList, tsize);

	char* buff = new char[tsize + IPCFunctionCallSIZE];
	fch = (IPCFunctionCall*)buff;

	fch->size = tsize;
	fch->functionHash = UTIL::MISC::RSHash_CSTR(name);
	fch->numP = pList.size();
	memcpy(&fch->data, data, tsize );
	
	safe_delete(data);

	IPCParameterI* ret = nullptr;

	if (async)
	{
		fch->id = 0;
		this->sendMessage(MT_FUNCTIONCALL_ASYNC, (const char*)fch, sizeofStruct(fch) );
		safe_delete(buff);

		ret = new PVoid();
	}
	else
	{	
		IPCScopedLock<IPCClass> lock(this, newLock());

		fch->id = lock->id;

		this->sendMessage(MT_FUNCTIONCALL, (const char*)fch, sizeofStruct(fch) );
		safe_delete(buff);

		//wait on mutex
		if (lock->wait(30, 0))
			throw gcException(ERR_IPC, "Waited too long with no response");

		ret = lock->result;
	}

	return ret;
}


IPCParameterI* IPCClass::callLoopback(const char* name, bool async, IPCParameterI* a, IPCParameterI* b, IPCParameterI* c, IPCParameterI* d, IPCParameterI* e, IPCParameterI* f)
{
	std::vector<IPCParameterI*> pList;

	if (a)
		pList.push_back(a);
	if (b)
		pList.push_back(b);
	if (c)
		pList.push_back(c);
	if (d)
		pList.push_back(d);
	if (e)
		pList.push_back(e);
	if (f)
		pList.push_back(f);

	IPCParameterI* res = callLoopback(name, async, pList);
	safe_delete(pList);

	return res;
}

IPCParameterI* IPCClass::callLoopback(const char* name, bool async, std::vector<IPCParameterI*> &pList)
{
	gcString traceName("{0}::{1}", typeid(this).name(), name);
	TraceT(traceName.c_str(), this, "Async: {0}, ArgC: {1}", async, pList.size());

	IPCFunctionCall *fch;

	uint32 tsize;
	char* data = serializeList(pList, tsize);

	char* buff = new char[tsize + IPCFunctionCallSIZE];
	fch = (IPCFunctionCall*)buff;

	fch->size = tsize;
	fch->functionHash = UTIL::MISC::RSHash_CSTR(name);
	fch->numP = pList.size();
	memcpy(&fch->data, data, tsize );
	
	safe_delete(data);

	IPCParameterI* ret = nullptr;

	if (async)
	{
		fch->id = 0;
		this->sendLoopbackMessage(MT_FUNCTIONCALL_ASYNC, (const char*)fch, sizeofStruct(fch) );
		safe_delete(buff);

		ret = new PVoid();
	}
	else
	{	
		IPCScopedLock<IPCClass> lock(this, newLock());

		fch->id = lock->id;

		this->sendLoopbackMessage(MT_FUNCTIONCALL, (const char*)fch, sizeofStruct(fch) );
		safe_delete(buff);

		//wait on mutex
		lock->wait();
		ret = lock->result;
	}

	return ret;
}

void IPCClass::registerFunction(NetworkFunctionI* funct, const char* name)
{
	uint32 hash = UTIL::MISC::RSHash_CSTR(name);

	std::map<uint32,NetworkFunctionI*>::iterator it = m_mFunc.find(hash);

	if (it == m_mFunc.end())
	{
		m_mFunc[hash] = funct;
	}
	else
	{
		safe_delete(funct);
	}	
}


void IPCClass::registerEvent(IPCEventI* e, const char* name)
{
	uint32 hash = UTIL::MISC::RSHash_CSTR(name);

	std::map<uint32,IPCEventI*>::iterator it = m_mEvent.find(hash);

	if (it == m_mEvent.end())
	{
		m_mEvent[hash] = e;
	}
	else
	{
		safe_delete(e);
	}	
}

void IPCClass::messageRecived(uint8 type, const char* buff, uint32 size)
{
	if (type == MT_FUNCTIONCALL)
	{
		handleFunctionCall(buff, size, false);
	}
	else if (type == MT_FUNCTIONCALL_ASYNC)
	{
		handleFunctionCall(buff, size, true);
	}
	else if (type == MT_FUNCTIONRETURN)
	{
		handleFunctionReturn(buff, size);
	}
	else if (type == MT_EVENTTRIGGER)
	{
		handleEventTrigger(buff,size);
	}
}

IPCParameterI* IPCClass::doHandleFunctionCall(const char* buff, uint32 size, uint32 &nFunctionId, uint32 &nFunctionHash)
{
	if (size < IPCFunctionCallSIZE)
		throw gcException(ERR_IPC, "IPC Handle Function call buffer size is too small.");

	IPCFunctionCall *fch = (IPCFunctionCall*)buff;

	if (size < fch->size + IPCFunctionCallSIZE)
	{
		Warning("IPC Handle Function call buffer size is too small. Some args might have invalid data.");
		fch->size = size - IPCFunctionCallSIZE;
	}

	nFunctionId = fch->id;
	nFunctionHash = fch->functionHash;

	std::map<uint32, NetworkFunctionI*>::iterator it;
	it = m_mFunc.find(fch->functionHash);

	if (it == m_mFunc.end())
	{
		Warning("Failed to find function for function call \n");
		throw gcException(ERR_IPC, gcString("Failed to find function [H:{0}, NP:{1}]!", fch->functionHash, fch->numP));
	}

	return it->second->call(&fch->data, fch->size, fch->numP);
}

void IPCClass::handleFunctionCall(const char* buff, uint32 size, bool async)
{
	uint32 nFunctionId = 0;
	uint32 nFunctionHash = 0;
	std::shared_ptr<IPCParameterI> ret;

	try
	{
		ret = std::shared_ptr<IPCParameterI>(doHandleFunctionCall(buff, size, nFunctionId, nFunctionHash));
	}
	catch (gcException &e)
	{
		ret = std::make_shared<PException>(e);
	}

	//dont worry about return for async calls.
	if (async || nFunctionHash == 0)
		return;

	if (!ret)
		ret = std::make_shared<PVoid>();

	uint32 dsize = ret->getSerializeSize();
	uint32 bsize = IPCFunctionCallSIZE + dsize + IPCParameterSIZE;

	gcBuff tempBuff(bsize);
	char* nbuff = tempBuff.c_ptr();

	memset(nbuff, 1, bsize);

	IPCFunctionCall *fchr = (IPCFunctionCall*)nbuff;
	fchr->id = nFunctionId;
	fchr->numP = 1;
	fchr->functionHash = nFunctionHash;
	fchr->size = dsize + IPCParameterSIZE;

	IPCParameter *p = (IPCParameter*)&fchr->data;

	p->size = dsize;
	p->type = ret->getType();
	ret->serialize(&p->data);

	sendMessage( MT_FUNCTIONRETURN, (const char*)fchr, sizeofStruct(fchr) );
}

void IPCClass::handleFunctionReturn(const char* buff, uint32 size)
{
	IPCFunctionCall *fch = (IPCFunctionCall*)buff;
	IPCLock* lock = findLock(fch->id);

	if (lock)
	{
		IPCParameter* par = (IPCParameter*)&fch->data;
		lock->result = getParameter(par->type, &par->data, par->size);
		lock->trigger();
	}
	else
	{
		Warning("Failed to process function return.\n");
	}
}

void IPCClass::handleEventTrigger(const char* buff, uint32 size)
{
	if (IPCEventTriggerSIZE > size)
	{
		Warning("Trigger size is too small \n");
		return;
	}

	IPCEventTrigger *fch = (IPCEventTrigger*)buff;

	std::map<uint32,IPCEventI*>::iterator it;
	it = m_mEvent.find(fch->eventHash);

	if (it != m_mEvent.end())
	{
		std::vector<IPCParameterI*> vParameters;
		deserializeList(vParameters, &fch->data, fch->size);

		it->second->trigger(vParameters);

		safe_delete(vParameters);
	}
	else
	{
		Warning("Failed to find event for Event Trigger \n");
	}
}


void IPCClass::sendMessage(uint8 type, const char* buff, uint32 size)
{
	//const char* buff, uint32 size, uint32 id, uint8 type
	m_pManager->sendMessage(buff, size, m_uiId, type);
}

void IPCClass::sendLoopbackMessage(uint8 type, const char* buff, uint32 size)
{
	//const char* buff, uint32 size, uint32 id, uint8 type
	m_pManager->sendLoopbackMessage(buff, size, m_uiId, type);
}

void IPCClass::destroy()
{
	if (!m_pManager->isDisconnected())
		sendMessage( MT_KILL, nullptr, 0 );

	m_pManager->destroyClass(this);
}

}
