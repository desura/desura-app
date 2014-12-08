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
#include "gcWebFakeBrowser.h"
#include "wx_controls/gcPanel.h"
#include "Managers.h"

gcWebFakeBrowser::gcWebFakeBrowser(wxWindow* parent)
{
	m_pParent = parent;
	m_szNoLoadString = Managers::GetString(L"#BH_NO_LOAD");
}

void gcWebFakeBrowser::destroy()
{
	delete this;
}

#ifdef WIN32
void gcWebFakeBrowser::onPaint()
{
	wxPaintDC dc(m_pParent);

	wxColor bg = m_pParent->GetBackgroundColour();
	wxColor fg = m_pParent->GetForegroundColour();

	dc.SetFont(m_pParent->GetFont());
	dc.SetTextForeground(*wxWHITE);
	dc.SetTextBackground(*wxBLACK);

	wxSize s = m_pParent->GetSize();

	dc.SetBackground(wxBrush(*wxBLACK));
	dc.Clear();

	wxSize te = dc.GetTextExtent(m_szNoLoadString);

	int x = (s.GetWidth()-te.GetWidth())/2;
	int y = (s.GetHeight()-te.GetHeight())/2;

	dc.SetBrush(wxBrush(*wxBLACK));
	dc.DrawText(m_szNoLoadString, x, y);
}
#endif
