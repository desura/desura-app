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
#include "InstallINCPage.h"

namespace UI
{
namespace Forms
{
namespace ItemFormPage
{


InstallINCPage::InstallINCPage(wxWindow* parent) : InstallINPage(parent)
{
	m_uiLastFlag = -1;
	m_labLabel->SetLabel("");
}

void InstallINCPage::onMcfProgress(MCFCore::Misc::ProgressInfo& info)
{
	uint8 oFlag = info.flag&0xF;
	uint8 sFlag = (info.flag>>4)&0xF;

	if (info.flag != m_uiLastFlag)
	{
		const char* format = "#IF_COMPLEXLABEL";
		const char* label = "#IF_COMPLEXINSTALL";

		if (sFlag == 0)
			label = "#IF_COMPLEXREMOVE";
		else if (sFlag == 1)
			label = "#IF_COMPLEXBACKUP";

		if (oFlag == 1)
			format = "#IF_COMPLEXLABEL_INIT";
			
		m_labInfo->SetLabel(gcString(Managers::GetString(format), Managers::GetString(label)));
		m_uiLastFlag = info.flag;
	}

	if (oFlag == 0)
	{
		gcFrame* par = dynamic_cast<gcFrame*>(GetParent());
		if (par)
			par->setProgress(info.percent);

		m_pbProgress->setProgress(info.percent);
	}

	Update();
}

}
}
}