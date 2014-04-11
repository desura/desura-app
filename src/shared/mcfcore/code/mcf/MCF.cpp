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
#include "MCF.h"
#include "mcfcore/DownloadProvider.h"

#include "BZip2.h"
#include <branding/mcfcore_version.h>
#include "mcfcore/ProgressInfo.h"

#include "XMLSaveAndCompress.h"
#include "thread/MCFServerCon.h"

using namespace MCFCore;

namespace 
{
	//! A struct that is used in the sorting of the mcf files
	//!
	struct file_sortkey
	{
		bool operator()(const std::shared_ptr<MCFCore::MCFFile>& lhs, const std::shared_ptr<MCFCore::MCFFile>& rhs)
		{
			return (lhs->getHash() < rhs->getHash());
		}
	};
}

typedef struct
{
	int hash;
	int id;
} find_s;


MCF::MCF()
	: m_sHeader(std::make_shared<MCFCore::MCFHeader>())
{
	setWorkerCount(0);
}

MCF::MCF(std::shared_ptr<MCFCore::Misc::DownloadProvidersI> pDownloadProviders)
	: m_sHeader(std::make_shared<MCFCore::MCFHeader>())
	, m_pDownloadProviders(pDownloadProviders)
{
	setWorkerCount(0);
}

MCF::~MCF()
{
	m_mThreadMutex.lock();
	safe_delete(m_pTHandle);
	m_mThreadMutex.unlock();
}

void MCF::disableCompression()
{
	gcAssert(m_sHeader);
	m_sHeader->addFlags(MCFCore::MCFHeaderI::FLAG_NOTCOMPRESSED);
}

bool MCF::isCompressed()
{
	gcAssert(m_sHeader);
	return !(m_sHeader->getFlags() & MCFCore::MCFHeaderI::FLAG_NOTCOMPRESSED);
}

void MCF::setFile(const char* file)
{
   m_szFile = file;
}

void MCF::setFile(const char* file, uint64 offset)
{
	m_szFile = file;
	m_uiFileOffset = offset;
}

const char* MCF::getFile()
{
	return m_szFile.c_str();
}

MCFCore::MCFHeaderI* MCF::getHeader()
{
	return m_sHeader.get();
}

void MCF::setHeader(MCFCore::MCFHeaderI* head)
{
	MCFCore::MCFHeader *temp = (MCFCore::MCFHeader*)head;

	if (!temp)
		return;

	m_sHeader = std::make_shared<MCFCore::MCFHeader>(head);
}

void MCF::setHeader(DesuraId id, MCFBranch branch, MCFBuild build)
{
	gcAssert(m_sHeader);
	m_sHeader->setBranch(branch);
	m_sHeader->setType(id.getType());
	m_sHeader->setBuild(build);
	m_sHeader->setId(id.getItem());
}


bool MCF::isPaused()
{
	return m_bPaused;
}

void MCF::pause()
{
	std::lock_guard<std::mutex> guard(m_mThreadMutex);

	if (!m_bPaused)
		return;

	gcTrace("");

	if (m_pTHandle)
		m_pTHandle->pause();

	m_bPaused = true;
}

void MCF::unpause()
{
	std::lock_guard<std::mutex> guard(m_mThreadMutex);

	if (m_bPaused)
		return;

	gcTrace("");

	if (m_pTHandle)
		m_pTHandle->unpause();

	m_bPaused = false;
}

void MCF::stop()
{
	std::lock_guard<std::mutex> guard(m_mThreadMutex);

	if (m_bStopped)
		return;

	gcTrace("");
	m_bStopped = true;

	if (m_pTHandle && !m_pTHandle->isStopped())
		m_pTHandle->stop();

	if (m_pMCFServerCon)
		m_pMCFServerCon->stop();
}

void MCF::setServerCon(MCFCore::Misc::MCFServerCon *pMCFServerCon)
{
	std::lock_guard<std::mutex> guard(m_mThreadMutex);

	gcAssert((pMCFServerCon && !m_pMCFServerCon) || (!pMCFServerCon && m_pMCFServerCon));
	m_pMCFServerCon = pMCFServerCon;
}

void MCF::setWorkerCount(uint16 count)
{
	if (count == 0)
		m_uiWCount = UTIL::MISC::getCoreCount();
	else
		m_uiWCount = count;
}

