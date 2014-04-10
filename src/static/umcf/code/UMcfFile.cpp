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


#pragma warning( disable : 4627 )

#include "Common.h"
#include "umcf/UMcfFile.h"
#include <iostream>
#include <fstream>

#include "XMLMacros.h"
#include "UMcfFile_utils.h"

#ifdef NIX
#include <errno.h>
#include <limits.h>
#endif


UMcfFile::UMcfFile()
{
	m_uiFlags = 0;
	m_uiHash = 0;
	m_ullSize = 0;
	m_ullCSize = 0;
	m_ullTimeStamp = 0;
}

UMcfFile::~UMcfFile()
{
}

const wchar_t* UMcfFile::getName()
{
	return m_szName.c_str();
}

const wchar_t* UMcfFile::getPath()
{
	return m_szPath.c_str();
}

const char* UMcfFile::getCsum()
{
	return m_szCsum.c_str();
}

const char* UMcfFile::getCCsum()
{
	return m_szCCsum.c_str();
}

void UMcfFile::setName(const wchar_t* var)
{
	if (!var)
		m_szName = L"";
	else 
		m_szName = var;
}

void UMcfFile::setPath(const wchar_t* var)
{
	if (!var)
		m_szPath = L"";
	else 
		m_szPath = var;
}

void UMcfFile::setCsum(const char* var)
{
	if (!var)
		m_szCsum = "";
	else 
		m_szCsum = var;
}

void UMcfFile::setCCsum(const char* var)
{
	if (!var)
		m_szCCsum = "";
	else 
		m_szCCsum = var;
}

uint8 UMcfFile::loadXmlData(const XML::gcXMLElement &xmlElement)
{
	std::string name;
	std::string path;

	xmlElement.GetChild("name", name);
	xmlElement.GetChild("path", path);
	xmlElement.GetChild("nom_csum", m_szCsum);
	xmlElement.GetChild("com_csum", m_szCCsum);

	m_szName = name;
	m_szPath = path;

#ifdef NIX
	std::replace(m_szPath.begin(), m_szPath.end(), '\\', '/');
#endif

	xmlElement.GetChild("size", m_ullSize);
	xmlElement.GetChild("csize", m_ullCSize);
	xmlElement.GetChild("flags", m_uiFlags);

	xmlElement.GetChild("offset", m_ullOffset);
	xmlElement.GetChild("tstamp", m_ullTimeStamp);

	return MCFF_OK;
}

void UMcfFile::genXml(XML::gcXMLElement &xmlElement)
{
#ifdef NIX
	std::wstring copy(m_szPath);
	std::replace(copy.begin(), copy.end(), '/', '\\');
#endif

	xmlElement.WriteChild("name", gcString(m_szName));
#ifdef NIX
	xmlElement.WriteChild("path", gcString(copy));
#else
	xmlElement.WriteChild("path", gcString(m_szPath));
#endif
	xmlElement.WriteChild("nom_csum", m_szCsum);
	xmlElement.WriteChild("com_csum", m_szCCsum);

	xmlElement.WriteChild("size", m_ullSize);
	xmlElement.WriteChild("csize", m_ullCSize);
	xmlElement.WriteChild("flags", m_uiFlags);

	xmlElement.WriteChild("offset", m_ullOffset);
	xmlElement.WriteChild("tstamp", m_ullTimeStamp);
}


bool UMcfFile::checkFile(const wchar_t* dir)
{
#ifdef NIX
	gcString path("{0}/{1}/{2}", dir, m_szPath, m_szName);

	struct stat stFileInfo;
	int intStat = stat(path.c_str(), &stFileInfo);

#ifdef DEBUG
	char buffer[PATH_MAX];
	snprintf(buffer, PATH_MAX, "%s (%s)", __func__, path.c_str());
	ERROR_OUTPUT(buffer);
#endif

#else
	gcWString path("{0}\\{1}\\{2}", dir, m_szPath, m_szName);
	struct _stat64i32 stFileInfo;

	// Attempt to get the file attributes
	int intStat = _wstat(path.c_str(), &stFileInfo);
#endif

	return (intStat == 0);
}

bool UMcfFile::isSame(UMcfFile* file)
{
	if (!file || !this->getPath() || !file->getPath() || !this->getName() || !file->getName())
		return false;

	return (Safe::wcsicmp(getName(), file->getName()) == 0 && Safe::wcsicmp(getPath(), file->getPath()) == 0);
}

bool UMcfFile::verifyFile(FILEHANDLE hFile, uint64 baseOffset)
{
#ifdef WIN32
	LARGE_INTEGER mov;
	mov.QuadPart = baseOffset + getOffSet();

	if (SetFilePointerEx(hFile, mov, nullptr, FILE_BEGIN) == 0)
		return false;
#else
	fseek(hFile, baseOffset + getOffSet(), SEEK_SET);
#endif

	std::string temp;
	
	if (getFlags() & UMCFF_FLAG_ZEROSIZE)
		return true;

	if (getFlags() & UMCFF_FLAG_COMPRESSED)
		temp = UTIL::MISC::hashFile((FHANDLE)hFile, m_ullCSize);
	else
		temp = UTIL::MISC::hashFile((FHANDLE)hFile, m_ullSize);

	return verify(temp.c_str());
}

bool UMcfFile::verify(const char* hash)
{
	if (!hash)
		return false;

	if (getFlags() & UMCFF_FLAG_COMPRESSED)
		return m_szCCsum == std::string(hash);

	return m_szCsum == std::string(hash);
}



bool checkFile(const char* dir)
{
	return true;
}

