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
#include "gcGrid.h"

#include <wx/tooltip.h>

gcGrid::gcGrid( wxWindow *parent,wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxGrid(parent, id, pos, size, style, name)
{
	m_iLastCol = -1;
	m_iLastRow = -1;

	Bind(wxEVT_GRID_SELECT_CELL, &gcGrid::onSelSelect, this);
	GetGridWindow()->Bind(wxEVT_MOTION, &gcGrid::onMouseMotion, this);
}

gcGrid::~gcGrid()
{
	safe_delete(m_vToolTipGrid);
}

void gcGrid::CreateGrid(int numRows, int numCols, wxGrid::wxGridSelectionModes selmode)
{
	wxGrid::CreateGrid(numRows, numCols, selmode);

	if (numRows>0 && numCols>0)
	{
		safe_delete(m_vToolTipGrid);

		for (int32 x=0; x<numRows*numCols; x++)
		{
			m_vToolTipGrid.push_back(nullptr);
		}
	}
}

void gcGrid::setToolTip(int32 col, int32 row, const wchar_t* text)
{
	if (col < 0 || row < 0 || col > this->GetNumberCols() || row > this->GetNumberRows())
		return;

	uint32 pos = this->GetNumberRows()*col+row;

	delete m_vToolTipGrid[pos];
	m_vToolTipGrid[pos] = new gcWString(text);
}

const wchar_t* gcGrid::getToolTip(int32 col, int32 row)
{
	if (col < 0 || row < 0 || col > this->GetNumberCols() || row > this->GetNumberRows())
		return nullptr;

	uint32 pos = this->GetNumberRows()*col+row;
	return m_vToolTipGrid[pos]?m_vToolTipGrid[pos]->c_str():nullptr;
}

void gcGrid::onSelSelect( wxGridEvent& event )
{ 
	showToolTip(event.GetCol(), event.GetRow());
	event.Skip(); 
}

void gcGrid::onMouseMotion( wxMouseEvent &event)
{
	wxPoint pos = this->CalcUnscrolledPosition( event.GetPosition() );
	int32 row = this->YToRow(pos.y);
	int32 col = this->XToCol(pos.x);

	showToolTip(col, row);
	event.Skip();
}


void gcGrid::showToolTip(int32 col, int32 row)
{
	if (row >= 0 && col >= 0)
	{
		const wchar_t *tool = getToolTip(col, row);

		if (tool)
		{
			wxToolTip *tip = GetGridWindow()->GetToolTip();

			if (!tip)
			{
				tip = new wxToolTip(tool);
#ifdef WIN32
				tip->SetMaxWidth(5000);
#endif
				this->GetGridWindow()->SetToolTip(tip);
			}

			this->GetGridWindow()->SetToolTip(tool);
		}
		else
		{
			this->GetGridWindow()->SetToolTip(nullptr);
		}


	}
}
