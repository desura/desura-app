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
