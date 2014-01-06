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

#ifndef DESURA_SCRIPTCOREI_H
#define DESURA_SCRIPTCOREI_H
#ifdef _WIN32
#pragma once
#endif

#define SCRIPT_CORE "SCRIPT_CORE_001"
#define SCRIPT_CORE_SETUP "SCRIPT_CORE_SETUP_001"


namespace ChromiumDLL
{
	class JavaScriptExtenderI;
}

class ScriptCoreItemI
{
public:
	virtual void destroy()=0;
};

class ScriptTaskI
{
public:
	virtual void doTask()=0;
	virtual void destory()=0;
};

//! This is used to dispatch all scripting tasks on one thread. 
//! Only one thread and the same thread should process these tasks!!!
//!
//!
class ScriptTaskRunnerI
{
public:
	virtual void queTask(ScriptTaskI* task)=0;

	//! Last task to clean every thing up
	virtual void setLastTask(ScriptTaskI* task)=0;
};

class ScriptCoreSetupI
{
public:
	//! must be called before any scriptcoreI is made.
	//! will auto delete extender once done
	//!
	virtual bool addItemExtender(ChromiumDLL::JavaScriptExtenderI* extender)=0;

	virtual void useInternalTaskRunner()=0;

	virtual void setTaskRunner(ScriptTaskRunnerI* runner)=0;

	virtual void cleanUp()=0;
};

class ScriptCoreI
{
public:
	//! 
	virtual void setItem(ScriptCoreItemI* item)=0;
	virtual void delItem()=0;

	virtual void executeScript(const char* file)=0;
	virtual void executeString(const char* string)=0;

	virtual void destory()=0;
};


#endif //DESURA_SCRIPTCOREI_H
