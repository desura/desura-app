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
#include "MCFServerCon.h"
#include "MCFDPReporter.h"

#include <array>

#define READ_SIZE (1024*1024)

using namespace MCFCore::Misc;


MCFServerCon::MCFServerCon()
{
	m_FtpHandle->getProgressEvent() += delegate(this, &MCFServerCon::onProgress);
	m_FtpHandle->getWriteEvent() += delegate(this, &MCFServerCon::onWrite);
	m_FtpHandle->dontThrowOnPartFile();
}

MCFServerCon::~MCFServerCon()
{
	stop();
	disconnect();

	if (m_uiDPRId != UINT_MAX)
		GetDPReporter()->delProvider(m_uiDPRId);
}

void MCFServerCon::onProgress(Prog_s &prog)
{
	if (!m_bConnected)
		prog.abort = true;
}

void MCFServerCon::onWrite(WriteMem_s &mem)
{
	if (!m_bConnected)
		mem.stop = true;

	if (m_pOutBuffer)
	{
		uint32 ammount = mem.size;
		onProgressEvent(ammount);

		if (m_uiDPRId != UINT_MAX)
			GetDPReporter()->reportProgress(m_uiDPRId, ammount);

		mem.wrote = mem.size;
		mem.handled = true;

		bool res = m_pOutBuffer->writeData((char*)mem.data, mem.size);

		if (!res)
			mem.stop = true;
	}
}

void MCFServerCon::connect(const MCFCore::Misc::DownloadProvider &provider, const GetFile_s& fileAuth)
{
	if (m_bConnected)
		return;

	gcString u(provider.getUrl());
	m_bHttpDownload = u.find("http://") == 0;

	gcTrace("Url: {0}", u);

	if (!m_bHttpDownload)
	{
		if (u.find("mcf://") == 0)
			u.replace(0, 3, "ftp");

		size_t pos = u.find(":62001");

		if (pos != std::string::npos)
			u.replace(pos, 6, ":62003");

		u += "/mcf";
	}

	m_FtpHandle->setUrl(u.c_str());

	if (provider.getType() != DownloadProviderType::Cdn)
		m_FtpHandle->setUserPass(fileAuth.authkey.data(), fileAuth.authhash.data());

	m_bConnected = true;
}

void MCFServerCon::disconnect()
{
	if (!m_bConnected)
		return;

	gcTrace("");

	m_bConnected = false;
}

void MCFServerCon::downloadRange(uint64 offset, uint32 size, OutBufferI* buff)
{
	m_pOutBuffer = buff;
	m_uiDone=0;

	try
	{
		doDownloadRange(offset, size);
	}
	catch (gcException &e)
	{
		m_pOutBuffer = nullptr;

		if (e.getSecErrId() == 18) //CURLE_PARTIAL_FILE
			return;
			
		disconnect();
		throw;
	}

	m_pOutBuffer = nullptr;
}

void MCFServerCon::doDownloadRange(uint64 offset, uint32 size)
{
	if (!m_bConnected)
		throw gcException(ERR_SOCKET, "Socket not connected");

	m_FtpHandle->cleanUp(false);

	uint8 res;

	if (m_bHttpDownload)
	{
		gcString user(m_FtpHandle->getUserName());

		//CDN Links will have no user info, use http 1.1 headers for download range
		if (user.empty())
		{
			m_FtpHandle->setDownloadRange(offset, size);
		}
		else
		{
			gcString o("{0}", offset);

			m_FtpHandle->addPostText("authid", user.c_str());
			m_FtpHandle->addPostText("authkey", m_FtpHandle->getPassword());
			m_FtpHandle->addPostText("offset", o.c_str());
			m_FtpHandle->addPostText("length", size);
		}

		res = m_FtpHandle->postWeb();
	}
	else
	{
		m_FtpHandle->setDownloadRange(offset, size);
		res = m_FtpHandle->getFtp();
	}

	if (res == UWEB_USER_ABORT)
		throw gcException(ERR_MCFSERVER, ERR_USERCANCELED, "Client canceled data write");
}

void MCFServerCon::setDPInformation(const char* name)
{
	if (m_uiDPRId != UINT_MAX)
		GetDPReporter()->delProvider(m_uiDPRId);

	m_uiDPRId = -1;
	m_uiDPRId = GetDPReporter()->newProvider(name);
}

void MCFServerCon::onPause()
{
	m_FtpHandle->abortTransfer();

	if (m_uiDPRId != UINT_MAX)
		GetDPReporter()->delProvider(m_uiDPRId);

	m_uiDPRId = -1;	
}

void MCFServerCon::stop()
{
	m_FtpHandle->abortTransfer();
}
