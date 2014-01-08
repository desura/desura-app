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
#include <vector>
#include "TabHandler.h"
#include "TabPanel.h"

#include "boost\date_time.hpp"

TabHandler::TabHandler(TabPanelI* parent)
	: m_pParent(parent)
{
}

TabHandler::~TabHandler()
{
}

void TabHandler::clear()
{
	m_vPanelList.clear();
	m_uiCurPanel = 0;
}

void TabHandler::addPanel(TabPanelI* pan)
{
	if (pan)
	{
		m_vPanelList.push_back(pan);
		pan->setTabHandler(this);
	}
}


void TabHandler::onKeyDown(TabPanelI* pan, wxKeyEvent& event)
{
	auto now = gcTime();

	if (now < m_tNextUpdate)
		return;

	m_tNextUpdate = now + std::chrono::milliseconds(100);

	for (size_t x=0; x<m_vPanelList.size(); x++)
	{
		if ( m_vPanelList[x] == pan)
		{
			m_uiCurPanel = x;
			break;
		}
	}


	switch (event.GetKeyCode())
	{
	case WXK_TAB:
		{
			if (event.m_shiftDown)
				prev();
			else
				next();

			break;
		}

	case WXK_LEFT:
		pan->onLeft();
		break;

	case WXK_RIGHT:
		pan->onRight();
		break;

	case WXK_UP:
		prev();
		break;

	case WXK_DOWN:
		next();
		break;
	};
}

void TabHandler::next()
{
	if (m_vPanelList.size()-1 == m_uiCurPanel)
	{
		m_vPanelList[m_uiCurPanel]->getTabHandler()->first();
	}
	else
	{
		m_uiCurPanel++;
		updatePanel();
	}
}

void TabHandler::prev()
{
	if (m_uiCurPanel == 0)
	{
		if (m_pParent->getTabHandler())
		{
			m_pParent->getTabHandler()->last();
		}
		else if (m_vPanelList.size() > 1)
		{
			m_uiCurPanel = m_vPanelList.size()-1;
			updatePanel();
		}
	}
	else
	{
		m_uiCurPanel--;
		updatePanel();
	}
}

void TabHandler::front(bool down)
{
	if (!down && m_pParent->getTabHandler())
	{
		m_pParent->getTabHandler()->front(false);
	}
	else
	{
		if (m_vPanelList[ 0 ]->getTabHandler())
			m_vPanelList[ 0 ]->getTabHandler()->front(true);
		else
			first();
	}
}

void TabHandler::end(bool down)
{
	if (!down && m_pParent->getTabHandler())
	{
		m_pParent->getTabHandler()->end(false);
	}
	else
	{
		if (m_vPanelList[ m_vPanelList.size()-1 ]->getTabHandler())
			m_vPanelList[ m_vPanelList.size()-1 ]->getTabHandler()->end(true);
		else
			last();
	}
}	

void TabHandler::first()
{
	m_uiCurPanel = 0;
	updatePanel();
}

void TabHandler::last()
{
	m_uiCurPanel = m_vPanelList.size()-1;
	updatePanel();
}

void TabHandler::updatePanel()
{
	if (m_uiCurPanel < m_vPanelList.size())
		m_vPanelList[m_uiCurPanel]->setFocus();
}