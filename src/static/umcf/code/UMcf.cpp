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
#include "umcf/UMcf.h"
#include "XMLMacros.h"

#include "bzlib.h"
#include "UMcfFile_utils.h"

#include "BZip2.h"

#include <branding/branding.h>

#ifdef WIN32
class FileHandle
{
public:
	FileHandle(const char* path, uint64 offset)
	{
		hFileSrc = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);

		if (offset != 0)
			FileSeek(hFileSrc, offset);
	}

	FileHandle(const wchar_t* path, uint64 offset)
	{
		hFileSrc = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);

		if (offset != 0)
			FileSeek(hFileSrc, offset);
	}

	~FileHandle()
	{
		CloseHandle(hFileSrc);
	}

	bool isValid()
	{
		return (hFileSrc != INVALID_HANDLE_VALUE);
	}

	HANDLE hFileSrc;
};

#else

class FileHandle
{
public:
	FileHandle(const char* path, uint64_t offset)
	{
		hFileSrc = fopen64(path, "rb");
		
		if (offset != 0)
			fseek(hFileSrc, offset, SEEK_SET);
	}

	FileHandle(const wchar_t* path, uint64_t offset)
	{
		gcString p(path);
		hFileSrc = fopen64(p.c_str(), "rb");
		
		if (offset != 0)
			fseek(hFileSrc, offset, SEEK_SET);
	}

	~FileHandle()
	{
		fclose(hFileSrc);
	}

	bool isValid()
	{
		return (hFileSrc != nullptr);
	}

	FILE* hFileSrc;
};

#endif

namespace
{

#ifdef WIN32
	const std::vector<wchar_t*> g_vProblemFiles =
	{
		L"desura.exe",
		L"desura_service.exe",
		L"servicecore.dll",
		L"desura_browserhost.exe",
		L"mfc120.dll",
		L"msvcp120.dll",
		L"msvcr120.dll"
	};

	const std::vector<wchar_t*> g_vProblemFilesPath =
	{
		L"desura.exe",
		L"desura_service.exe",
		L"bin\\servicecore.dll",
		L"desura_browserhost.exe",
		L"bin\\mfc120.dll",
		L"bin\\msvcp120.dll",
		L"bin\\msvcr120.dll"
	};
#endif
}

UMcf::UMcf()
	: m_sHeader(std::make_unique<UMcfHeader>())
{
}

UMcf::~UMcf()
{
}

void UMcf::setFile(const wchar_t* file, uint64 offset)
{
	if (file)
		m_szFile = file;
	else
		m_szFile = L"";

	m_uiOffset = offset;
}

uint8 UMcf::parseXml(char* buff, size_t buffLen)
{
	UTIL::MISC::BZ2Worker worker(UTIL::MISC::BZ2_DECOMPRESS);

	gcBuff pTempBuff(1);

	if (!(m_sHeader->getFlags() & MCFCore::MCFHeaderI::FLAG_NOTCOMPRESSED))
	{
		worker.write(buff, buffLen, true);
		worker.doWork();

		buffLen = worker.getReadSize();

		pTempBuff = gcBuff(buffLen);
		worker.read(pTempBuff, buffLen);
	}

	XML::gcXMLDocument doc(pTempBuff, buffLen);
	return parseXml(doc.GetRoot("files"));
}


uint8 UMcf::parseXml(const XML::gcXMLElement &xmlElement)
{
	if (!xmlElement.IsValid())
		return UMCF_ERR_XML_NOPRIMENODE;

	xmlElement.for_each_child("file", [this](const XML::gcXMLElement &xmlChild)
	{
		auto temp = std::make_shared<UMcfFile>();

		if (temp->loadXmlData(xmlChild) == UMCF_OK)
			m_pFileList.push_back(temp);
	});

	return UMCF_OK;
}




