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
#include "ToolInstallForm.h"
#include "IPCPipeServer.h"

#include "tool_pipe/IPCToolMain.h"

#include <branding/branding.h>

#define STR_INSTALLING Managers::GetString(L"#TIF_INSTALLING")
#define STR_NOTINSTALLING Managers::GetString(L"#TIF_WAITINFO")

ToolInstallForm* g_pToolInstallForm = nullptr;

static VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	if (uMsg == WM_TIMER && g_pToolInstallForm)
		g_pToolInstallForm->timerNotify();
}

void PipeIsActive()
{
	if (g_pToolInstallForm)
		g_pToolInstallForm->pipeIsActive();
}

ToolInstallForm::ToolInstallForm(wxWindow* parent, const char* key) : gcFrame(parent, wxID_ANY, PRODUCT_NAME_CATW(L" Tool Install Helper"), wxDefaultPosition, wxSize(354,82), wxCAPTION|wxCLOSE_BOX|wxSYSTEM_MENU|wxTAB_TRAVERSAL|wxMINIMIZE_BOX)
{
	m_szKey = key;
	g_pToolInstallForm = this;
	m_bInstallInProgress = false;
	m_bIPCConnected = false;

	Bind(wxEVT_CLOSE_WINDOW, &ToolInstallForm::onClose, this);

	startServer();
	startTimer();

	Show(false);
}

ToolInstallForm::~ToolInstallForm()
{
	stopServer();
	g_pToolInstallForm = nullptr;
}

void ToolInstallForm::startTimer()
{
	m_uiTimerId = ::SetTimer((HWND)GetHWND(),  GetId(), 30000, TimerProc);
}

void ToolInstallForm::stopTimer()
{
	::KillTimer((HWND)GetHWND(), m_uiTimerId);
}

void ToolInstallForm::timerNotify()
{
	stopTimer();
	GetParent()->Close();
}

void ToolInstallForm::startServer()
{
	gcString pipe("DesuraToolHelper-{0}", m_szKey);

	m_pIPCServer = new IPC::PipeServer(pipe.c_str(), 1, true);
	m_pIPCServer->onDisconnectEvent += delegate(this, &ToolInstallForm::onDissconnect);

	IPCToolMain::SetupLogging();
	m_pIPCServer->start();
}

void ToolInstallForm::stopServer()
{
	m_pIPCServer->stop();
	safe_delete(m_pIPCServer);
}

void ToolInstallForm::onDissconnect(uint32& id)
{
	//not much we can do
	exit(-1);
}

void ToolInstallForm::onConnect()
{
	pipeIsActive();
}

void ToolInstallForm::pipeIsActive()
{
	if (m_bIPCConnected)
		return;

	m_bIPCConnected = true;
	stopTimer();
}

void ToolInstallForm::onClose(wxCloseEvent& event)
{
	stopTimer();
}
