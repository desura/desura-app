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

#include "common.h"
#include "InstallHandler.h"
#include "User.h"

InstallHandler::InstallHandler(UserCore::Item::ItemHandle* handle, const char* path) : BaseHandler(handle)
{
	m_szPath = path;
	
}

InstallHandler::~InstallHandler()
{

}







//res will be 1 on error (i.e. hash fail) and 0 if no error
void InstallHandler::onComplete(uint32 res)
{

}

void InstallHandler::onError(gcException e)
{

}

void InstallHandler::onMcfProgress(MCFCore::Misc::ProgressInfo& info)
{

}