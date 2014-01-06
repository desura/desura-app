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
#include "BasePage.h"

#include "MainApp.h"
#include "usercore/ItemInfoI.h"
#include "usercore/ItemManagerI.h"

BasePage::BasePage( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : gcPanel( parent, id, pos, size, style )
{
	m_uiMCFBuild = 0;
	m_uiMCFBranch = 0;
}

void BasePage::setInfo(DesuraId id)
{
	m_iInternId = id;

	UserCore::Item::ItemInfoI *item = GetUserCore()->getItemManager()->findItemInfo(id);

	if (item && item->getIcon() && UTIL::FS::isValidFile(UTIL::FS::PathWithFile(item->getIcon())))
	{
		gcFrame *frame = dynamic_cast<gcFrame*>(GetParent());
		
		if (frame)
			frame->setIcon(item->getIcon());
	}
}

void BasePage::setMCFInfo(MCFBranch branch, MCFBuild build)
{
	m_uiMCFBuild = build;
	m_uiMCFBranch = branch;
}

UserCore::Item::ItemInfoI* BasePage::getItemInfo()
{
	return GetUserCore()->getItemManager()->findItemInfo(m_iInternId);
}