uint64 MCF::getDLSize()
{
	uint64 size = 0;
	for (size_t x=0; x<m_pFileList.size(); x++)
	{
		if (!m_pFileList[x]->isSaved())
			continue;

		size += m_pFileList[x]->getCurSize();

		if (m_pFileList[x]->hasDiff())
			size += m_pFileList[x]->getDiffSize();
	}
	return size;
}

uint64 MCF::getINSize()
{
	uint64 size = 0;
	for (size_t x=0; x<m_pFileList.size(); x++)
		size += m_pFileList[x]->getSize();

	return size;
}

uint64 MCF::getFileSize()
{
	uint64 size = 0;
	for (size_t x=0; x<m_pFileList.size(); x++)
	{
		if (!m_pFileList[x]->isSaved())
			continue;

		size += m_pFileList[x]->getSize();
	}
	return size;
}

void MCF::addFile(std::shared_ptr<MCFCore::MCFFile>&& file)
{
	m_pFileList.push_back(std::move(file));
}

MCFCore::MCFFileI* MCF::getMCFFile(uint32 index)
{
	if (index >= m_pFileList.size())
		return nullptr;

	return m_pFileList[index].get();
}

std::shared_ptr<MCFCore::MCFFile> MCF::getFile( uint32 index )
{
	if (index >= m_pFileList.size())
		return std::shared_ptr<MCFCore::MCFFile>();

	return m_pFileList[index];
}

void MCF::sortFileList()
{
	std::sort(m_pFileList.begin(), m_pFileList.end(), file_sortkey());
	m_iLastSorted = (uint32)m_pFileList.size();
}

void MCF::parseXml(char* buff, uint32 buffLen)
{
	if (m_bStopped)
		return;

	XML::gcXMLDocument doc(buff, buffLen);
	
	auto fNode = doc.GetRoot("files");

	if (!fNode.IsValid())
		throw gcException(ERR_XML_NOPRIMENODE);

	fNode.for_each_child("file", [this](const XML::gcXMLElement &xmlElement)
	{
		if (m_bStopped)
			return;

		try
		{
			auto temp = std::make_shared<MCFCore::MCFFile>();
			temp->loadXmlData(xmlElement);
			m_pFileList.push_back(temp);
		}
		catch (gcException &)
		{
		}
	});
}

void MCF::genXml(XMLSaveAndCompress *sac)
{
	sac->save("<?xml version=\"1.0\" encoding=\"UTF-8\"?>", 38);
	sac->save("<files>", 7);

	for (size_t x=0; x<m_pFileList.size(); x++)
	{
		sac->save("<file>", 6);
		m_pFileList[x]->genXml(sac);
		sac->save("</file>", 7);
	}

	sac->save("</files>", 8);
}

uint32 MCF::findFileIndexByHash(uint64 hash)
{
	auto it = std::find_if(begin(m_pFileList), end(m_pFileList), [hash](const std::shared_ptr<MCFCore::MCFFile>& file){
		return file->getHash() == hash;
	});

	if (it == end(m_pFileList))
		return UNKNOWN_ITEM;

	return std::distance(begin(m_pFileList), it);
}

void MCF::printAll()
{
	uint32 size = (uint32) m_pFileList.size();

	Debug("---------------------------------\n");

	for (uint32 x=0; x<size; x++)
		Debug(gcString("## {0,3}: {1} [{2}]\n", x,  m_pFileList[x]->getTimeStamp(), m_pFileList[x]->getName()));

	Debug("---------------------------------\n");

}

bool MCF::verifyMCF()
{
	gcTrace("");

	if (m_sHeader)
		m_sHeader->addFlags(MCFCore::MCFHeaderI::FLAG_NONVERIFYED);

	bool complete = true;

	UTIL::FS::FileHandle hFile;
	getReadHandle(hFile);

	size_t size = m_pFileList.size();
	for (size_t x=0; x<size; x++)
	{
		if (m_bStopped)
		{
			complete = false;
			break;
		}

		MCFCore::Misc::ProgressInfo temp;
		temp.percent = (uint8)(x*100/size);
		onProgressEvent(temp);

		if (!m_pFileList[x] || !m_pFileList[x]->isSaved())
			continue;

		try
		{
			m_pFileList[x]->verifyMcf(hFile, m_bStopped);
		}
		catch (gcException &)
		{
		}

		if (m_pFileList[x]->isComplete() == false)
			complete = false;
	}

	if (complete)
	{
		if (m_sHeader)
			m_sHeader->delFlags(MCFCore::MCFHeaderI::FLAG_NONVERIFYED);
	}

	return complete;
}



