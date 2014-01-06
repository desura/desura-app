/*
Desura is the leading indie game distribution platform
Copyright (C) 2013 Mark Chandler (Lindenlab)

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


#ifndef DESURA_ALTLOGINFORM_H
#define DESURA_ALTLOGINFORM_H

#include "wx_controls/gcDialog.h"
#include "gcWebControl.h"

class AltLoginDialog : public gcDialog
{
public:
	AltLoginDialog(wxWindow* parent, const char* szProvider, const char* szApiUrl);

private:
	gcWebControl *m_pBrowser;
};


#endif