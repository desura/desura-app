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
#include "UploadThread.h"

#include <algorithm>

#define MINCHUNKSIZE (500*1024)

namespace UserCore
{
namespace Thread
{


UploadThread::UploadThread(UploadThreadInfo* info) : MCFThread( "Upload Thread", info->itemId )
{
	m_pInfo = info;

	m_uiContinueCount = 0;
	m_uiChunkSize = 1024*1024; //chunksize;
	m_uiFileSize = 0;
	m_uiAmountRead =0;

	m_fLastAmmount = 0;
	m_bSetPauseStart = false;
	m_bCancel = false;
}

UploadThread::~UploadThread()
{
	stop();
}

void UploadThread::doRun()
{
	DesuraId id= getItemId();
	gcString type = id.getTypeString();


	uint32 sCode = 0;
	UTIL::FS::Path path = UTIL::FS::PathWithFile(m_pInfo->szFile);
	m_uiFileSize = UTIL::FS::getFileSize(path);

	if (m_uiFileSize == 0)
		throw gcException(ERR_ZEROFILE, gcString("File size is zero [{0}].", m_pInfo->szFile));

	UTIL::FS::FileHandle hFile(path, UTIL::FS::FILE_READ);

	if (m_pInfo->uiStart > 0)
	{
		hFile.seek(m_pInfo->uiStart);

		UserCore::Misc::UploadInfo ui;
		ui.percent = (uint8)(m_pInfo->uiStart*100/m_uiFileSize);
		onUploadProgressEvent(ui);

		ui = UserCore::Misc::UploadInfo();
		ui.milestone = true;
		onUploadProgressEvent(ui);
	}

	uint64 chunkCalc = m_uiFileSize / 100;
	//max chunck size is 40mg, min is 5mg

	uint32 minSize = 5*1024*1024;
	uint32 maxSize = 40*1024*1024;

	if (chunkCalc > UINT_MAX)
		m_uiChunkSize = maxSize;
	else
		m_uiChunkSize = Clamp((uint32)chunkCalc, minSize, maxSize);

	//if the file is 20% (or less) bigger than one chunk. Upload in one go
	if ( (uint64)((double)m_uiChunkSize*1.2) > m_uiFileSize)
		m_uiChunkSize = (uint32)m_uiFileSize;

	gcString url = getWebCore()->getUrl(WebCore::McfUpload);
	m_hHttpHandle->setUrl(url.c_str());

	UTIL::MISC::Buffer buffer(m_uiChunkSize);

	m_tStartTime = gcTime();

	while (sCode != 999)
	{
		if (m_bCancel)
			break;

		if (m_bSetPauseStart)
		{
			m_tPauseStartTime = gcTime();
			m_bSetPauseStart = false;
		}

		while (isPaused())
		{
			gcSleep(500);
		}

		uint32 chunkSize = m_uiChunkSize;
		uint64 fileLeft =  m_uiFileSize - (m_uiAmountRead + m_pInfo->uiStart);
		if ((uint64)chunkSize > fileLeft)
		{
			chunkSize = (uint32)(fileLeft);
		}

		if (sCode == 0)
		{
			hFile.read(buffer, chunkSize);
			sCode = 900;
		}


		m_hHttpHandle->cleanUp();
		m_hHttpHandle->clearCookies();
		getWebCore()->setWCCookies(m_hHttpHandle);

		m_hHttpHandle->addPostText("key", m_pInfo->szKey.c_str());
		m_hHttpHandle->addPostText("action", "uploadchunk");
		m_hHttpHandle->addPostText("siteareaid", id.getItem());
		m_hHttpHandle->addPostText("sitearea", type.c_str());
		m_hHttpHandle->addPostFileAsBuff("mcf", "upload.mcf", buffer, chunkSize);
		m_hHttpHandle->addPostText("uploadsize", chunkSize);
		m_hHttpHandle->getProgressEvent()  += delegate(this,  &UploadThread::onProgress);

		uint8 res = 0;

		//need to check here a second time incase we where paused or by fluke missed the check the first time as m_hHttpHandle->CleanUp() removes the abort flag.
		if (m_bCancel)
			break;

		try
		{
			res = m_hHttpHandle->postWeb();
		}
		catch (gcException &except)
		{
			if (m_uiContinueCount > 3)
			{
				throw;
			}
			else
			{
				m_uiContinueCount++;
				Warning(gcString("Failed to upload chunk: {0}. Retrying.\n", except));	
				continue;
			}
		}

		if (res == UWEB_USER_ABORT)
			continue;

		const char* error = m_hHttpHandle->getData();

		//Warning("UC: %s\n", error);

		XML::gcXMLDocument doc(const_cast<char*>(error), m_hHttpHandle->getDataSize());

		int status = -1;

		try
		{
			doc.ProcessStatus("itemupload", status);
		}
		catch (...)
		{
			if (status == (int)MCFUploadStatus::Finished)
			{
			}
			else if (status == (int)MCFUploadStatus::Failed || status == (int)MCFUploadStatus::ItemNotFound || m_uiContinueCount > 3)
			{
				throw;
			}
			else
			{
				m_uiContinueCount++;
				Warning(gcString("Upload xml error! Retrying. [{0}]\n", m_pInfo->szFile));
				continue;
			}
		}
		
		sCode = status;

		if (sCode == (int)MCFUploadStatus::Ok || sCode == (int)MCFUploadStatus::Finished)
		{
			m_uiAmountRead += chunkSize;
			m_uiContinueCount = 0;

			UserCore::Misc::UploadInfo ui;
			ui.milestone = true;
			onUploadProgressEvent(ui);
		}
	}

	hFile.close();

	if (sCode == 999)
	{
		onCompleteEvent(sCode);
		getUploadManager()->removeUpload(m_pInfo->szKey.c_str(), false);
	}
}


void UploadThread::onProgress(Prog_s& p)
{
	Prog_s* temp = &p;

	if (!temp)
		return;

	uint64 currProg = m_uiAmountRead + (uint64)temp->ulnow;

	UserCore::Misc::UploadInfo ui;
	ui.num = 0;
	ui.totalAmmount = m_uiFileSize;
	ui.doneAmmount = currProg + m_pInfo->uiStart;
	ui.percent = (uint8)((currProg+m_pInfo->uiStart)*100/m_uiFileSize);

	if (temp->abort)
	{
		ui.doneAmmount = m_uiAmountRead + m_pInfo->uiStart;
		ui.paused = true;
		onUploadProgressEvent(ui);
		return;
	}

	gcTime curTime;
	double rate = (temp->ulnow - m_fLastAmmount)/((double)(curTime - m_tLastTime).seconds());
	
	if (m_fLastAmmount < 1)
		rate = 0;

	m_fLastAmmount = temp->ulnow;
	m_tLastTime = curTime;

	auto total = curTime - m_tStartTime;
	total -= m_tTotPauseTime;

	double avgRate = (currProg) / ((double)total.seconds());
	double pred = (m_uiFileSize - currProg - m_pInfo->uiStart) / avgRate;

	auto predTime = gcDuration(std::chrono::seconds((long)pred));

	ui.min = (uint8)predTime.minutes();
	ui.hour = (uint8)predTime.hours();
	ui.rate = (uint32)(avgRate);

	onUploadProgressEvent(ui);
}

void UploadThread::onPause()
{
	m_bSetPauseStart = true;
	m_hHttpHandle->abortTransfer();

	onPauseEvent();
}

void UploadThread::onUnpause()
{
	m_tTotPauseTime += gcTime() - m_tPauseStartTime;
	onUnpauseEvent();
}

void UploadThread::onStop()
{
	m_bCancel = true;
	m_hHttpHandle->abortTransfer();
}

}
}
