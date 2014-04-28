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
#include "IPCManager.h"
#include "IPCClass.h"
#include "IPCParameter.h"
#include "IPCPipeBase.h"

namespace IPC
{


std::map<uint32, newClassFunc> *g_pmIPCClassList = nullptr;

class AutoCleanUp
{
public:
	~AutoCleanUp()
	{
		safe_delete(g_pmIPCClassList);
	}
};

AutoCleanUp aca;

void RegIPCClass(uint32 id, newClassFunc funct)
{
	if (!g_pmIPCClassList)
		g_pmIPCClassList = new std::map<uint32, newClassFunc>();

	g_pmIPCClassList->insert(  std::pair<uint32, newClassFunc>(id, funct) );
}

//! Internal class used to store pipe messages in the manager
//!
class PipeMessage
{
public:
	PipeMessage(uint32 size)
	{
		m_uiSize = size;
		m_szBuffer = new char[size];
	}

	PipeMessage(const char* message, uint32 size)
	{
		m_uiSize = size;
		m_szBuffer = new char[size];
		memcpy(m_szBuffer, message, size);
	}

	PipeMessage(PipeMessage& msg)
	{
		m_uiSize = msg.getSize();
		m_szBuffer = new char[m_uiSize];
		memcpy(m_szBuffer, msg.getBuffer(), m_uiSize);
	}

	~PipeMessage()
	{
		safe_delete(m_szBuffer);
	}

	const char* getBuffer()
	{
		return m_szBuffer;
	}

	uint32 getSize()
	{
		return m_uiSize;
	}

private:
	char* m_szBuffer;
	uint32 m_uiSize;
};


class ProcessThread : public Thread::BaseThread
{
public:
	ProcessThread(const char* threadName) : Thread::BaseThread(gcString("{0}: Process", threadName).c_str())
	{
	}

	~ProcessThread()
	{
		stop();
		safe_delete(m_vItemList);
	}

	void newMessage(WeakPtr<IPCClass> ipcClass, uint32 type, const char* buffer, uint32 size)
	{
		ProcessData *e = new ProcessData(ipcClass, type, buffer, size);

		m_mVectorMutex.lock();
		m_vItemList.push_back(e);
		m_mVectorMutex.unlock();

		m_WaitCond.notify();
	}

	void purgeEvents(IPCClass* c)
	{
		m_mVectorMutex.lock();
		
		std::vector<uint32> vDelList;

		for (size_t x=0; x<m_vItemList.size(); x++)
		{
			if (!m_vItemList[x]->pClass.expired())
			{	
				SmartPtr<IPCClass> classObj = m_vItemList[x]->pClass.lock();
				
				if (classObj->getId() == c->getId())
					vDelList.push_back(x);
			}
		}

		for (size_t x=vDelList.size(); x>0; x--)
		{
			SmartPtr<ProcessData> e(m_vItemList[x-1]);

			if (e.get() && !e->pClass.expired())
			{
				SmartPtr<IPCClass> classObj = e->pClass.lock();
				classObj->messageRecived(e->type, e->buff, e->buffsize);
			}

			m_vItemList.erase(m_vItemList.begin()+x-1);
		}

		m_mVectorMutex.unlock();
	}

protected:
	class ProcessData
	{
	public:
		ProcessData(WeakPtr<IPCClass> c, uint32 t, const char* b, uint32 s)
		{
			pClass = c;
			buffsize = s;
			buff = new char[s];
			memcpy(buff, b, s);
			type = t;
		}

		~ProcessData()
		{
			safe_delete(buff);
		}

		WeakPtr<IPCClass> pClass;
		char* buff;
		uint32 buffsize;
		uint32 type;
	};

	void run()
	{
		while (!isStopped())
		{
			SmartPtr<ProcessData> e;

			m_mVectorMutex.lock();

				if (m_vItemList.size() > 0)
				{
					e = SmartPtr<ProcessData>(m_vItemList[0]);
					m_vItemList.erase(m_vItemList.begin());
				}

			m_mVectorMutex.unlock();

			if (e.get() && !e->pClass.expired())
			{
				SmartPtr<IPCClass> classObj = e->pClass.lock();
				classObj->messageRecived(e->type, e->buff, e->buffsize);
			}
			else
			{
				m_WaitCond.wait();
			}
		}
	}

