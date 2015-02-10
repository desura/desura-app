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

#include "Common.h"
#include <branding/scriptcore_version.h>

#include "LogBones.cpp"
#include "v8.h"

#include "ScriptCoreI.h"
#include "ScriptTaskThread.h"

gcString g_szSCVersion("{0}.{1}.{2}.{3}", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILDNO, VERSION_EXTEND);


ScriptCoreI* NewScriptCore(ScriptTaskRunnerI* taskRunner);
bool AddItemExtender(ChromiumDLL::JavaScriptExtenderI* extender);
bool IsV8Init();

class ShellArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
public:
	virtual void* Allocate( size_t length ) {
		void* data = AllocateUninitialized( length );
		return data == NULL ? data : memset( data, 0, length );
	}
	virtual void* AllocateUninitialized( size_t length ) { return malloc( length ); }
	virtual void Free( void* data, size_t ) { free( data ); }
};

void hackV8()
{
	static bool once = false;

	if ( !once )
	{
		once = true;
		v8::V8::InitializeICU();
		v8::V8::Initialize();

		ShellArrayBufferAllocator array_buffer_allocator;
		v8::V8::SetArrayBufferAllocator( &array_buffer_allocator );
	}
}


class ScriptCoreSetup : public ScriptCoreSetupI, public ScriptTaskI
{
public:
	ScriptCoreSetup()
	{
		hackV8();
		m_bCleanUp = false;
		m_pThread = nullptr;
	}

	~ScriptCoreSetup()
	{
		safe_delete(m_pThread);
	}

	virtual bool addItemExtender(ChromiumDLL::JavaScriptExtenderI* extender)
	{
		return AddItemExtender(extender);
	}

	virtual void useInternalTaskRunner()
	{
		if (m_pThread)
			return;

		doTask();

		m_pThread = new ScriptTaskThread();
		m_pThread->start();

		setTaskRunner(m_pThread);
	}

	virtual void setTaskRunner(ScriptTaskRunnerI* runner)
	{
		m_pTaskRunner = runner;

		if (m_pTaskRunner)
			m_pTaskRunner->setLastTask(this);
	}

	virtual void cleanUp()
	{
		m_bCleanUp = true;

		if (m_pThread)
			m_pThread->stop();
	}

	virtual void* newScriptCore()
	{
		return static_cast<void*>(NewScriptCore(m_pTaskRunner));
	}

	virtual void doTask()
	{
		if (m_bCleanUp && IsV8Init())
			v8::V8::Dispose();

		m_bCleanUp = false;
	}

	virtual void destory()
	{
	}

	bool m_bCleanUp;
	ScriptTaskRunnerI* m_pTaskRunner;
	ScriptTaskThread* m_pThread;
};

ScriptCoreSetup g_Setup;

namespace SCore
{
	CEXPORT void* FactoryBuilder(const char* name)
	{
		if (strcmp(name, SCRIPT_CORE)==0)
		{
			return g_Setup.newScriptCore();
		}
		else if (strcmp(name, SCRIPT_CORE_SETUP)==0)
		{
			return (ScriptCoreSetupI*)&g_Setup;
		}

		return nullptr;
	}
}

extern "C"
{

CEXPORT void* FactoryBuilderScriptCore(const char* name)
{
	return SCore::FactoryBuilder(name);
}

CEXPORT const char* GetMCFCoreVersion()
{
	return g_szSCVersion.c_str();
}

CEXPORT void RegDLLCB_SCRIPT(LogCallback* cb)
{
	g_pLogCallback = cb;

	if (g_pLogCallback)
		g_pLogCallback->Message(MT_MSG, "ScriptCore Logging Started.\n");
}

}





