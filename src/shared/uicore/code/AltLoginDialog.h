/*
Copyright (C) 2013 Mark Chandler (Lindenlab)
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
