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
#include "UploadPrepThread.h"
#include "UploadInfoThread.h"

namespace UserCore
{
namespace Thread
{

UploadPrepThread::UploadPrepThread(DesuraId id, const char* file) : MCFThread( "UploadPrep Thread", id )
{
	m_szFile = gcString(file);
}

void UploadPrepThread::doRun()
{
	UTIL::FS::Path path = UTIL::FS::PathWithFile(m_szFile);

	if (!UTIL::FS::isValidFile(path))
		throw gcException(ERR_INVALIDFILE);


	McfHandle mcfHandle;;

	bool validMcf = false;

	mcfHandle->setFile(m_szFile.c_str());
	mcfHandle->parseMCF();

	MCFCore::MCFHeaderI* mcfHead = mcfHandle->getHeader();

	DesuraId mcfId = mcfHead->getDesuraId();
	DesuraId itemId =  getItemId();

	validMcf = (mcfHead->isValid() && (!mcfId.isOk() || mcfId == itemId));

	if (!validMcf)
		throw gcException(ERR_INVALIDMCF);


	std::string hash = UTIL::MISC::hashFile(path.getFullPath().c_str());
	uint64 fileSize = UTIL::FS::getFileSize(path);

	char* key = nullptr;

	try
	{
		getWebCore()->newUpload(getItemId(), hash.c_str(), fileSize, &key);
	}
	catch (gcException &)
	{
		safe_delete(key);
		throw;
	}

	getUploadManager()->addUpload(getItemId(), key, m_szFile.c_str());
	m_szKey = gcString(key);
	safe_delete(key);

	onCompleteStrEvent(m_szKey);
}

}
}