void RecMakeFolder(UTIL::FS::Path path)
{
	std::string subPath;

	for (size_t x=0; x<path.getFolderCount(); x++)
	{
		subPath += path.getFolder(x);
		subPath += path.GetDirSeperator();

#ifdef WIN32
		//skip drive letters
		if (x == 0 && subPath.size() == 3 && subPath[1] == ':')
			continue;
#endif

		if (!CreateDir(gcWString(subPath).c_str()))
			throw gcException(ERR_BADPATH, GetLastError(), gcString("Failed to make path: {0}", subPath));
	}
}


uint8 UMcfFile::decompressAndSave(FILEHANDLE hSrc, FILEHANDLE hSink, uint64 offset, DelegateI<ProgressCB&> *del)
{
	UTIL::MISC::Buffer buff(BUFFSIZE, true);
	const size_t buffsize = BUFFSIZE;

	if (!FileSeek(hSrc, offset + m_ullOffset))
		return MCFF_ERR_INVALIDFILE;

	uint64 leftToDo = m_ullCSize;
	bool end = false;

	UTIL::MISC::BZ2Worker worker(UTIL::MISC::BZ2_DECOMPRESS);

	do
	{
		size_t curSize = BUFFSIZE;

		if (curSize > leftToDo)
		{
			end = true;
			curSize = (size_t)leftToDo;
		}

		if (curSize == 0)
			return MCFF_ERR_PARTREAD;

		if (!FileRead(hSrc, curSize, buff))
			return MCFF_ERR_FAILEDREAD;

		leftToDo -= curSize;

		worker.write(buff, curSize, end);
		worker.doWork();

		size_t b = 0;

		do
		{
			b = buffsize;
			worker.read(buff.data(), b);

			if (!FileWrite(hSink, b, buff))
				return MCFF_ERR_FAILEDWRITE;
		}
		while (b > 0);

		ProgressCB p(m_ullCSize - leftToDo);

		if (del)
			del->operator()(p);

		if (p.cancel)
			return MCFF_ERR_CANCELED;

	}
	while (!end);

	return MCFF_OK;
}

uint8 UMcfFile::saveData(FILEHANDLE hSrc, FILEHANDLE hSink, uint64 offset, DelegateI<ProgressCB&> *del)
{
	UTIL::MISC::Buffer buff(BUFFSIZE, true);
	const size_t buffsize = BUFFSIZE;

	if (!FileSeek(hSrc, offset + m_ullOffset))
		return MCFF_ERR_INVALIDFILE;

	uint64 done = 0;

	do
	{
		size_t curSize = buffsize;

		if ((m_ullSize-done) < buffsize)
			curSize = (uint32)(m_ullSize-done);

		if (!FileRead(hSrc, curSize, buff))
			return MCFF_ERR_FAILEDREAD;

		if (!FileWrite(hSink, curSize, buff))
			return MCFF_ERR_FAILEDWRITE;

		done += curSize;
		ProgressCB p(done);

		if (del)
			del->operator()(p);

		if (p.cancel)
			return MCFF_ERR_CANCELED;
	}
	while (done < m_ullSize);

	return MCFF_OK;
}

void UMcfFile::remove(const wchar_t* dir)
{

#ifdef WIN32
	gcWString path("{0}\\{1}\\{2}", dir, getPath(), getName());
	FileDelete(path.c_str());
#else
	gcString path("{0}/{1}/{2}", dir, getPath(), getName());
	::remove(path.c_str());
#endif
}

class AutoCleanupDelegate
{
public:
	AutoCleanupDelegate(DelegateI<ProgressCB&>* del)
		: m_pDel(del)
	{
	}

	~AutoCleanupDelegate()
	{
		if (m_pDel)
			m_pDel->destroy();
	}

	DelegateI<ProgressCB&> *m_pDel;
};

uint8 UMcfFile::readMCFAndSave(FILEHANDLE hFile, const wchar_t* dir, uint64 offset, DelegateI<ProgressCB&> *del)
{
	AutoCleanupDelegate acd(del);

	if (!IsValidFileHandle(hFile))
		return MCFF_ERR_INVALIDHANDLE;

	if (!dir)
		return MCFF_ERR_INVALIDPATH;

	UTIL::FS::Path path(gcString(dir), gcString(m_szName), false);
	path += gcString(m_szPath);

	RecMakeFolder(path);

	FILEHANDLE hSaveFile;

	std::string fullPath = path.getFullPath();

	if (!FileOpen(hSaveFile, gcWString(fullPath).c_str(), FILE_WRITE))
		return MCFF_ERR_INVALIDHANDLE;

	if (getFlags() & UMCFF_FLAG_ZEROSIZE)
	{
		FileClose(hSaveFile);
		return MCFF_OK;
	}

	uint8 res = 0;

	if (getFlags() & UMCFF_FLAG_COMPRESSED)
		res = decompressAndSave(hFile, hSaveFile, offset, del);
	else
		res = saveData(hFile, hSaveFile, offset, del);

	FileClose(hSaveFile);

#ifdef NIX
	struct stat s;

	if (HasAnyFlags(getFlags(), UMCFF_FLAG_XECUTABLE) && stat(fullPath.c_str(), &s) == 0)
		chmod(fullPath.c_str(), (s.st_mode|S_IXUSR|S_IRWXG|S_IRWXO));
#endif	

	if (m_szCsum.size() == 0)
	{
		printf("File has no md5 hash: %ls\n", m_szName.c_str());
		return MCFF_ERR_BADHASH;
	}

	std::string fileHash = UTIL::MISC::hashFile(path.getFullPath().c_str());
	if (fileHash != m_szCsum)
	{
		printf("Hash failed on file: %ls\n", m_szName.c_str());
		return MCFF_ERR_BADHASH;
	}
	
	return res;
}
