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
#include "ItemToolBarControl.h"
#include "MainApp.h"

#include "Managers.h"
#include "SearchControl.h"
#include "gcJSBinding.h"

enum
{
	LEFT,
	CENTER,
	RIGHT,
};

class ItemButton : public gcButton
{
public:
	ItemButton(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& text = wxString(""), uint32 pos = CENTER) : gcButton(parent, id, text, wxDefaultPosition, wxSize(-1,19))
	{
		m_szImage = ("#playlist_button_normal");
		m_szHoverImage = ("#playlist_button_hover");
		m_szDepressedImage = ("#playlist_button_normal");
		m_szDisabledImage = ("#playlist_button_normal");
		m_szFocusImage = ("#playlist_button_focus");

		m_uiPos = pos;

		m_colHover = wxColor(GetGCThemeManager()->getColor("itemToolBar", "hov-fg"));
		m_colNormal = wxColor(GetGCThemeManager()->getColor("itemToolBar", "fg"));
		m_colSelected =  wxColor(GetGCThemeManager()->getColor("itemToolBar", "selected"));

		refreshImage(true);
		
#ifdef NIX
		//LINUX TODO: Work out best size
		wxFont f = GetFont();
		f.SetPointSize(7);
		SetFont(f);
#endif
	}

	void onActiveToggle(bool state)
	{
		if (state)
		{
			m_colNormal = wxColor(GetGCThemeManager()->getColor("itemToolBar", "fg"));
			m_szImage = "#playlist_button_normal";
		}
		else
		{
			m_colNormal = wxColor(GetGCThemeManager()->getColor("itemToolBar", "na-fg"));
			m_szImage = "#playlist_button_nonactive";
		}

		refreshImage(true);
		invalidatePaint();
	}

	void setSelected(bool bState)
	{
		if (bState == m_bSelected)
			return;

		m_bSelected = bState;

		if (bState)
			m_szImage = "#playlist_button_active";
		else
			m_szImage = "#playlist_button_normal";

		m_szCurImage = "";
		refreshImage(true);
	}

protected:
	virtual void doPaint(wxDC* dc)
	{
		if (!m_imgHandle.getImg() || !m_imgHandle->IsOk())
		{
			dc->SetTextForeground(wxColor(0,0,0));
			dc->Clear();
			return;
		}

		dc->SetFont(GetFont());
		dc->SetTextBackground(GetBackgroundColour());

		if (m_bHovering)
			dc->SetTextForeground(m_colHover);
		else if (m_bSelected)
			dc->SetTextForeground(m_colSelected);
		else
			dc->SetTextForeground(m_colNormal);


		int h = GetSize().GetHeight();
		int w = GetSize().GetWidth();
		int iw = m_imgHandle->GetSize().GetWidth();

		wxBitmap   tmpBmp(w, h);
		wxMemoryDC tmpDC(tmpBmp);

		tmpDC.SetBrush(wxBrush(wxColor(255,0,255)));
		tmpDC.SetPen( wxPen(wxColor(255,0,255),1) );
		tmpDC.DrawRectangle(0,0,w,h);

		wxImage scaled = m_imgHandle->Scale(iw, h);

		wxBitmap left;
		wxBitmap right;
		wxBitmap centerImg = GetGCThemeManager()->getSprite(scaled, "playlist_button", "fill");

		if (m_uiPos == LEFT)
		{
			left = GetGCThemeManager()->getSprite(scaled, "playlist_button", "left");
		}
		else
		{
			left = GetGCThemeManager()->getSprite(scaled, "playlist_button", "fill");
		}

		if (m_uiPos == RIGHT)
		{
			right = GetGCThemeManager()->getSprite(scaled, "playlist_button", "right");
		}
		else
		{
			right = GetGCThemeManager()->getSprite(scaled, "playlist_button", "seperator");
		}


		wxBitmap center(w-(left.GetWidth()+right.GetWidth()),h);

		wxColor c(255,0,255);
		gcImage::tileImg(center, centerImg, &c);

		tmpDC.DrawBitmap(left, 0,0,true);
		tmpDC.DrawBitmap(center, left.GetWidth(),0,true);
		tmpDC.DrawBitmap(right, w-right.GetWidth(),0,true);

		tmpDC.SelectObject(wxNullBitmap);
		dc->DrawBitmap(tmpBmp, 0,0, true);

		if (m_bTransEnabled)
		{
			wxRegion region = wxRegion(tmpBmp, wxColor(255,0,255), 1);
			SetShape(region, this);
		}

		wxString lab = GetLabel();

		long x = (w - dc->GetTextExtent(lab).GetWidth())/2;
		long y = (h - dc->GetTextExtent(lab).GetHeight())/2;

		dc->DrawText(lab, x, y);
	}

private:
	bool m_bSelected = false;
	uint32 m_uiPos;

	wxColor m_colHover;
	wxColor m_colNormal;
	wxColor m_colSelected;
};