	void onStop()
	{
		m_WaitCond.notify();
	}

	std::mutex m_mVectorMutex;
	Thread::WaitCondition m_WaitCond;
	std::vector<ProcessData*> m_vItemList;
};






IPCManager::IPCManager(LoopbackProcessor* loopbackProcessor, uint32 managerId, const char* threadName, bool isServer)
{
	m_bServer = isServer;

	if (isServer)
		m_mClassId = 1;
	else
		m_mClassId = -1;

#ifdef WIN32
	m_hEvent = INVALID_HANDLE_VALUE;
#else
	m_hEvent = nullptr;
#endif

	m_bDisconnected = false;

	m_pEventThread = nullptr;
	m_pCallThread = nullptr;

	m_uiManagerId = managerId;
	m_pLoopbackProcessor = loopbackProcessor;

	m_szThreadName = threadName;

	if (isServer)
		m_szThreadName += ":Server ";
	else
		m_szThreadName += ":Client ";
	
#ifdef WIN32
	m_hEvent = INVALID_HANDLE_VALUE;
#else
	m_hEvent = nullptr;
#endif
}

IPCManager::~IPCManager()
{
	safe_delete(m_pEventThread);
	safe_delete(m_pCallThread);
	m_vClassList.clear();
}

WeakPtr<IPCClass> IPCManager::createClass(const char* name)
{
	if (!g_pmIPCClassList)
		throw gcException(ERR_IPC, "Class list is nullptr");

	uint32 hash = UTIL::MISC::RSHash_CSTR(name);
	std::map<uint32, newClassFunc>::iterator it = g_pmIPCClassList->find(hash);

	if (it == g_pmIPCClassList->end())
		throw gcException(ERR_IPC, "Cant find class to create");

	uint32 id = 0;
	SmartPtr<IPCClass> ipcc;
	{
		std::lock_guard<std::mutex> al(m_ClassMutex);

		id = m_mClassId;

		if (m_bServer)
			m_mClassId++;
		else
			m_mClassId--;

		ipcc = SmartPtr<IPCClass>(it->second(this, id, m_uiItemId));
	}

	IPCScopedLock<IPCManager> lock(this, newLock());

	IPCCreateClass cc;
	cc.hash = hash;
	cc.id = id;
	cc.lock = lock->id;
	sendMessage( (const char*)&cc, sizeofStruct(&cc), 0, MT_CREATECLASS);

	lock->timedWait();

	SmartPtr<IPCParameterI> ret(lock->result);

	if (ret.get() == nullptr)
	{
		throw gcException(ERR_IPC, "Return is nullptr");
	}
	else if (ret->getType() == PException::getTypeS())
	{
		throw gcException((gcException*)getParameterValue<gcException*>(ret.get()));	
	}
	else if (!ret->getValue()) //should be bool
	{
		throw gcException(ERR_IPC, "Return value is false");
	}


	std::lock_guard<std::mutex> al(m_ClassMutex);
	m_vClassList.push_back(ipcc);

	return ipcc;
}

IPCParameterI* IPCManager::createClass(uint32 hash, uint32 id)
{
	if (!g_pmIPCClassList)
	{
		gcException e(ERR_IPC, "Cannot find class to create");
		return new PException(e);
	}

	std::map<uint32, newClassFunc>::iterator it = g_pmIPCClassList->find(hash);

	if (it == g_pmIPCClassList->end())
	{
		gcException e(ERR_IPC, "Cannot find class to create");
		return new PException(e);
	}

	std::lock_guard<std::mutex> al(m_ClassMutex);
	m_vClassList.push_back(SmartPtr<IPCClass>(it->second(this, id, m_uiItemId)));

	return new PBool((void*)true);
}

void IPCManager::destroyClass(IPCClass* obj)
{
	std::lock_guard<std::mutex> al(m_ClassMutex);

	for (size_t x=0; x<m_vClassList.size(); x++)
	{
		if (m_vClassList[x]->getId() != obj->getId())
			continue;

		m_vClassList.erase(m_vClassList.begin()+x);
		return;
	}
}

void IPCManager::destroyClass(uint32 id)
{
	SmartPtr<IPCClass> obj;

	{
		std::lock_guard<std::mutex> al(m_ClassMutex);

		for (size_t x=0; x<m_vClassList.size(); x++)
		{
			if (m_vClassList[x]->getId() == id)
			{
				obj = m_vClassList[x];
				break;
			}
		}
	}

	destroyClass(obj.get());
}


void IPCManager::processInternalMessage(uint8 type, const char* buff, uint32 size)
{
	if (type == MT_STOP)
	{
		//disconnect();
	}
	else if (type == MT_CREATECLASS)
	{
		IPCCreateClass *cc = (IPCCreateClass*)buff;

		auto res = std::unique_ptr<IPCParameterI>(createClass(cc->hash, cc->id));
		uint32 bsize = res->getSerializeSize();

		gcBuff tempBuff(bsize + IPCCreateClassRetSIZE + IPCParameterSIZE);
		char* nbuff = tempBuff.c_ptr();

		IPCCreateClassRet *cr = (IPCCreateClassRet*)nbuff;
		cr->id = cc->id;
		cr->size = bsize + IPCParameterSIZE;
		cr->lock = cc->lock;

		IPCParameter* pc = (IPCParameter*)&cr->data;
		pc->size = bsize;
		pc->type = res->getType();

		res->serialize(&pc->data);

		sendMessage( (const char*)cr, sizeofStruct(cr), 0, MT_CREATECLASSRETURN);
	}
	else if (type == MT_CREATECLASSRETURN)
	{
		IPCCreateClassRet *cr = (IPCCreateClassRet*)buff;
		IPCLock* lock = findLock(cr->lock);

		if (lock)
		{
			IPCParameter* par = (IPCParameter*)&cr->data;
			lock->result = getParameter(par->type, &par->data, par->size);
			lock->trigger();
		}
		else
		{
			Warning("Failed to process create class return.\n");
		}
	}
}



void IPCManager::stop()
{
	gcTrace("");

	if (!m_bDisconnected)
		sendMessage(nullptr, 0, 0, MT_STOP);
}


void IPCManager::joinPartMessages(std::vector<PipeMessage*> &vMessages)
{
	std::vector<IPCMessage*> vIPCMessages;
	uint32 totSize = IPCMessageSIZE;

	for (auto p : vMessages)
	{
		IPCMessage *pIPCM = (IPCMessage*)p->getBuffer();
		vIPCMessages.push_back(pIPCM);
		totSize += pIPCM->size;
	}
		
	std::sort(begin(vIPCMessages), end(vIPCMessages), [](IPCMessage* a, IPCMessage* b){
		return a->part < b->part;
	});

	gcBuff buff(totSize);

	IPCMessage *msg = (IPCMessage*)buff.c_ptr();

	msg->id = vIPCMessages[0]->id;
	msg->type = vIPCMessages[0]->type;
	msg->tsize = totSize;
	msg->size = totSize - IPCMessageSIZE;
	msg->part = 0;
	msg->totparts = 1;
	msg->serial = vIPCMessages[0]->serial;

	auto temp = &msg->data;

	for (auto p : vIPCMessages)
	{
		memcpy(temp, &p->data, p->size);
		temp += p->size;
	}

	recvMessage(msg);
	safe_delete(vMessages);
}

void IPCManager::recvMessage(IPCMessage* msg)
{
	if (!msg)
		return;

	if (msg->totparts != 1)
	{
		std::lock_guard<std::mutex> guard(m_PartLock);
		m_mOutstandingPartMessages[msg->serial].push_back(new PipeMessage((const char*)msg, msg->tsize));

		if (m_mOutstandingPartMessages[msg->serial].size() == msg->totparts)
		{
			joinPartMessages(m_mOutstandingPartMessages[msg->serial]);
			m_mOutstandingPartMessages.erase(m_mOutstandingPartMessages.find(msg->serial));
		}

		return;
	}

	if (msg->id == 0)
	{
		processInternalMessage(msg->type, &msg->data, msg->size);
	}
	else
	{
		if (msg->type == MT_KILL)
		{
			destroyClass(msg->id);
		}
		else if (msg->type == MT_FUNCTIONRETURN)
		{
			std::lock_guard<std::mutex> al(m_ClassMutex);

			for (size_t x=0; x<m_vClassList.size(); x++)
			{
				if (m_vClassList[x]->getId() == msg->id)
				{
					m_vClassList[x]->messageRecived(msg->type, &msg->data, msg->size);
					break;
				}
			}
		}
		else
		{
			// as event callbacks often block we need to offload them to another thread
			std::lock_guard<std::mutex> al(m_ClassMutex);
			SmartPtr<IPCClass> outClass;

			for (size_t x=0; x<m_vClassList.size(); x++)
			{
				if (m_vClassList[x]->getId() == msg->id)
				{
					outClass = m_vClassList[x];
					break;
				}
			}

			if (!outClass.get())
				return;

			if (msg->type == MT_EVENTTRIGGER)
			{
				if (!m_pEventThread)
				{
					m_pEventThread = new ProcessThread(m_szThreadName.c_str());
					m_pEventThread->start();
				}

				m_pEventThread->newMessage(outClass, msg->type, &msg->data, msg->size);
			}
			else
			{
				if (!m_pCallThread)
				{
					m_pCallThread = new ProcessThread(m_szThreadName.c_str());
					m_pCallThread->start();
				}

				m_pCallThread->newMessage(outClass, msg->type, &msg->data, msg->size);
			}
		}
	}
}


class PendingPartRecvMessage
{
public:
	PendingPartRecvMessage(IPCMessage* pMsg, uint32 size)
		: left(pMsg->tsize - size)
		, buff(pMsg->tsize)
	{
		memcpy(buff.c_ptr(), (const char*)pMsg, size);
	}

