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
#include "IPCPipeClient.h"

namespace IPC
{


PipeClient::PipeClient(const char* name, LoopbackProcessor* loopbackProcessor, uint32 managerId) 
	: PipeBase(name, gcString("{0}- IPC Client",name).c_str()), IPCManager(loopbackProcessor, managerId, name)
{
	m_pdSend.pendingConnection = false;
	m_pdRecv.pendingConnection = false;

	m_pdSend.sender = true;
	m_pdRecv.sender = false;

	m_bSetUped = false;

	m_hEventsArr[0] = INVALID_HANDLE_VALUE;
	m_hEventsArr[1] = INVALID_HANDLE_VALUE;
}

PipeClient::~PipeClient()
{
	Thread::BaseThread::stop();
	cleanUp();
}

void PipeClient::cleanUp()
{
	m_bSetUped = false;

	CancelIo(m_pdSend.hPipe);
	CancelIo(m_pdRecv.hPipe);

	CloseHandle(m_pdSend.hPipe);
	CloseHandle(m_pdRecv.hPipe);

	m_pdSend.hPipe = INVALID_HANDLE_VALUE;
	m_pdRecv.hPipe = INVALID_HANDLE_VALUE;

	m_pdSend.reset();
	m_pdRecv.reset();

	m_pdSend.pendingConnection = false;
	m_pdRecv.pendingConnection = false;

	CloseHandle(m_hEventsArr[0]);
	CloseHandle(m_hEventsArr[1]);

	m_hEventsArr[0] = INVALID_HANDLE_VALUE;
	m_hEventsArr[1] = INVALID_HANDLE_VALUE;
}


void PipeClient::setUpPipes()
{
	gcTrace("Send: {0}, Recv: {1}", m_szSendName, m_szRecvName);

	if (m_bSetUped)
		return;

	m_hEventsArr[0] = CreateEvent(nullptr, TRUE, TRUE, nullptr);
	m_hEventsArr[1] = CreateEvent(nullptr, TRUE, TRUE, nullptr);

	if (m_hEventsArr[0] == nullptr || m_hEventsArr[1] == nullptr) 
		throw gcException(ERR_IPC, gcString("CreateEvent failed with {0}.\n", GetLastError()));

	setSendEvent(m_hEventsArr[0]);
	m_pdSend.oOverlap.hEvent = m_hEventsArr[0];
	m_pdRecv.oOverlap.hEvent = m_hEventsArr[1];

	// All PipeClient instances are busy, so wait for 30 seconds. 
	BOOL sRes = WaitNamedPipe(m_szSendName.c_str(), 30000);

	if (sRes == 0)
		throw gcException(ERR_PIPE, GetLastError(), "Failed to create PipeClient (Send PipeClient is busy).");

	BOOL rRes = WaitNamedPipe(m_szRecvName.c_str(), 30000);

	if (rRes == 0)
		throw gcException(ERR_PIPE, GetLastError(), "Failed to create PipeClient (Receive PipeClient is busy).");
		

	//as this is the client connect to opposite server pipes
	m_pdSend.hPipe = CreateFile(m_szRecvName.c_str(), GENERIC_READ|GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
	DWORD sErr = GetLastError();

	m_pdRecv.hPipe = CreateFile(m_szSendName.c_str(), GENERIC_READ|GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
	DWORD rErr = GetLastError();

	if (m_pdSend.hPipe == INVALID_HANDLE_VALUE || m_pdRecv.hPipe == INVALID_HANDLE_VALUE)
	{
		if (m_pdSend.hPipe != INVALID_HANDLE_VALUE)
			CloseHandle(m_pdSend.hPipe);

		if (m_pdRecv.hPipe != INVALID_HANDLE_VALUE)
			CloseHandle(m_pdRecv.hPipe);

		m_pdSend.hPipe = INVALID_HANDLE_VALUE;
		m_pdRecv.hPipe = INVALID_HANDLE_VALUE;
		throw gcException(ERR_PIPE, GetLastError(), gcString("Failed to connect to pipe (S:{0} R:{1}).", sErr, rErr));
	}

	DWORD dwMode = PIPE_MODE; //PipeClient_READMODE_MESSAGE; 
	uint32 res1 = SetNamedPipeHandleState(m_pdSend.hPipe, &dwMode, nullptr, nullptr);
	uint32 res2 = SetNamedPipeHandleState(m_pdRecv.hPipe, &dwMode, nullptr, nullptr);

	if (!(res1 == 0) || !(res2 == 0))
		throw gcException(ERR_PIPE, "Failed to set PipeClient state");

	m_bSetUped = true;
}

PipeData* PipeClient::getData(uint32 index)
{
	if (index == 0)
		return &m_pdSend;

	return &m_pdRecv;
}

IPCManager* PipeClient::getManager(uint32 index)
{
	return this;
}

void PipeClient::disconnectAndReconnect(uint32 i) 
{ 
	gcTrace("Id: {0}", i);

	informClassesOfDisconnect();

	cleanUp();
	disconnect();
} 

}
