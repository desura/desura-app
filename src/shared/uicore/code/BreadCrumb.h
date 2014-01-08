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

#ifndef DESURA_BREADCRUMB_H
#define DESURA_BREADCRUMB_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcControls.h"

class BaseMenuButton;
class MenuFiller;

class BreadCrump : public gcPanel
{
public:
	BreadCrump(wxPanel *parent, int id = wxID_ANY );
	~BreadCrump();

	int addItem(const char* text, uint32 id);
	void clear();

protected:
	wxFlexGridSizer* m_sizerMenu;
	MenuFiller *m_pFiller;
	MenuFiller *m_pStart;

private:
	std::vector<BaseMenuButton*> m_vButtons;
};


#endif //DESURA_BreadCrump_H