#ifdef WIN32
void UMcf::removeOldFiles(const wchar_t* installPath)
{
	for (auto p : g_vProblemFilesPath)
	{
		wchar_t src[255] = {0};
		Safe::snwprintf(src, 255, L"%s\\%s_old", installPath, p);
		DeleteFileW(src);
	}
}

void UMcf::moveOldFiles(const wchar_t* installPath, const wchar_t* fileName)
{
	gcAssert(g_vProblemFilesPath.size() == g_vProblemFiles.size());

	size_t x = 0;

	for (auto p : g_vProblemFiles)
	{
		x++;

		if (Safe::wcsicmp(fileName, p) != 0)
			continue;

		auto probFilePath = g_vProblemFilesPath[x - 1];

		wchar_t src[255] = {0};
		wchar_t dest[255] = {0};

		Safe::snwprintf(src, 255, L"%s\\%s", installPath, probFilePath);
		Safe::snwprintf(dest, 255, L"%s\\%s_old", installPath, probFilePath);

		//cant copy over a running exe.
		MoveFileExW(src, dest, MOVEFILE_WRITE_THROUGH);
		return;
	}
}
#endif

void UMcf::disableMoveOldFiles()
{
	m_bShouldMoveOldFiles = false;
}

void UMcf::cancel()
{
	m_bCanceled = true;
}

bool SortMcfFiles(const std::shared_ptr<UMcfFile>& a, const std::shared_ptr<UMcfFile>& b)
{
	return a->getOffSet() < b->getOffSet();
}

void UMcf::install(const wchar_t* path)
{
	m_bCanceled = false;
	FileHandle fh(m_szFile.c_str(), m_uiOffset);

	if (!fh.isValid())
		throw gcException(ERR_INVALIDFILE, GetLastError(), gcString("Failed to open update file {0}.", m_szFile));

#ifdef WIN32
	if (m_bShouldMoveOldFiles)
		removeOldFiles(path);
#endif

	//sort by offset so we are not seeking all over the place
	std::sort(m_pFileList.begin(), m_pFileList.end(), &SortMcfFiles);

	m_uiTotProgress = 0;
	m_uiCurProgress = 0;

	for (size_t x=0; x<m_pFileList.size(); x++)
	{
		if (m_pFileList[x]->isCompressed())
			m_uiTotProgress += m_pFileList[x]->getCSize();
		else
			m_uiTotProgress += m_pFileList[x]->getSize();
	}

	for(size_t x=0; x<m_pFileList.size(); x++)
	{
		if (m_bCanceled)
			throw gcException(ERR_USERCANCELED);

		if (!m_pFileList[x] || !m_pFileList[x]->isSaved())
			continue;

#ifdef WIN32
		if (m_bShouldMoveOldFiles)
			moveOldFiles(path, m_pFileList[x]->getName());
#else
		m_pFileList[x]->remove(path);
#endif

		uint8 res = m_pFileList[x]->readMCFAndSave(fh.hFileSrc, path?path:L".\\", m_uiOffset, delegate(this, &UMcf::onFileProgress));
		
		if (m_bCanceled)
			throw gcException(ERR_USERCANCELED);
			
		if (res == MCFF_ERR_BADHASH)
			throw gcException(ERR_CSUM_FAILED, gcString("Check sum failed on file {0}\\{1}", m_pFileList[x]->getPath(), m_pFileList[x]->getName()));

		if (res == MCFF_ERR_INVALIDHANDLE)
			throw gcException(ERR_INVALID, gcString("Could not open file {0}\\{1} for updating. Sys Code: {2}", m_pFileList[x]->getPath(), m_pFileList[x]->getName(), GetLastError()));

		if (res != 0 && res != MCF_NOTCOMPRESSED)
			throw gcException(ERR_INVALID, gcString("Failed to update file {0}\\{1}: {2}.", m_pFileList[x]->getPath(), m_pFileList[x]->getName(), (uint32)res));

		if (m_pFileList[x]->isCompressed())
			m_uiCurProgress += m_pFileList[x]->getCSize();
		else
			m_uiCurProgress += m_pFileList[x]->getSize();
	}
}

