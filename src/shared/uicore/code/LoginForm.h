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


#ifndef DESURA_LOGINFORM_H
#define DESURA_LOGINFORM_H


#include <wx/gbsizer.h>

#include "Event.h"
#include "Managers.h"
#include "util_thread/BaseThread.h"
#include "wx_controls/gcControls.h"


class LoginThread;
class StripMenuButton;

///////////////////////////////////////////////////////////////////////////////
/// Class LoginForm
///////////////////////////////////////////////////////////////////////////////
class LoginForm : public gcFrame
{
public:
	LoginForm(wxWindow* parent);
	~LoginForm();

	void autoLogin();

	EventV onLoginEvent;
	Event<gcException> onLoginErrorEvent;

	void newAccountLogin(const char* username, const char* cookie);
	void newAccountLoginError(const char* szErrorMessage);

	void showForcedLogoutPromt();

protected:
	friend class LanguageTestDialog;

	gcTextCtrl* m_tbUsername;
	gcTextCtrl* m_tbPassword;
	gcTextCtrl* m_tbPasswordDisp;

	gcCheckBox* m_cbRemPass;
#ifdef WIN32
	gcCheckBox* m_cbProxyOff;
#endif

	gcStaticLine* m_staticline1;

	gcButton* m_butSignin;
	gcButton* m_butCancel;

	gcImageControl* m_imgLogo;
	gcImageControl* m_imgAvatar;

	gcChoice* m_comboProvider;

	gcImageButton *m_butTwitter;
	gcImageButton *m_butSteam;
	gcImageButton *m_butFacebook;
	gcImageButton *m_butGoogle;

	StripMenuButton* m_linkOffline;
	StripMenuButton* m_linkNewAccount;
	StripMenuButton* m_linkLostPassword;




	void doLogin();
	void doLogin(gcString user, gcString pass);

	void onLogin();
	void onLoginError(gcException &e);

	void onStartLogin(std::pair<gcString, gcString> &l);
	Event<std::pair<gcString, gcString>> onStartLoginEvent;

	void processTab(bool forward, int32 id);

	wxRect getWindowsBorderRect() const;
	void setFrameRegion();

	void onButtonClick(wxCommandEvent& event);
	void onTextBoxEnter(wxCommandEvent& event);
	void onMove(wxMoveEvent& event);
	void onChar(wxKeyEvent& event);
	void onClose(wxCloseEvent& event);

	void onFormChar(wxKeyEvent& event);

	void onMouseUp(wxMouseEvent& event);
	void onMouseDown(wxMouseEvent& event);
	void onMouseMove(wxMouseEvent& event);
	void onTextChange(wxCommandEvent& event);
	void onFocus(wxFocusEvent& event);
	void onBlur(wxFocusEvent& event);

	void onLinkClick( wxCommandEvent& event );
#ifdef WIN32
	void onCheckBoxClick( wxCommandEvent& event );
#endif
	void onAltLoginClick( wxCommandEvent& event );

	void onNewAccount();

	void onAltLogin(const char* szProvider);

	void newAccountLoginCB(std::pair<gcString, gcString> &info);
	void newAccountLoginErrorCB(gcString &szErrorMessage);

private:
	Event<std::pair<gcString, gcString>> onNewAccountLoginEvent;
	Event<gcString> onNewAccountLoginErrorEvent;

	bool m_bSavePos;
	LoginThread* m_pLogThread;

	bool m_bAutoLogin;

	std::vector<wxWindow*> m_vTabOrder;

	bool m_bMouseDrag;
	wxPoint m_StartPos;
	wxPoint m_StartDrag;

	wxDialog* m_pNewAccount;

	//Used for language testing
	bool m_bDisabled = false;
};

#endif