	void append(const char* szBuff, uint32 nSize)
	{
		gcAssert(nSize <= left);
		memcpy(buff.c_ptr() + (buff.size() - left), (const char*)szBuff, nSize);
		left -= nSize;
	}

	operator IPCMessage* ()
	{
		gcAssert(left == 0);
		return (IPCMessage*)buff.c_ptr();
	}

	uint32 getLeft()
	{
		return left;
	}

private:
	uint32 left;
	gcBuff buff;
};

void IPCManager::recvMessage(const char* buff, uint32 size)
{
	uint32 processed = 0;

	while (processed < size)
	{
		uint32 left = size-processed;

		if (left < 4)
		{
			Warning("Failed to read 4 bytes from pipe.\n");
			return;
		}

		if (m_pPendingMessage)
		{
			if (m_pPendingMessage->getLeft() < left)
			{
				auto t = m_pPendingMessage->getLeft();
				m_pPendingMessage->append(buff + processed, m_pPendingMessage->getLeft());

				recvMessage(*m_pPendingMessage);
				safe_delete(m_pPendingMessage);

				processed += t;
			}
			else
			{
				m_pPendingMessage->append(buff + processed, left);
				processed += left;
			}
		}
		else
		{
			IPCMessage* pMsg = (IPCMessage*)(buff + processed);

			if (pMsg->tsize > left)
			{
				m_pPendingMessage = std::make_shared<PendingPartRecvMessage>(pMsg, left);
				processed -= left;
			}
			else
			{
				recvMessage(pMsg);
				processed += pMsg->tsize;
			}
		}
	}
}

void IPCManager::sendMessage(const char* buff, uint32 size, uint32 id, uint8 type)
{
	if (m_bDisconnected)
		throw gcException(ERR_IPC, "Pipe is disconnected!");

	uint64 serial = ++m_nMsgSerial;

	auto createMessage = [id, type, serial](const char* buff, uint32 size, uint8 part, uint8 totparts)
	{
		auto pm = new PipeMessage(IPCMessageSIZE + size);
		IPCMessage *msg = (IPCMessage*)pm->getBuffer();

		msg->id = id;
		msg->type = type;
		msg->tsize = IPCMessageSIZE + size;
		msg->size = size;
		msg->part = part;
		msg->totparts = totparts;
		msg->serial = serial;

		memcpy(&msg->data, buff, size);
		return pm;
	};

	std::vector<std::pair<uint32, const char*>> vMsgParts;

	auto tbuff = buff;

	do
	{
		auto s = size;

		if (s > (BUFSIZE - IPCMessageSIZE))
			s = BUFSIZE - IPCMessageSIZE;

		vMsgParts.push_back(std::make_pair(s, tbuff));

		size -= s;
		tbuff += s;
	} 
	while (size > 0);

	int x = 0;

	for (auto p : vMsgParts)
	{
		auto pm = createMessage(p.second, p.first, x, vMsgParts.size());
		++x;

		std::lock_guard<std::mutex> guard(m_mVectorMutex);
		m_vPipeMsgs.push_back(pm);	
	}


#ifdef WIN32
	if (m_hEvent != INVALID_HANDLE_VALUE)
		SetEvent(m_hEvent);
#else
	if (m_hEvent)
		m_hEvent->notify();
#endif
}

void IPCManager::sendLoopbackMessage(const char* buff, uint32 size, uint32 id, uint8 type)
{
	if (m_bDisconnected)
		throw gcException(ERR_IPC, "Pipe is disconnected!");

	uint32 tsize = size + IPCMessageSIZE;
	char* buffer = new char[tsize];

	IPCMessage *msg = (IPCMessage*)buffer;

	msg->id = id;
	msg->type = type;
	msg->tsize = tsize;
	msg->size = size;
	msg->part = 0;
	msg->totparts = 1;

	memcpy(&msg->data, buff, size);

	m_pLoopbackProcessor->loopbackMessage(buffer, tsize, m_uiManagerId);

	safe_delete(buffer);

#ifdef WIN32
	if (m_hEvent != INVALID_HANDLE_VALUE)
		SetEvent(m_hEvent);
#else
	if (m_hEvent)
		m_hEvent->notify();
#endif
}

bool IPCManager::getMessageToSend(char* buffer, uint32 buffSize, uint32& msgSize)
{
	PipeMessage *msg = nullptr;

	m_mVectorMutex.lock();
	if (m_vPipeMsgs.size() > 0)
	{
		msg = m_vPipeMsgs.front();
		m_vPipeMsgs.erase(m_vPipeMsgs.begin());
	}
	m_mVectorMutex.unlock();

	if (!msg)
		return false;

	msgSize = msg->getSize();

	gcAssert(msgSize <= buffSize);
	uint32 cpysize = (msgSize > buffSize)?buffSize:msgSize;
	msgSize = cpysize;

	memcpy(buffer, msg->getBuffer(), cpysize);
	safe_delete(msg);

	return true;
}

void IPCManager::disconnect(bool triggerEvent)
{
	gcTrace("");

	m_bDisconnected = true;

	if (m_pEventThread)
		m_pEventThread->stop();

	if (m_pCallThread)
		m_pCallThread->stop();

	if (triggerEvent)
		onDisconnectEvent();
}

void IPCManager::restart()
{
	gcTrace("");

	m_vClassList.clear();
	safe_delete(m_pEventThread);
	safe_delete(m_pCallThread);
	m_bDisconnected = false;
}

DesuraId IPCManager::setItemHashId(const char* itemHashId)
{
	PipeItemAuth pia;
	pia.hash = gcString(itemHashId);
	pia.internId = 0;
	
	onNeedAuthEvent(pia);
	m_uiItemId = pia.internId;

	for (size_t x=0; x<m_vClassList.size(); x++)
		m_vClassList[x]->setItemId(m_uiItemId);

	return pia.internId;
}


void IPCManager::informClassesOfDisconnect()
{
	std::lock_guard<std::mutex> al(m_ClassMutex);

	gcException e(ERR_PIPE, "Pipe Disconnected. IPC Failed.");

	for (size_t x=0; x<m_vClassList.size(); x++)
		m_vClassList[x]->cancelLocks(e);
}


#ifdef NIX
uint32 IPCManager::getNumSendEvents()
{
	uint32 ret = 0;
	m_mVectorMutex.lock();
	ret = m_vPipeMsgs.size();
	m_mVectorMutex.unlock();

	return ret;
}

#endif

extern std::map<uint32, newFunc> *g_vParameterType;
}



void cleanUpIPC()
{
	safe_delete(IPC::g_pmIPCClassList);
	safe_delete(IPC::g_vParameterType);
}