ItemToolBarControl::ItemToolBarControl(wxWindow* parent) : BaseToolBarControl(parent)
{
	m_butExpand = new gcImageButton(this, BUTTON_EXPAND, wxDefaultPosition, wxSize( 19,19 ), 0 );
	m_butExpand->setDefaultImage(("#items_expand"));
	m_butExpand->setHoverImage(("#items_expand_hover"));
	m_butExpand->SetToolTip(Managers::GetString(L"#DM_EXPAND"));

	m_butContract = new gcImageButton(this, BUTTON_CONTRACT, wxDefaultPosition, wxSize( 19,19 ), 0 );
	m_butContract->setDefaultImage(("#items_contract"));
	m_butContract->setHoverImage(("#items_contract_hover"));
	m_butContract->SetToolTip(Managers::GetString(L"#DM_CONTRACT"));

#ifdef ENABLE_SEARCH
	m_pSearch = new SearchControl(this);
	m_pSearch->onSearchEvent += delegate(&onSearchEvent);
	m_pSearch->onFullSearchEvent += delegate(&onSearchEvent);
#else
	m_pSearch = nullptr;
#endif

	m_pFGContentSizer = new wxFlexGridSizer( 1, 10, 0, 0 );
	m_pFGContentSizer->AddGrowableRow( 0 );
	m_pFGContentSizer->SetFlexibleDirection( wxBOTH );
	m_pFGContentSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	createButtons();

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 6, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->AddGrowableRow( 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	fgSizer1->Add( m_pFGContentSizer, 0, 0, 4 );
	fgSizer1->Add( 3, 0, 1, 0, 4 );
	fgSizer1->Add( m_butExpand, 0, wxTOP|wxBOTTOM, 4 );
	

#ifdef ENABLE_SEARCH
	fgSizer1->Add( m_butContract, 0, wxTOP|wxBOTTOM, 4 );
	fgSizer1->Add( m_pSearch, 0, wxTOP|wxBOTTOM, 4);
	fgSizer1->Add( m_pSearch->getButton(), 0, wxTOP|wxBOTTOM|wxRIGHT, 4);
#else
	fgSizer1->Add( m_butContract, 0, wxTOP|wxBOTTOM|wxRIGHT, 4 );
#endif

	this->SetSizer( fgSizer1 );
	this->Layout();

	auto userCore = GetUserCore();

	if (userCore && GetUploadMng())
	{
		userCore->getItemsAddedEvent() += guiDelegate(this, &ItemToolBarControl::onItemsAdded);
		userCore->getLoginItemsLoadedEvent() += guiDelegate(this, &ItemToolBarControl::onLoginItemsLoaded);
		GetUploadMng()->getUpdateEvent() += guiDelegate(this, &ItemToolBarControl::onUploadItemsAdded);
	}
}

ItemToolBarControl::~ItemToolBarControl()
{
	auto userCore = GetUserCore();

	if (userCore)
	{
		userCore->getItemsAddedEvent() -= guiDelegate(this, &ItemToolBarControl::onItemsAdded);
		userCore->getLoginItemsLoadedEvent() -= guiDelegate(this, &ItemToolBarControl::onLoginItemsLoaded);
	}

	if (GetUploadMng())
		GetUploadMng()->getUpdateEvent() -= guiDelegate(this, &ItemToolBarControl::onUploadItemsAdded);
}

void ItemToolBarControl::focusSearch()
{
	if (m_pSearch)
		m_pSearch->focusSearch();
}

void ItemToolBarControl::onActiveToggle(bool state)
{
	for (size_t x=0; x<m_vButtonList.size(); x++)
	{
		m_butExpand->setDefaultImage(state?"#items_expand":"#items_expand_nonactive");
		m_butContract->setDefaultImage(state?"#items_contract":"#items_contract_nonactive");

		m_vButtonList[x]->onActiveToggle(state);
	}
}

void ItemToolBarControl::onLoginItemsLoaded()
{
	onUploadItemsAdded();
}

void ItemToolBarControl::onItemsAdded(uint32&)
{
	if (GetUserCore()->isDelayLoading())
		return;

	onUploadItemsAdded();
}

void ItemToolBarControl::onUploadItemsAdded()
{
	createButtons();
	Layout();
	Refresh();
}

void ItemToolBarControl::createButtons()
{
	if (!GetUserCore() || !GetUserCore()->getItemManager())
		return;

	std::vector<gcRefPtr<UserCore::Item::ItemInfoI>> dList;
	GetUserCore()->getItemManager()->getDevList(dList);

	bool dev = (dList.size() > 0) || (GetUploadMng()->getCount() > 0);

	if (!m_vButtonList.empty() && dev == m_bShowDevList)
		return;

	m_bShowDevList = dev;

	m_pFGContentSizer->Clear(true);
	m_vButtonList.clear();


	uint32 fPos = CENTER;
	uint32 dPos = RIGHT;

	if (!dev)
		fPos = RIGHT;

	m_vButtonList.push_back(new ItemButton(this, BUTTON_GAME, Managers::GetString(L"#PLAY_GAMES"), LEFT));
	m_vButtonList.push_back(new ItemButton(this, BUTTON_FAV, Managers::GetString(L"#PLAY_FAVORITES"), fPos));
	
	if (dev)
		m_vButtonList.push_back(new ItemButton(this, BUTTON_DEV, Managers::GetString(L"#PLAY_DEVELOPMENT"), dPos));
	
	for (size_t x=0; x<m_vButtonList.size(); x++)
		m_pFGContentSizer->Add( m_vButtonList[x], 0, wxTOP|wxBOTTOM, 4 );

	gcString strVal = DesuraJSBinding::getCacheValue_s("LastActiveTab", "");

	if (!strVal.empty())
	{
		if (strVal == "game")
			m_vButtonList[0]->setSelected(true);
		else if (strVal == "fav")
			m_vButtonList[1]->setSelected(true);
		else if (dev && strVal == "dev")
			m_vButtonList[2]->setSelected(true);
		else
			m_vButtonList[0]->setSelected(true);
	}
	else
	{
		m_vButtonList[0]->setSelected(true);
	}
}

void ItemToolBarControl::onButtonClick(wxCommandEvent &event)
{
	BaseToolBarControl::onButtonClick(event);

	for (auto b : m_vButtonList)
	{
		auto bSelect = b->GetId() == event.GetId();
		b->setSelected(bSelect);
	}
}