bool MCF::verifyInstall(const char* path, bool flagMissing, bool useDiffs)
{
	gcTrace("Path: {0}", path);

	if (!path)
		throw gcException(ERR_BADPATH);

	bool complete = true;

	size_t size = m_pFileList.size();
	for (size_t x=0; x<size; x++)
	{
		if (m_bStopped)
		{
			complete = false;
			break;
		}

		MCFCore::Misc::ProgressInfo temp;
		temp.percent = (uint8)(x*100/size);
		onProgressEvent(temp);

		if (!m_pFileList[x])
			continue;

		try
		{
			bool isComplete = m_pFileList[x]->isComplete();

			m_pFileList[x]->delFlag(MCFCore::MCFFileI::FLAG_COMPLETE);

			m_pFileList[x]->setDir(path);
			m_pFileList[x]->verifyFile(useDiffs);
			m_pFileList[x]->setDir(nullptr);

			if ( !m_pFileList[x]->isComplete() )
			{
				complete = false;

				if (flagMissing)
					m_pFileList[x]->addFlag(MCFCore::MCFFileI::FLAG_SAVE);
			}
			else
			{
				if (flagMissing)
					m_pFileList[x]->delFlag(MCFCore::MCFFileI::FLAG_SAVE);
			}

			m_pFileList[x]->delFlag(MCFCore::MCFFileI::FLAG_COMPLETE);

			if (isComplete)
				m_pFileList[x]->addFlag(MCFCore::MCFFileI::FLAG_COMPLETE);
		}
		catch (gcException &)
		{
			m_pFileList[x]->setDir(nullptr);
			throw;
		}
	}

	return complete;
}

//cant stop remove files :P
void MCF::removeFiles(const char* szPath, bool removeNonSave)
{
	gcTrace("Path: {0}", szPath);

	if (!szPath)
		throw gcException(ERR_BADPATH);

	size_t size = m_pFileList.size();

	for (size_t x=0; x<size; x++)
	{
		MCFCore::Misc::ProgressInfo temp;
		temp.percent = (uint8)(x*100/size);

		onProgressEvent(temp);

		if (!m_pFileList[x])
			continue;

		if (!removeNonSave && !m_pFileList[x]->isSaved())
			continue;

		m_pFileList[x]->setDir(szPath);
		m_pFileList[x]->delFile();
		m_pFileList[x]->setDir(nullptr);
	}

	UTIL::FS::Path path(szPath, "", false);
	UTIL::FS::delEmptyFolders(path);

	if (UTIL::FS::isFolderEmpty(path))
		UTIL::FS::delFolder(path);
}

bool MCF::isComplete()
{
	size_t size = m_pFileList.size();
	for (size_t x=0; x<size; x++)
	{
		if (!m_pFileList[x])
			continue;

		if (!(m_pFileList[x]->getFlags() & MCFCore::MCFFileI::FLAG_SAVE))
			return false;
	}

	return true;
}

bool MCF::isComplete(MCFI* exsitingFiles)
{
	MCF *temp = static_cast<MCF*>(exsitingFiles);

	std::vector<mcfDif_s> vSame;
	findChanges(temp, &vSame, nullptr, nullptr);

	bool res = true;

	size_t size = m_pFileList.size();
	for (size_t x=0; x<size; x++)
	{
		if (!m_pFileList[x])
			continue;

		bool saved = m_pFileList[x]->isSaved();
		bool inExistingList = false;

		for (size_t y=0; y<vSame.size(); y++)
		{
			if (vSame[y].thisMcf ==  x)
			{
				inExistingList = true;
				break;
			}
		}

		if (!saved && !inExistingList)
		{
			res = false;
			break;
		}
	}

	return res;
}

