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
#include "TBI_WindowMenu.h"

#include "wx_controls/gcCustomMenu.h"
#include "Managers.h"

#include "wx/frame.h"

wxMenu* TBIWindowMenu::createMenu(uint32 &lastMenuId)
{
	m_WindowMap.clear();

	gcMenu* menu = new gcMenu();

	std::vector<wxFrame*> vList;
	GetWindowManager().getWindowList(vList);

	int32 count = 0;

	for (size_t x=0; x<vList.size(); x++)
	{
		wxFrame *temp = vList[x];
		if (!temp || !temp->IsShown())
			continue;

		wxMenuItem* window = new gcMenuItem(menu, lastMenuId, temp->GetTitle());

		m_WindowMap[lastMenuId] = temp->GetId();
		menu->Append(window);

		wxBitmap bm = wxBitmap(temp->GetIcon());

		if (bm.IsOk())
		{
			wxImage icon = wxImage(bm.ConvertToImage());
			window->SetBitmap(wxBitmap(icon.Scale(16,16)));
		}

		count++;
		lastMenuId++;
	}

	if (vList.size() == 0 || count==0)
	{
		wxMenuItem* m_miNoWindwos = new gcMenuItem(menu, lastMenuId, Managers::GetString(L"#TB_NOWINDOWS"));
		menu->Append(m_miNoWindwos);
		m_miNoWindwos->Enable(false);

		lastMenuId++;
	}

	return menu;
}

void TBIWindowMenu::onMenuSelect(wxCommandEvent& event)
{
	if (m_WindowMap.find(event.GetId()) == m_WindowMap.end())
		return;

	std::vector<wxFrame*> vList;
	GetWindowManager().getWindowList(vList);

	for (size_t x=0; x<vList.size(); x++)
	{
		wxFrame *temp = vList[x];
		if (temp && temp->GetId()== m_WindowMap[event.GetId()])
		{
			temp->Raise();
			break;
		}
	}
}

const wchar_t* TBIWindowMenu::getMenuName()
{
	return Managers::GetString(L"#TB_WINDOWS");
}
