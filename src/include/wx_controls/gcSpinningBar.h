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

#ifndef DESURA_GCSPINNINGBAR_H
#define DESURA_GCSPINNINGBAR_H
#ifdef _WIN32
#pragma once
#endif

#include "gcPanel.h"
#include "gcImageHandle.h"


#ifdef WIN32
	typedef int32 TimerId;
#else
	typedef uint TimerId;
#endif

//! Spinning progress bar for when you dont know how long something is going to take
class gcSpinningBar : public gcPanel
{
public:
	gcSpinningBar(wxWindow* parent, wxWindowID id = wxID_ANY,const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	~gcSpinningBar();

	//! Callback from timer events
	void notify();

protected:
	void onEraseBg(wxEraseEvent& event){}
	void onPaint(wxPaintEvent& event); 

	void doPaint(wxDC* dc);
	void tileImg(wxBitmap src);

	EventV onNeedRedrawEvent;
	void onNeedRedraw();

private:
	gcImageHandle m_imgProg;
	wxBitmap m_Buffer;

	uint32 m_uiOffset;
	TimerId m_tId;
	
	volatile bool m_bNotifyRedraw;
};

#endif //DESURA_GCSPINNINGBAR_H