void MCF::removeIncompleteFiles()
{
	size_t size = m_pFileList.size();
	for (size_t x=0; x<size; x++)
	{
		if (!m_pFileList[x])
			continue;

		if (!(m_pFileList[x]->getFlags() & MCFCore::MCFFileI::FLAG_COMPLETE))
			m_pFileList[x]->delFlag(MCFCore::MCFFileI::FLAG_SAVE);
	}
}

void MCF::hashFiles()
{
	size_t size = m_pFileList.size();
	for (size_t x=0; x<size; x++)
	{
		if (!m_pFileList[x])
			continue;

		m_pFileList[x]->hashFile();
	}
}

void MCF::hashFiles(MCFI* inMcf)
{
	if (!inMcf)
		return;

	MCF* mcf = dynamic_cast<MCF*>(inMcf);

	if (!mcf)
		return;

	mcf->sortFileList();
	this->sortFileList();

	MCF *a = this;
	MCF *b = mcf;

	if (a->getFileCount() > b->getFileCount())
	{
		a = b;
		b = this;
	}

	for (size_t x=0; x<a->getFileCount(); x++ )
	{
		uint32 element = b->findFileIndexByHash( a->getFile(x)->getHash() );

		if (element == UNKNOWN_ITEM)
			continue;

		if (a == this)
			a->getFile(x)->hashFile();
		else
			b->getFile(element)->hashFile();
	}
}

bool MCF::crcCheck()
{
	bool res = true;

	UTIL::FS::FileHandle hFile;
	getReadHandle(hFile);

	size_t size = m_pFileList.size();
	for (size_t x=0; x<size; x++)
	{
		if (!m_pFileList[x])
			continue;

		if (m_pFileList[x]->getCRCCount() == 0)
		{
			printf("No crc's for file %s\n", m_pFileList[x]->getName());
			continue;
		}

		if (!m_pFileList[x]->crcCheck(hFile))
			res = false;
	}

	return res;
}

void MCF::makeCRC()
{
	printf("Making crc's\n");

	UTIL::FS::FileHandle hFile;
	getReadHandle(hFile);

	for (size_t x=0; x<m_pFileList.size(); x++)
	{
		if (!m_pFileList[x] || !m_pFileList[x]->isSaved())
			continue;

		m_pFileList[x]->generateCRC(hFile);
	}

	hFile.close();

	saveMCF_Header();
}




int32 MCF::verifyAll(const char* tempPath)
{
	gcTrace("Path: {0}", tempPath);

	if (!tempPath)
	{
		printf("Temp path is null.\n");
		return 3;
	}

	if (!m_sHeader->isValid())
	{
		printf("Mcf header is invalid.\n");
		return 1;
	}

	uint32 count = 0;
	for (size_t x=0; x<m_pFileList.size(); x++)
	{
		if (!m_pFileList[x]->isSaved())
			continue;

		count++;
	}

	if (count == 0)
	{
		printf("No files in the mcf are saved.\n");
		return 2;
	}

	UTIL::FS::FileHandle fh(getFile(), UTIL::FS::FILE_READ);

	uint32 badCount = 0;
	std::atomic<bool> placeholder;

	for (size_t x=0; x<m_pFileList.size(); x++)
	{
		if (!m_pFileList[x]->isSaved())
			continue;

		try
		{
			m_pFileList[x]->verifyMcf(fh, placeholder);
		}
		catch (gcException &e)
		{
			printf("%s", gcString("{0}: Failed to verify Mcf with file: {1}\n", m_pFileList[x]->getName(), e).c_str());
			badCount++;
			continue;
		}

		if (!m_pFileList[x]->isComplete())
		{
			printf("%s", gcString("{0}: Failed mcf verify\n", m_pFileList[x]->getName()).c_str());
			badCount++;
			continue;
		}

		gcString t("{0}{1}{2}", tempPath?tempPath:".", DIRS_STR, m_pFileList[x]->getFullPath());
		UTIL::FS::Path path(t, "", true);
		UTIL::FS::recMakeFolder(path);

		UTIL::MISC::Buffer buff(10*1024);

		try
		{
			UTIL::FS::FileHandle file(path, UTIL::FS::FILE_WRITE);
			fh.seek(m_pFileList[x]->getOffSet());
#ifdef WIN32
			size_t size = 0;
#endif
			if (m_pFileList[x]->isCompressed())
			{
				uint64 done = m_pFileList[x]->getCSize();
				UTIL::MISC::BZ2Worker bz(UTIL::MISC::BZ2_DECOMPRESS);

				fh.read(done, [&bz, &file](const unsigned char* buff, uint32 size) -> bool
				{
					UTIL::FS::FileHandle* pFile = &file;

					bz.write((const char*)buff, size, [pFile](const unsigned char* tbuff, uint32 tsize) -> bool
					{
						pFile->write((const char*)tbuff, tsize);
						return false;
					});

					return false;
				});

				bz.end([&file](const unsigned char* tbuff, uint32 tsize) -> bool
				{
					file.write((const char*)tbuff, tsize);
					return false;
				});
			}
			else
			{
				uint64 done = m_pFileList[x]->getSize();

				fh.read(done, [&file](const unsigned char* buff, uint32 size) -> bool
				{
					file.write((const char*)buff, size);
					return false;
				});
			}
		}
		catch (gcException &e)
		{
			printf("%s", gcString("{0}: Failed to save file: {1}\n", m_pFileList[x]->getName(), e).c_str());
			badCount++;

			UTIL::FS::delFile(path);
			continue;
		}

		m_pFileList[x]->setDir(tempPath);
		m_pFileList[x]->verifyFile();
		m_pFileList[x]->setDir("");

		if (!m_pFileList[x]->isComplete())
		{
			printf("%s", gcString("{0}: Failed file verify\n", m_pFileList[x]->getName()).c_str());
			badCount++;
		}

		UTIL::FS::delFile(path);
	}

	UTIL::FS::delEmptyFolders(tempPath);

	return (int32)badCount*-1;
}

