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

#ifndef DESURA_GCMANAGERS_H
#define DESURA_GCMANAGERS_H
#ifdef _WIN32
#pragma once
#endif

#include <wx/wx.h>
#include "wx_controls/gcImageHandle.h"
#include "managers/Managers.h"


class gcThemeManagerI : public ThemeManagerI
{
public:
	//! Gets image handle. Alpha refers to if the image has transperency
	//!
	virtual gcImageHandle getImageHandle(const char* path)=0;

	//! Gets a sprite from an image
	//!
	virtual wxBitmap getSprite(wxImage& img, const char* spriteId, const char* spriteName)=0;

protected:
	virtual void newImgHandle(uint32 hash)=0;
	virtual void desposeImgHandle(uint32 hash)=0;

	friend class gcImageHandle;
};


gcThemeManagerI* GetGCThemeManager();


extern wxWindow*		GetMainWindow();
extern void				ExitApp();


#endif //DESURA_GCMANAGERS_H
