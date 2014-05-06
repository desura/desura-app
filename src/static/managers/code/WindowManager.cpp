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
#include "WindowManager.h"
#include <wx/wx.h>

namespace Managers
{

void LoadTheme(wxWindow* win, const char* name)
{
	Color bg = GetThemeManager().getColor(name, "bg");
	Color fg = GetThemeManager().getColor(name, "fg");

	win->SetForegroundColour(wxColor(fg));
	win->SetBackgroundColour(wxColor(bg));
}

}


WindowItem::WindowItem(wxFrame* frame) 
	: BaseItem()
{
	m_uiHash = frame->GetId();
	m_pFrame = frame;
}

WindowManager::WindowManager() 
	: BaseManager()
{

}

void WindowManager::registerWindow(wxFrame* win)
{
	if (win)
		addItem(gcRefPtr<WindowItem>::create(win));
}

void WindowManager::unRegisterWindow(wxFrame* win)
{
	if (win)
		removeItem(win->GetId());
}

void WindowManager::getWindowList(std::vector<wxFrame*> &vList)
{
	for (uint32 x=0; x<getCount(); x++)
		vList.push_back(getItem(x)->m_pFrame);

	std::sort(vList.begin(), vList.end(), [](wxFrame* lhs, wxFrame* rhs){
		return wcscmp(lhs->GetTitle().c_str(), rhs->GetTitle().c_str())>0;
	});
}

