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
#include "BaseMCFThread.h"

#include "mcf/MCF.h"
#include "mcf/MCFFile.h"

namespace MCFCore
{
namespace Thread
{

BaseMCFThread::BaseMCFThread(uint16 num, MCFCore::MCF* caller, const char* name)
	: BaseThread( name )
	, m_rvFileList(caller->getFileList())
{
	gcAssert(caller);

	m_szFile = caller->getFile();
	m_bCompress = caller->isCompressed();
	m_pHeader = caller->getHeader();
	m_uiFileOffset = caller->getFileOffset();

	m_pUPThread = new MCFCore::Thread::UpdateProgThread(num, 0);
	m_pUPThread->onProgUpdateEvent += delegate(&onProgressEvent);

	m_uiNumber = num;
}

BaseMCFThread::~BaseMCFThread()
{
	safe_delete(m_pUPThread);
}

void BaseMCFThread::onPause()
{
	if (m_pUPThread)
		m_pUPThread->pause();
}

void BaseMCFThread::onUnpause()
{
	if (m_pUPThread)
		m_pUPThread->unpause();
}

void BaseMCFThread::onStop()
{
	if (m_pUPThread)
		m_pUPThread->stop();
}

}}
