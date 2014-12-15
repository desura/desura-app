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

#pragma once

#include "DesuraWnd.h"

// DesuraProgress

enum
{
	MODE_STARTUP,
	MODE_DOWNLOADING,
	MODE_INSTALLING,
};


class DesuraProgress : public Desurium::CDesuraWnd
{
public:
	DesuraProgress();
	virtual ~DesuraProgress();

	bool Create(Desurium::CDesuraWnd *pParent, const Desurium::CRect& rect, UINT nID, DWORD dwStyle = WS_VISIBLE) override;

	void setDone(int done);
	int getDone();

	void setProgress(int prog)
	{
		if (prog < 0)
			prog = 0;

		m_iProg = prog;
		refresh();
	}

	void setMode(int mode)
	{
		m_iMode = mode;
		m_iProg = 0;
		m_iTotal = 0;
		refresh();
	}

	void setTotal(int total)
	{
		if (total < 0)
			total = 0;

		m_iTotal = total;
	}

	void refresh()
	{
		RedrawWindow(0, 0, RDW_INTERNALPAINT|RDW_NOERASE|RDW_UPDATENOW|RDW_INVALIDATE);
	}

protected:

	bool RegisterWindowClass();

	void OnPaint() override;
	bool OnEraseBkgnd() override;
	void OnMouseMove(UINT nFlags, Desurium::CPoint point) override;
	void OnLButtonUp(UINT nFlags, Desurium::CPoint point) override;
	void OnLButtonDown(UINT nFlags, Desurium::CPoint point) override;
	void OnMouseLeave() override;

	Desurium::CRect getCancelRect();

	static LRESULT CALLBACK WinProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


private:
	int m_iTotal;
	int m_iProg;
	int m_iMode;
	bool m_bMouseOver;

	int m_vDone[3];

	Desurium::CBitmap *m_pBackgroundImage;
	Desurium::CBitmap *m_pLoadingBar;

	bool m_bDragging;
	Desurium::CPoint m_StartPoint;

	static DesuraProgress* gs_pDesuraProgress;
};


