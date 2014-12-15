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

#ifndef DESURA_TABHANDLER_H
#define DESURA_TABHANDLER_H
#ifdef _WIN32
#pragma once
#endif

#include <vector>
#include "util/gcTime.h"

class TabPanelI;

class TabHandler
{
public:
	TabHandler(TabPanelI* parent);
	~TabHandler();

	void clear();
	void addPanel(TabPanelI* pan);

	void onKeyDown(TabPanelI* pan, wxKeyEvent& event);

	void next();
	void prev();

	void front(bool down = false);
	void end(bool down = false);

	void first();
	void last();

protected:
	void updatePanel();

private:
	gcTime m_tNextUpdate;


	std::vector<TabPanelI*> m_vPanelList;

	TabPanelI* m_pParent;
	size_t m_uiCurPanel = 0;
};

#endif //DESURA_TABHANDLER_H