void UMcf::onFileProgress(ProgressCB& prog)
{
	prog.cancel = m_bCanceled;

	uint32 per = (uint32)( (uint64)(m_uiCurProgress + prog.done)*100/m_uiTotProgress );
	onProgressEvent(per);
}

uint8 UMcf::loadFromFile(const wchar_t* file)
{
	gcString strFile(file);
	XML::gcXMLDocument doc(strFile.c_str());

	if (!doc.IsValid())
		return MCF_ERR_INVALIDHANDLE;

	parseUpdateXml(doc);

	if (m_pFileList.size() == 0)
		return MCF_ERR_FAILEDREAD;

	return MCF_OK;
}

void UMcf::parseUpdateXml(const XML::gcXMLDocument &xmlDocument)
{
	auto uNode = xmlDocument.GetRoot("appupdate");

	if (!uNode.IsValid())
		return;

	auto mcfNode = uNode.FirstChildElement("mcf");

	if (!mcfNode.IsValid())
		return;


	const std::string appid = mcfNode.GetAtt("appid");

	if (!appid.empty())
		m_iAppId = Safe::atoi(appid.c_str());
	else
		m_iAppId = 100;

	const std::string build = mcfNode.GetAtt("build");

	if (!build.empty())
		m_iAppBuild = Safe::atoi(build.c_str());
	else
		m_iAppBuild = 0;

	mcfNode.GetChild("url", m_szUrl);
	parseXml(mcfNode.FirstChildElement("files"));
}

//checks local files. returns true if they are good false if they are bad
bool UMcf::checkFiles()
{
	return checkFiles(nullptr);
}

//checks local files. returns true if they are good false if they are bad
bool UMcf::checkFiles(IBadFileCallback *pCallback)
{
	uint32 prog = 0;

	onProgressEvent(prog);

	size_t size = m_pFileList.size();

	if (size == 0)
		return false;

	auto bValidInstall = true;

	for(size_t x=0; x<size; x++)
	{
		prog = (uint32)((x*100)/size);
		onProgressEvent(prog);

		if (!m_pFileList[x])
			continue;

		if (!m_pFileList[x]->checkFile(L"."))
		{
			if (!pCallback || pCallback->foundBadFile(m_pFileList[x]->getName(), m_pFileList[x]->getPath()))
				return false;

			bValidInstall = false;
		}
	}

	return bValidInstall;
}

//checks the header and makes sure its a valid installer
bool UMcf::isValidInstaller()
{
	if (!m_sHeader->isValid())
		return false;

	if (m_sHeader->getType() != TYPE_APP)
		return false;

	FileHandle fh(m_szFile.c_str(), m_uiOffset);

	if(!fh.isValid())
		return false;

	uint32 count = 0;

	for(size_t x=0; x<m_pFileList.size(); x++)
	{
		if (!m_pFileList[x] || !m_pFileList[x]->isSaved())
			continue;

		if (!m_pFileList[x]->verifyFile(fh.hFileSrc, m_uiOffset))
		{
			printf("Not valid: %s\n", gcString(m_pFileList[x]->getName()).c_str());
			return false;
		}
		
		count++;
	}

	return (count != 0);
}


void UMcf::updateAllCB(Prog_s& p)
{
	onDownloadProgressEvent(p);
}

uint32 UMcf::progressUpdate(Prog_s *info, uint32 other)
{
	if (!info)
		return other;

	uint64 done = (uint32)info->dlnow; //+= (uint64)size;
	uint32 per = 0;

	if (done > 0)
		per = (uint32)(done*100/info->dltotal);

	return per;
}

