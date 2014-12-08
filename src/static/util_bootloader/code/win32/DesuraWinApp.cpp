/*
Desura is the leading indie game distribution platform
Copyright (C) 2013 Mark Chandler (Desura Net Pty Ltd)

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

#include "DesuraWinApp.h"
#include "DesuraWnd.h"
#include <windows.h>

#include <cassert>

using namespace Desurium;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	CDesuraWnd::SetInstanceHandle(hInstance);

	CDesuraWinApp& app = CDesuraWinApp::GetApp();

	app.Init(hInstance, nCmdShow, lpCmdLine);

	app.InitInstance();
	return app.ExitInstance();
}


CDesuraWinApp* CDesuraWinApp::gs_pWinApp = nullptr;


CDesuraWinApp::CDesuraWinApp()
	: m_hInstance(nullptr)
	, m_nCmdShow(0)
	, m_lpCmdLine(nullptr)
{
	gs_pWinApp = this;
}

CDesuraWinApp::~CDesuraWinApp()
{
	if (gs_pWinApp == this)
		gs_pWinApp = nullptr;
}

void CDesuraWinApp::Init(HINSTANCE hInstance, int nCmdShow, const char* lpCmdLine)
{
	m_hInstance = hInstance;
	m_nCmdShow = nCmdShow;
	m_lpCmdLine = lpCmdLine;
}

CDesuraWinApp& CDesuraWinApp::GetApp()
{
	if (!gs_pWinApp)
	{
		assert(false);
		abort();
	}

	return *gs_pWinApp;
}
