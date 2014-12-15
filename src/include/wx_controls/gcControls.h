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

#ifndef DESURA_GCCONTROLS_H
#define DESURA_GCCONTROLS_H
#ifdef _WIN32
#pragma once
#endif

#include "gcButton.h"
#include "gcCheckBox.h"
#include "gcChoice.h"
#include "gcCombo.h"
#include "gcDialog.h"
#include "gcFrame.h"
#include "gcHyperlinkCtrl.h"
#include "gcImage.h"
#include "gcImageButton.h"
#include "gcImageControl.h"
#include "gcImgButtonCount.h"
#include "gcMessageBox.h"
#include "gcPanel.h"
#include "gcProgressBar.h"
#include "gcRadioButton.h"
#include "gcScrolledWindow.h"
#include "gcSpinningBar.h"
#include "gcStaticLine.h"
#include "gcStaticText.h"
#include "gcTaskBar.h"
#include "gcTextCtrl.h"
#include "gcToggleButton.h"
#include "gcUpProgBar.h"


#ifdef NIX
#include "util/UtilLinux.h"
#endif

inline bool gcLaunchDefaultBrowser(const gcString& url)
{
#ifdef WIN32
	return wxLaunchDefaultBrowser(url);
#else
	return UTIL::LIN::launchProcessXDG(url.c_str());
#endif
}

#endif
