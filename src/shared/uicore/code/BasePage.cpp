/*
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)
Copyright (C) 2014 Bad Juju Games, Inc.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA.

Contact us at legal@badjuju.com.

*/

#include "Common.h"
#include "BasePage.h"

#include "MainApp.h"
#include "usercore/ItemInfoI.h"
#include "usercore/ItemManagerI.h"

BasePage::BasePage(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, gcRefPtr<UserCore::ItemManagerI> pItemManager)
	: gcPanel( parent, id, pos, size, style )
	, m_pItemManager(pItemManager)
{
	if (!m_pItemManager && GetUserCore())
		m_pItemManager = GetUserCore()->getItemManager();
}

BasePage::BasePage(wxWindow* parent, gcRefPtr<UserCore::ItemManagerI> pItemManager)
	: BasePage(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, pItemManager)
{
}

void BasePage::setInfo(DesuraId id)
{
	setInfo(id, m_pItemManager->findItemInfo(id));
}

void BasePage::setInfo(DesuraId id, gcRefPtr<UserCore::Item::ItemInfoI> pItemInfo)
{
	m_iInternId = id;
	m_pItemInfo = pItemInfo;

	if (pItemInfo && pItemInfo->getIcon() && UTIL::FS::isValidFile(UTIL::FS::PathWithFile(pItemInfo->getIcon())))
	{
		gcFrame *frame = dynamic_cast<gcFrame*>(GetParent());

		if (frame)
			frame->setIcon(pItemInfo->getIcon());
	}
}

void BasePage::setMCFInfo(MCFBranch branch, MCFBuild build)
{
	m_uiMCFBuild = build;
	m_uiMCFBranch = branch;
}

gcRefPtr<UserCore::Item::ItemInfoI> BasePage::getItemInfo()
{
	if (m_pItemInfo)
		return m_pItemInfo;

	return m_pItemManager->findItemInfo(m_iInternId);
}
