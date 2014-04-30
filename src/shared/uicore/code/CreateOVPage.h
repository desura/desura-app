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

#ifndef DESURA_CREATEOVPAGE_H
#define DESURA_CREATEOVPAGE_H
#ifdef _WIN32
#pragma once
#endif

#include "BasePage.h"
#include "MainApp.h"
#include "wx_controls/gcControls.h"
#include "wx_controls/gcGrid.h"

class CreateMCFOverview : public BasePage 
{
public:
	CreateMCFOverview(wxWindow* parent);
	~CreateMCFOverview();

	void dispose() override;
	void setInfo(DesuraId itemId, UserCore::Item::ItemInfoI* pItemInfo, const char* path);

	Event<ut> onUploadTriggerEvent;

	void run() override {}

protected:
	
	gcStaticText* m_labInfo;
	gcStaticText* m_labName;
	gcStaticText* m_labSize;
	gcStaticText* m_labPath;

	gcGrid* m_gInfo;
	
	gcButton* m_butUpload;
	gcButton* m_butClose;
	gcButton* m_butViewFile;
	

	void onButtonClick( wxCommandEvent& event );
	void onMouseDown( wxMouseEvent& event );
private:
	UserCore::Item::ItemInfoI* m_pItem;

	gcString m_szPath;
	gcString m_szFolderPath;
};



#endif //DESURA_CREATEOVPAGE_H

