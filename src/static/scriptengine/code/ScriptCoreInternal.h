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


#ifndef DESURA_SCRIPTCOREINTERNAL_H
#define DESURA_SCRIPTCOREINTERNAL_H
#ifdef _WIN32
#pragma once
#endif

#include "v8.h"

class ScriptCoreInternal
{
public:
	void init();
	void del();
	
	void runScript(const char* file, const char* buff, uint32 size);
	void runString(const char* string);

protected:
	void doRunScript(v8::Handle<v8::Script> script);
	gcString reportException(v8::TryCatch* try_catch);

private:
	friend bool IsV8Init();
	static bool s_IsInit;
	static bool s_Disabled;

	static void OnFatalError(const char* location, const char* message);

	v8::Persistent<v8::Context> m_v8Context;
};




#endif //DESURA_SCRIPTCOREINTERNAL_H
