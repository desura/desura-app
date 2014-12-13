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

#ifndef DESURA_CDKPROGRESS_H
#define DESURA_CDKPROGRESS_H
#ifdef _WIN32
#pragma once
#endif

#include "BasePage.h"
#include "wx_controls/gcControls.h"

#include "usercore/MCFThreadI.h"
#include "usercore/CDKeyManagerI.h"

class CDKProgressProxy;

class CDKProgress : public BasePage
{
public:
	CDKProgress(wxWindow* parent, bool launch);
	~CDKProgress();

	void dispose();
	void run();

protected:
	friend class LanguageTestDialog;
	friend class CDKProgressProxy;

	Event<gcString> onCompleteEvent;
	Event<gcException> onErrorEvent;

	void onComplete(gcString& cdKey);
	void onError(gcException& e);

	void onCDKeyComplete(DesuraId id, gcString &cdKey);
	void onCDKeyError(DesuraId id, gcException& e);

private:
	wxStaticText* m_labInfo;
	bool m_bLaunch;

	gcSpinningBar* m_pbProgress;
	gcButton* m_butClose;

	gcRefPtr<CDKProgressProxy> m_ProgressProxy;

	bool m_bOutstandingRequest = false;
};


#endif //DESURA_CDKPROGRESS_H
