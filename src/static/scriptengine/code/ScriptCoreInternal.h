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


#ifndef DESURA_SCRIPTCOREINTERNAL_H
#define DESURA_SCRIPTCOREINTERNAL_H
#ifdef _WIN32
#pragma once
#endif

#include "v8.h"
#include "libplatform/libplatform.h"


class ScriptCoreInternal
{
public:
	void init();
	void del();

	void runScript(const char* file, const char* buff, uint32 size);
	void runString(const char* string);

	static v8::Isolate* getIsolate()	{ return m_isolate; }

protected:
	void doRunScript(v8::Handle<v8::Script> script);
	gcString reportException(v8::TryCatch* try_catch);

private:
	friend bool IsV8Init();

	// Not sure why this isn't a singleton instead
	static std::mutex s_InitLock;
	static bool s_IsInit;
	static bool s_Disabled;
	static v8::Platform* m_platform;
	static v8::Isolate* m_isolate;

	static void OnFatalError(const char* location, const char* message);

	v8::Handle<v8::Context> m_v8Context;
};




#endif //DESURA_SCRIPTCOREINTERNAL_H
