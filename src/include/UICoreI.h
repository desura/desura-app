/*
Copyright (C) Mark 2010 Chandler (Desura Net Pty Ltd)
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

#ifndef DESURA_UICORE_H
#define DESURA_UICORE_H
#ifdef _WIN32
#pragma once
#endif


typedef void (*RestartFP)(const char*);
typedef void (*DumpLevelFP)(unsigned char);

#ifdef WIN32
typedef void (*DumpSettingsFP)(const wchar_t*, bool);
#else
typedef void (*DumpSettingsFP)(const char*, bool);
#endif

class TracerI;

//! Interface to setup and start the ui part of desura
class UICoreI
{
public:
#ifdef WIN32
	//! Initilizes wx widgets
	//!
	//! @param hInst HInstance of loading executable
	//! @param cmdShow Default show state
	//! @param argc Number of arguments
	//! @param argv Argument array
	//! @return true if successful
	//!
	virtual bool initWxWidgets(HINSTANCE hInst, int cmdShow, int argc, char** argv)=0;

	//! On idle callback
	//!
	virtual BOOL onIdle()=0;

	//! Translates a windows msg and handles it if it can
	//!
	//! @param msg Windows msg
	//! @return true if handle
	//!
	virtual bool preTranslateMessage(MSG *msg)=0;

	//! Get handle of main window
	//!
	virtual HWND getHWND()=0;
#else
	//! Initilizes wx widgets. Warning, will block until UI shutsdown
	//!
	//! @param argc Number of arguments
	//! @param argv Argument array
	//! @return true if successful
	//!
	virtual bool initWxWidgets(int argc, char** argv)=0;

	//! Check to see if any other instances are running
	//!
	//! @return true if no other instances are running
	//!
	virtual bool singleInstantCheck(const char* args = nullptr)=0;

	//! Kills single instance check
	//!
	virtual void destroySingleInstanceCheck()=0;

	//! Disables single instance check and stops it been deleted in this process
	//!
	virtual void disableSingleInstanceLock()=0;
#endif

	//! Closes the main window causing the end of the application
	//!
	virtual void closeMainWindow()=0;

	//! Sets the bootloader version
	//!
	virtual void setDesuraVersion(const char* version)=0;

	//! Stops the UI and grabs the return code
	//!
	//! @param result variable to save return code into
	//!
	virtual void exitApp(int* result)=0;

	//! Sets the restart function callback
	//!
	virtual void setRestartFunction(RestartFP rfp)=0;

	//! Sets the crash dump settings callback
	//!
	virtual void setCrashDumpSettings(DumpSettingsFP dsfp)=0;

	//! Sets the crash dump level callback
	//!
	virtual void setCrashDumpLevel(DumpLevelFP dlfp)=0;

	//! Runs all unit tests
	virtual int runUnitTests(int argc, char** argv) = 0;

	//! Set the tracer
	//!
	virtual void setTracer(TracerI *pTracer) = 0;
};

typedef UICoreI* (*UICoreFP)();

#endif //DESURA_UICORE_H
