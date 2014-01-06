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
#include "MenuSeperator.h"


MenuSeperator::MenuSeperator(wxWindow *parent, const char* image, const wxSize &size) : gcPanel(parent, wxID_ANY, wxDefaultPosition, size), BaseMenuButton((gcPanel*)this)
{
	m_szImg = image;

	m_imgBG = new gcImageControl(this, wxID_ANY, wxPoint(0,0), size);
	m_imgBG->setImage(image);
	m_imgBG->setTrans(false);
}

MenuSeperator::~MenuSeperator()
{
	safe_delete(m_imgBG);
}

void MenuSeperator::setImages(const char* right, const char* left)
{
	m_szSImg_right = right;
	m_szSImg_left = left;
}

void MenuSeperator::onSelected()
{
	if (m_szSImg_right.size() == 0 || m_szSImg_left.size() == 0)
		return;

	if (isSelected())
	{
		if (isRight())
			m_imgBG->setImage(m_szSImg_right.c_str());
		else
			m_imgBG->setImage(m_szSImg_left.c_str());
	}
	else
	{
		m_imgBG->setImage(m_szImg.c_str());
	}
}