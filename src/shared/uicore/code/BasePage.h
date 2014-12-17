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

#ifndef DESURA_INSTALLBASEPAGE_H
#define DESURA_INSTALLBASEPAGE_H
#ifdef _WIN32
#pragma once
#endif

#include "wx_controls/gcControls.h"

#define TYPE_FOLDER true
#define TYPE_FILE false

namespace UserCore
{
	class ItemManagerI;

	namespace Item
	{
		class ItemInfoI;
	}
}


//! Base class that represents a basic page on a multi page form
//!
class BasePage : public gcPanel
{
public:
	BasePage(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, gcRefPtr<UserCore::ItemManagerI> pItemManager = gcRefPtr<UserCore::ItemManagerI>());
	BasePage(wxWindow* parent, gcRefPtr<UserCore::ItemManagerI> pItemManager);


	//! Sets the item info id
	//!
	//! @param id Item id
	//!
	virtual void setInfo(DesuraId id) final;
	virtual void setInfo(DesuraId id, gcRefPtr<UserCore::Item::ItemInfoI> pItemInfo);

	//! Sets the mcf build number
	//! Maybe this shouldnt be here?
	//!
	//! @param branch Mcf branch
	//! @param build Mcf Build
	//!
	virtual void setMCFInfo(MCFBranch branch, MCFBuild build);

	//! Sets the current page task to paused or not
	//!
	//! @param state Set paused or not
	//!
	virtual void setPaused(bool state=true){;}

	//! Gets the item info form the id number
	//!
	//! @return ItemInfo if found or nullptr if not
	//!
	gcRefPtr<UserCore::Item::ItemInfoI> getItemInfo();

	//! Gets the item id number
	//!
	//! @return Item id
	//!
	DesuraId getItemId(){return m_iInternId;}


	//! Overide this to start the task needed when this page is active
	//!
	virtual void run()=0;

	//! Overide this to clean up once the page becomes inactive
	//!
	virtual void dispose()=0;

protected:
	DesuraId m_iInternId;

	uint32 m_uiMCFBuild = 0;
	uint32 m_uiMCFBranch = 0;

	gcRefPtr<UserCore::ItemManagerI> m_pItemManager;
	gcRefPtr<UserCore::Item::ItemInfoI> m_pItemInfo;
};


#endif //DESURA_INSTALLBASEPAGE_H
