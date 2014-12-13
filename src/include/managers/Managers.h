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

#ifndef DESURA_INCLUDE_MANAGERS_H
#define DESURA_INCLUDE_MANAGERS_H
#ifdef _WIN32
#pragma once
#endif

#include "Color.h"

class wxFrame;
class wxWindow;
class CVar;
class ConCommand;

#define COLOR_ONE 1
#define COLOR_TWO 2

class SpriteRectI : public gcRefBase
{
public:
	virtual int32 getX()=0;
	virtual int32 getY()=0;
	virtual int32 getW()=0;
	virtual int32 getH()=0;

protected:
	virtual ~SpriteRectI(){}
};

class ThemeStubI
{
public:
	virtual const char* getName()=0;
	virtual const char* getPrintName()=0;
	virtual const char* getAuthor()=0;
	virtual const char* getPreview()=0;
	virtual const char* getVersion()=0;
	virtual bool parseFile(const char* file)=0;

	virtual ~ThemeStubI(){}
};

class ThemeManagerI
{
public:
	virtual const char* getWebPage(const char* id)=0;
	virtual const char* getImage(const char* id)=0;

	virtual const char* getThemeFolder()=0;

	virtual Color getColor(const char* name, const char* id)=0;
	virtual gcRefPtr<SpriteRectI> getSpriteRect(const char* id, const char* rectId) = 0;

	virtual void loadFromFolder(const char* folder)=0;
	virtual void loadTheme(const char* name)=0;

	//make sure to destroy them
	virtual void getThemeStubList(std::vector<ThemeStubI*> &vList)=0;

protected:
	virtual ~ThemeManagerI(){};
};

class LanguageManagerI
{
public:
	virtual const char* getString(const char* name)=0;
	virtual const wchar_t* getString(const wchar_t* name)=0;

	virtual bool loadFromFile(const char* file)=0;
	virtual void unloadAll()=0;

protected:
	virtual ~LanguageManagerI(){};
};

class WindowManagerI
{
public:
	virtual void registerWindow(wxFrame* win)=0;
	virtual void unRegisterWindow(wxFrame* win)=0;
	virtual void getWindowList(std::vector<wxFrame*> &vList)=0;

protected:
	virtual ~WindowManagerI(){};
};

class CVarManagerI
{
public:
	virtual void saveAll()=0;
	virtual void cleanUserCvars()=0;

	virtual void loadUser(uint32 userId)=0;

	virtual gcRefPtr<CVar> findCVar(const char* name) = 0;
	virtual void getCVarList(std::vector<gcRefPtr<CVar>> &vList) = 0;

protected:
	virtual ~CVarManagerI(){};
};

class CVarRegTargetI
{
public:
	virtual bool RegCVar(const gcRefPtr<CVar> &var) = 0;
	virtual void UnRegCVar(const gcRefPtr<CVar> &var) = 0;
};

class CCommandManagerI
{
public:
	virtual gcRefPtr<ConCommand> findCCommand(const char* name) = 0;
	virtual void getConCommandList(std::vector<gcRefPtr<ConCommand>> &vList) = 0;

protected:
	virtual ~CCommandManagerI(){};
};

ThemeManagerI &     GetThemeManager();
LanguageManagerI &  GetLanguageManager();
WindowManagerI &    GetWindowManager();
CVarManagerI*       GetCVarManager();
CCommandManagerI*   GetCCommandManager();

namespace Managers
{
	void LoadTheme(wxWindow* win, const char* name);
	const char* GetString( const char* str );
	const wchar_t* GetString( const wchar_t* str );
}

#define WINDOW_REG()                         \
{                                            \
	GetWindowManager().registerWindow(this); \
}

#define WINDOW_UNREG() virtual bool Destroy()  \
{                                              \
	GetWindowManager().unRegisterWindow(this); \
	return wxFrame::Destroy();                 \
}


#endif //DESURA_MANAGERS_H
