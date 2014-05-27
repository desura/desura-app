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

#ifndef DESURA_GCJSUPLOADS_H
#define DESURA_GCJSUPLOADS_H
#ifdef _WIN32
#pragma once
#endif

#include "gcJSBase.h"

namespace UserCore
{
	namespace Misc
	{
		class UploadInfoThreadI;
	}

	namespace Item
	{
		class ItemInfoI;
	}
}

class DesuraJSUploadInfo : public DesuraJSBase<DesuraJSUploadInfo>
{
public:
	DesuraJSUploadInfo();

	bool preExecuteValidation(const char* function, uint32 functionHash, JSObjHandle object, size_t argc, JSObjHandle* argv) override;


private:
	gcRefPtr<UserCore::Misc::UploadInfoThreadI> getUploadFromId(int32 uid);

	bool isValid(gcRefPtr<UserCore::Misc::UploadInfoThreadI> upload);
	bool isComplete(gcRefPtr<UserCore::Misc::UploadInfoThreadI> upload);
	bool hasError(gcRefPtr<UserCore::Misc::UploadInfoThreadI> upload);
	bool hasStarted(gcRefPtr<UserCore::Misc::UploadInfoThreadI> upload);
	bool isPaused(gcRefPtr<UserCore::Misc::UploadInfoThreadI> upload);
	bool shouldDeleteMcf(gcRefPtr<UserCore::Misc::UploadInfoThreadI> upload);

	void setDeleteMcf(gcRefPtr<UserCore::Misc::UploadInfoThreadI> upload, bool del);
	void remove(gcRefPtr<UserCore::Misc::UploadInfoThreadI> upload);
	void pause(gcRefPtr<UserCore::Misc::UploadInfoThreadI> upload);
	void unpause(gcRefPtr<UserCore::Misc::UploadInfoThreadI> upload);
	void cancel(gcRefPtr<UserCore::Misc::UploadInfoThreadI> upload);
	void exploreMcf(gcRefPtr<UserCore::Misc::UploadInfoThreadI> upload);

	gcRefPtr<UserCore::Item::ItemInfoI> getItem(gcRefPtr<UserCore::Misc::UploadInfoThreadI> upload);
	int32 getProgress(gcRefPtr<UserCore::Misc::UploadInfoThreadI> upload);
	gcString getFileName(gcRefPtr<UserCore::Misc::UploadInfoThreadI> upload);
	gcString getUploadId(gcRefPtr<UserCore::Misc::UploadInfoThreadI> upload);
	gcString getItemId(gcRefPtr<UserCore::Misc::UploadInfoThreadI> upload);

	uint32 m_uiIsValidHash;
	uint32 m_uiGetUploadFromIdHash;
};

#endif //DESURA_GCJSUPLOADS_H