void UMcf::setRegValues(const char* szInstallPath)
{
#ifdef WIN32
	char exePath[255];

	if (!szInstallPath)
	{
		GetModuleFileName(nullptr, exePath, 255);

		size_t exePathLen = strlen(exePath);
		for (size_t x = exePathLen; x > 0; x--)
		{
			auto c = exePath[x];
			exePath[x] = '\0';

			if (c == '\\')
				break;
		}

		szInstallPath = exePath;
	}

	if (m_sHeader && m_sHeader->getId())
	{
		char appid[100];
		char build[100];

		Safe::snprintf(appid, 100, "%d", m_sHeader->getId());
		Safe::snprintf(build, 100, "%d", m_sHeader->getBuild());

		UTIL::WIN::setRegValue(APPID, appid);
		UTIL::WIN::setRegValue(APPBUILD, build);

		UTIL::WIN::setRegValue("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Desura\\DisplayVersion", gcString("{0}.{1}", appid, build));
		UTIL::WIN::setRegValue("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Desura\\VersionMajor", m_sHeader->getId());
		UTIL::WIN::setRegValue("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Desura\\VersionMinor", m_sHeader->getBuild());
	}

	std::string command = gcString("\"{0}\\desura.exe\" \"{1}\" -urllink", szInstallPath, "%1");

	UTIL::WIN::setRegValue("HKEY_CLASSES_ROOT\\Desura\\", "URL:Desura Protocol");
	UTIL::WIN::setRegValue("HKEY_CLASSES_ROOT\\Desura\\URL Protocol", "");
	UTIL::WIN::setRegValue("HKEY_CLASSES_ROOT\\Desura\\shell\\open\\command\\", command);
	UTIL::WIN::setRegValue("HKEY_LOCAL_MACHINE\\Software\\Desura\\DesuraApp\\InstallPath", szInstallPath);
	UTIL::WIN::delRegKey("HKEY_LOCAL_MACHINE\\Software\\DesuraNET");

	UTIL::WIN::setRegValue("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Desura\\DisplayName", PRODUCT_NAME);
	UTIL::WIN::setRegValue("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Desura\\UninstallString", gcString("{0}\\Desura_Uninstaller.exe", szInstallPath));
	UTIL::WIN::setRegValue("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Desura\\InstallLocation", szInstallPath);
	UTIL::WIN::setRegValue("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Desura\\DisplayIcon", gcString("{0}\\desura.exe", szInstallPath));
	UTIL::WIN::setRegValue("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Desura\\HelpLink", "http://www.desura.com");
	UTIL::WIN::setRegValue("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Desura\\URLInfoAbout", "http://www.desura.com/about");
	UTIL::WIN::setRegValue("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Desura\\NoRepair", "0");
	UTIL::WIN::setRegValue("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Desura\\NoModify", "1");
	UTIL::WIN::setRegValue("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Desura\\RegCompany", "Desura");
	UTIL::WIN::setRegValue("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Desura\\Publisher", "Desura");

#else
	FILE* fh = fopen("version", "w");

	if (fh)
	{
		fprintf(fh, "BRANCH=%u\nBUILD=%u", m_sHeader->getId(), (uint32)m_sHeader->getBuild());
		fclose(fh);
	}
	else
	{
		ERROR_OUTPUT("Failed to open version file to write version info.");
	}
#endif
}

