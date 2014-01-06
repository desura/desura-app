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


#ifndef DESURA_SCRIPTCORE_H
#define DESURA_SCRIPTCORE_H
#ifdef _WIN32
#pragma once
#endif

#include "ScriptCoreI.h"
#include "v8.h"

class ScriptTask;
class ScriptCoreInternal;

class ScriptCore : public ScriptCoreI
{
public:
	ScriptCore(ScriptTaskRunnerI* runner);
	~ScriptCore();

	virtual void setItem(ScriptCoreItemI* item);
	virtual void delItem();

	virtual void executeScript(const char* file);
	virtual void executeString(const char* string);

	virtual void destory()
	{
		delete this;
	}

protected:
	void doRunScript(v8::Handle<v8::Script> script);

	friend class InitV8;
	void init();

	void runTask(ScriptTask* task);

private:
	
	uint32 m_uiItemId;

	ScriptCoreInternal* m_pInternal;
	ScriptTaskRunnerI* m_pTaskRunner;
};




#endif //DESURA_SCRIPTCORE_H
