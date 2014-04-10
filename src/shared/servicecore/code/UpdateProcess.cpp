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
#include "UpdateProcess.h"

#include "McfInit.h"
#include "umcf/UMcf.h"

#ifdef WIN32
#include <ShlObj.h>
#endif

GCUpdateProcess::GCUpdateProcess(const char* mcfpath, const char* inspath, bool testMode) : Thread::BaseThread("UpdateProcess Thread")
{
	m_bTestMode = testMode;

	m_szIPath = gcWString(inspath);
	m_szMCFPath= gcWString(mcfpath);

	m_pUMcf = nullptr;
	m_pOldMcf = nullptr;
}


GCUpdateProcess::~GCUpdateProcess()
{
	stop();
	safe_delete(m_pUMcf);
	safe_delete(m_pOldMcf);
}


void GCUpdateProcess::run()
{
	try
	{
		install();
	}
	catch (gcException &e)
	{
		onErrorEvent(e);
	}

#ifdef WIN32
	if (!m_bTestMode)
	{
		//remove patch
		//rename it first incase its in use
		gcWString newName = m_szMCFPath;
		newName += L"_old";


		DWORD res1 = DeleteFileW(newName.c_str());
		DWORD res2 = MoveFileW(m_szMCFPath.c_str(), newName.c_str());
		DWORD res3 = DeleteFileW(newName.c_str());
	}
#endif

	onCompleteEvent();
}


void GCUpdateProcess::install()
{
	gcTrace("");

	if (m_szIPath == L"" || m_szMCFPath == L"")
		throw gcException(ERR_BADPATH,"One of the paths for install was nullptr.");

	m_pUMcf = new UMcf();
	m_pUMcf->setFile(m_szMCFPath.c_str());
	m_pUMcf->parseMCF();
	m_pUMcf->onProgressEvent += delegate(&onProgressEvent);

	//make sure patch is newer than currently installed version
	if (!m_pUMcf->isUpdateNewer())
	{
		//dont do shit. :P
	}
	else if (m_pUMcf->isValidInstaller())
	{
		UTIL::FS::recMakeFolder(gcString(m_szIPath).c_str());

		gcWString xmlPath(m_szIPath);

		xmlPath += DIRS_WSTR;
		xmlPath += UPDATEXML_W;

		if (!m_bTestMode)
			deleteDumps();
		
		m_pOldMcf = new UMcf();
		
		if (m_pOldMcf->loadFromFile(xmlPath.c_str()) != MCF_OK)
			throw gcException(ERR_INVALID, "Failed to load xml file");

		UTIL::FS::delFile(gcString(xmlPath));

		m_pUMcf->install(m_szIPath.c_str());
		m_pUMcf->dumpXml(xmlPath.c_str());

		m_pUMcf->removeOldFiles(m_pOldMcf, m_szIPath.c_str());

		if (!m_bTestMode)
		{
			gcString szIPath(m_szIPath);

			m_pUMcf->setRegValues(szIPath.c_str());
			
			if (!updateService(szIPath.c_str()))
				throw gcException(ERR_INVALID, "Failed to move service to common files.");
		}
	}
	else
	{
		UTIL::FS::delFile(gcString(m_szMCFPath));
		throw gcException(ERR_INVALIDFILE, gcString("The MCF file {0} is an invalid installer.", m_szMCFPath));
	}

	safe_delete(m_pOldMcf);
	safe_delete(m_pUMcf);
}

bool GCUpdateProcess::updateService(const char* curDir)
{
	gcString cpf = UTIL::OS::getCommonProgramFilesPath();

	UTIL::FS::Path curPath(curDir, "desura_service.exe", false);
	UTIL::FS::Path destPath(cpf, "desura_service.exe", false);
	UTIL::FS::Path oldPath(cpf, "desura_service_old.exe", false);

	if (!UTIL::FS::isValidFolder(destPath))
		UTIL::FS::recMakeFolder(destPath);

	UTIL::FS::delFile(oldPath);
	UTIL::FS::moveFile(destPath, oldPath);
	UTIL::FS::copyFile(curPath, destPath);

	if (!UTIL::FS::isValidFile(destPath))
		return false;

	UTIL::OS::setConfigValue(gcString("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\{0}\\ImagePath", SERVICE_NAME), destPath.getFullPath());

	return true;
}

void GCUpdateProcess::deleteDumps()
{
	gcString path = UTIL::OS::getAppDataPath(L"dumps");
	UTIL::FS::delFolder(path);
}

bool GCUpdateProcess::fixDataDir()
{
#ifdef WIN32
	wchar_t comAppPathW[MAX_PATH];
	SHGetFolderPathW(nullptr, CSIDL_COMMON_APPDATA , nullptr, SHGFP_TYPE_CURRENT, comAppPathW);

	char comAppPathA[MAX_PATH];
	SHGetFolderPathA(nullptr, CSIDL_COMMON_APPDATA , nullptr, SHGFP_TYPE_CURRENT, comAppPathA);

	UTIL::FS::copyFolder(comAppPathA, gcString(comAppPathW), nullptr, false);
	UTIL::FS::delFolder(comAppPathA);
#endif
	return true;
}