bool UMcf::isUpdateNewer()
{
#ifdef WIN32
	DWORD res;
	HKEY hk;

	char lszAppId[255];
	char lszBuild[255];
	DWORD dwType=REG_SZ;
	DWORD dwSize=255;

	res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Desura\\DesuraApp", 0, KEY_READ, &hk);

	if (res != ERROR_SUCCESS)
		return true;

	DWORD err1 = RegQueryValueEx(hk, "appid", nullptr, &dwType,(LPBYTE)&lszAppId, &dwSize);
	DWORD err2 = RegQueryValueEx(hk, "appver", nullptr, &dwType,(LPBYTE)&lszBuild, &dwSize);
	RegCloseKey(hk);

	if (err1 != ERROR_SUCCESS || err2 != ERROR_SUCCESS)
		return true;

	uint32 appid = Safe::atoi(lszAppId);
	uint32 build = Safe::atoi(lszBuild);
#else
	FILE* fh = fopen("version", "r");

	if (!fh)
		return true;
	
	uint32 appid = -1;
	uint32 build = -1;

	int res = fscanf(fh, "BRANCH=%u\nBUILD=%u", &appid, &build);
	fclose(fh);

	if (res != 2)
		return true;
#endif

	//return true if not same app type or this mcf is newer
	return (appid != m_iAppId || (appid == m_iAppId && m_iAppBuild >= build));
}

void UMcf::dumpXml(const wchar_t* path)
{
	if (!path)
		return;

	XML::gcXMLDocument doc;

	auto root = doc.Create("appupdate");
	auto mcfElFiles = root.NewElement("mcf");

	mcfElFiles.SetAttribute("build",  m_sHeader->getBuild());
	mcfElFiles.SetAttribute("appid",  m_sHeader->getId());

	auto filesElFiles = mcfElFiles.NewElement( "files" );

	for (auto file : m_pFileList)
	{
		auto el = filesElFiles.NewElement("file");
		file->genXml(el);
	}

	doc.SaveFile(gcString(path).c_str());
}

void UMcf::removeOldFiles(UMcf* oldMcf, const wchar_t* dir)
{
	for (size_t x=0; x<oldMcf->m_pFileList.size(); x++)
	{
		UMcfFile* file = oldMcf->m_pFileList[x].get();

		if (!file)
			continue;

		bool found = false;

		for (size_t y=0; y<m_pFileList.size(); y++)
		{
			if (!m_pFileList[y])
				continue;

			if (m_pFileList[y]->isSame(file))
			{
				found = true;
				break;
			}
		}

		if (!found)
			file->remove(dir);
	}

}

uint8 UMcf::parseMCF()
{
	FileHandle fh(m_szFile.c_str(), m_uiOffset);

	if (!fh.isValid())
		return MCF_ERR_INVALIDHANDLE;

	uint64 fileSize = FileSize(fh.hFileSrc) - m_uiOffset;

	if (fileSize == 0)
		return MCF_ERR_EMPTYFILE;

	m_pFileList.erase(m_pFileList.begin(), m_pFileList.end());

	{
		std::unique_ptr<UMcfHeader> tempHeader = std::make_unique<UMcfHeader>();
		uint8 err = tempHeader->readFromFile(fh.hFileSrc);

		if (err != MCF_OK)
			return MCF_ERR_BADHEADER;

		m_sHeader = std::move(tempHeader);
	}

	if (!FileSeek(fh.hFileSrc, m_uiOffset + m_sHeader->getXmlStart()))
		return MCF_ERR_FAILEDSEEK;

	m_iAppId = m_sHeader->getId();
	m_iAppBuild = m_sHeader->getBuild();

	uint32 xmlBuffLen = m_sHeader->getXmlSize()+1;
	UTIL::MISC::Buffer buf(xmlBuffLen, true);

	bool res = FileRead(fh.hFileSrc, m_sHeader->getXmlSize(), buf);

	if (!res)
		return MCF_ERR_FAILEDREAD;

	return parseXml(buf, m_sHeader->getXmlSize());
}

std::string UMcf::getFileHash(const std::wstring& fileName, const std::wstring& path)
{
	for (size_t x=0; x<m_pFileList.size(); x++)
	{
		if (fileName == m_pFileList[x]->getName() && path == m_pFileList[x]->getPath())
			return m_pFileList[x]->getCsum();
	}

	return "";
}

void UMcf::deleteAllFiles(const wchar_t* path)
{
	for (size_t x=0; x<m_pFileList.size(); x++)
		m_pFileList[x]->remove(path);
}
