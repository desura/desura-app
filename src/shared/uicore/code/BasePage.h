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
	BasePage(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, UserCore::ItemManagerI* pItemManager = nullptr);
	BasePage(wxWindow* parent, UserCore::ItemManagerI* pItemManager);


	//! Sets the item info id
	//!
	//! @param id Item id
	//!
	virtual void setInfo(DesuraId id) final;
	virtual void setInfo(DesuraId id, UserCore::Item::ItemInfoI* pItemInfo);

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
	UserCore::Item::ItemInfoI* getItemInfo();

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

	UserCore::ItemManagerI* m_pItemManager = nullptr;
	UserCore::Item::ItemInfoI* m_pItemInfo = nullptr;
};


#endif //DESURA_INSTALLBASEPAGE_H