bool MCF::verifyUnitTest(MCFI* mcf)
{
	MCF *temp = static_cast<MCF*>(mcf);

	gcAssert(temp);

	std::vector<mcfDif_s> vSame;
	std::vector<mcfDif_s> vDiff;
	std::vector<mcfDif_s> vDel;
	std::vector<mcfDif_s> vNew;

	findChanges( temp, &vSame, &vDiff, &vDel, &vNew);

	for (size_t x=0; x<vDiff.size(); x++)
		printf("\tDiff: %s [%s]\n", m_pFileList[vDiff[x].thisMcf]->getName(), m_pFileList[vDiff[x].thisMcf]->getPath());

	for (size_t x=0; x<vDel.size(); x++)
		printf("\tDel: %s [%s]\n", m_pFileList[vDel[x].thisMcf]->getName(), m_pFileList[vDel[x].thisMcf]->getPath());

	for (size_t x=0; x<vNew.size(); x++)
		printf("\tNew: %s [%s]\n", temp->getFile(vNew[x].otherMcf)->getName(), temp->getFile(vNew[x].otherMcf)->getPath());

	bool res = (vDiff.size() == 0) && (vDel.size() == 0) && (vNew.size() == 0);

	return res;

}

void MCF::removeNonSavedFiles()
{
	std::vector<size_t> delList;

	for (size_t x=0; x<m_pFileList.size(); x++)
	{
		if (m_pFileList[x]->isSaved())
			continue;

		delList.push_back(x);
	}


	std::vector<size_t>::reverse_iterator it=delList.rbegin();

	while (it != delList.rend())
	{
		m_pFileList.erase(m_pFileList.begin()+*it);
		++it;
	}
}

void MCF::getReadHandle(UTIL::FS::FileHandle& handle)
{
	handle.open(m_szFile.c_str(), UTIL::FS::FILE_READ, m_uiFileOffset);
}

void MCF::getWriteHandle(UTIL::FS::FileHandle& handle)
{
	gcAssert(m_uiFileOffset == 0);
	handle.open(m_szFile.c_str(), UTIL::FS::FILE_WRITE, m_uiFileOffset);
}

uint64 MCF::getFileOffset()
{
	return m_uiFileOffset;
}

void MCF::setDownloadProvider(std::shared_ptr<MCFCore::Misc::DownloadProvidersI> pDownloadProviders)
{
	m_pDownloadProviders = pDownloadProviders;

	auto header = getHeader();

	if (header && m_pDownloadProviders)
	{
		DesuraId id(header->getId(), header->getType());
		m_pDownloadProviders->setInfo(id, header->getBranch(), header->getBuild());
	}
}