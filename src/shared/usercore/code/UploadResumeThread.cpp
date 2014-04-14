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
#include "UploadResumeThread.h"

namespace UserCore
{
namespace Thread
{


UploadResumeThread::UploadResumeThread(DesuraId id, const char* key, WebCore::Misc::ResumeUploadInfo *info) : MCFThread("UploadResume Thread", id)
{
	m_szKey = gcString(key);
	m_pUpInfo = info;
}

void UploadResumeThread::doRun()
{
	gcAssert(m_pUpInfo);

	getWebCore()->resumeUpload(getItemId(), m_szKey.c_str(), *m_pUpInfo);

	const char* localFilePath = getUploadManager()->findUpload(m_szKey.c_str());
	gcString sLocalFP(localFilePath);

	if (localFilePath && validFile(localFilePath))
	{
		onCompleteStrEvent(sLocalFP);
		return;
	}

	//gona try and search for the file in the data/mods/id folder
	const char* appDataPath = getUserCore()->getAppDataPath();
	gcString searchPath("{0}{1}{2}", appDataPath, DIRS_STR, getItemId().getFolderPathExtension());
	gcString sNull("nullptr");

	if (doSearch(searchPath.c_str()))
		return;

	if (getUserCore()->isAdmin())
	{
		searchPath = gcString("{0}{1}temp{1}", appDataPath, DIRS_STR);
		
		if (doSearch(searchPath.c_str()))
			return;
	}

	onCompleteStrEvent(sNull);
}

bool UploadResumeThread::doSearch(const char* path)
{
	if (!path)
		return false;

	std::vector<UTIL::FS::Path> fileList;
	std::vector<std::string> extList;

	extList.push_back("mcf");

	UTIL::FS::getAllFiles(UTIL::FS::PathWithFile(path), fileList, &extList);

	for (size_t x=0; x<fileList.size(); x++)
	{
		if (validFile(fileList[x].getFullPath().c_str()))
		{
			gcString fullPath = fileList[x].getFullPath();
			onCompleteStrEvent(fullPath);
			return true;
		}
	}

	return false;
}

bool UploadResumeThread::validFile(const char* szPath)
{
	if (!m_pUpInfo || !szPath)
		return false;

	UTIL::FS::Path path = UTIL::FS::PathWithFile(szPath);

	if (!UTIL::FS::isValidFile(path))
		return false;

	std::string md5 = UTIL::MISC::hashFile(path.getFullPath());
	bool res = (UTIL::FS::getFileSize(path) == m_pUpInfo->size && md5 == m_pUpInfo->szHash);
	return res;
}

}
}
