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
#include "ScriptCoreInternal.h"


std::mutex ScriptCoreInternal::s_InitLock;
bool ScriptCoreInternal::s_IsInit = false;
bool ScriptCoreInternal::s_Disabled = false;
v8::Platform* ScriptCoreInternal::m_platform = nullptr;
v8::Isolate* ScriptCoreInternal::m_isolate = nullptr;

bool IsV8Init()
{
	std::lock_guard<std::mutex> guard(ScriptCoreInternal::s_InitLock);
	return ScriptCoreInternal::s_IsInit && !ScriptCoreInternal::s_Disabled;
}



v8::FunctionCallback JSDebug;
v8::FunctionCallback JSWarning;
void MessageCallback(v8::Handle<v8::Message> message, v8::Handle<v8::Value> data);

extern v8::ExtensionConfiguration* RegisterJSBindings();


const char* ToCString(const v8::String::Utf8Value& value)
{
	return *value ? *value : "<string conversion failed>";
}

void ScriptCoreInternal::OnFatalError(const char* location, const char* message)
{
	WarningS("Fatal error in v8. Disabling runtime. \n{0}: {1}\n", location, message);
	s_Disabled = false;
}


class ShellArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
public:
	virtual void* Allocate( size_t length ) {
		void* data = AllocateUninitialized( length );
		return data == NULL ? data : memset( data, 0, length );
	}
	virtual void* AllocateUninitialized( size_t length ) { return malloc( length ); }
	virtual void Free( void* data, size_t ) { free( data ); }
};


void ScriptCoreInternal::init()
{
	if (s_Disabled)
		return;

	{
		std::lock_guard<std::mutex> guard(s_InitLock);
		if (!s_IsInit)
		{
			s_IsInit = true;
			v8::V8::InitializeICU();
			v8::V8::Initialize();

			ShellArrayBufferAllocator array_buffer_allocator;
			v8::V8::SetArrayBufferAllocator( &array_buffer_allocator );
			m_isolate = v8::Isolate::New();

			v8::V8::AddMessageListener(&MessageCallback);
			v8::V8::SetCaptureStackTraceForUncaughtExceptions(true);
			v8::V8::SetFatalErrorHandler(&ScriptCoreInternal::OnFatalError);
		}
	}


	// Create a template for the global object.
	v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New( m_isolate );

	global->Set( v8::String::NewFromUtf8( m_isolate, "Warning" ), v8::FunctionTemplate::New( m_isolate, JSWarning ) );
	global->Set( v8::String::NewFromUtf8( m_isolate, "Debug" ), v8::FunctionTemplate::New( m_isolate, JSDebug ) );

	v8::Handle<v8::Context> context = v8::Context::New( m_isolate, RegisterJSBindings(), global );
	m_v8Context = context;
}

void ScriptCoreInternal::del()
{
	if (s_Disabled)
		return;

	v8::V8::Dispose();
	v8::V8::ShutdownPlatform();
}

void ScriptCoreInternal::runString(const char* string)
{
	if (s_Disabled)
		throw gcException(ERR_V8, "V8 Internal error");

	if (!string)
		throw gcException(ERR_INVALID, "String is null");

	v8::HandleScope handle_scope( m_isolate );
	v8::Context::Scope context_scope( m_v8Context );
	v8::TryCatch try_catch;
	v8::Handle<v8::String> source = v8::String::NewFromUtf8( m_isolate, string );
	v8::ScriptOrigin origin( v8::String::NewFromUtf8( m_isolate, "StringExe" ) );
	v8::Handle<v8::Script> script = v8::Script::Compile( source, &origin );

	if (script.IsEmpty())
	{
		throw gcException(ERR_V8, gcString("V8 Err: {0}", reportException(&try_catch)));
	}
	else
	{
		v8::Handle<v8::Value> result = script->Run();

		if (result.IsEmpty())
			throw gcException(ERR_V8, gcString("V8 Err: {0}", reportException(&try_catch)));
	}
}

void ScriptCoreInternal::runScript(const char* file, const char* buff, uint32 size)
{
	if (s_Disabled)
		throw gcException(ERR_V8, "V8 Internal error");

	v8::HandleScope handle_scope( m_isolate );
	v8::Context::Scope context_scope( m_v8Context );
	v8::TryCatch try_catch;
	v8::Handle<v8::String> source = v8::String::NewFromUtf8( m_isolate, buff, v8::String::kNormalString, size );
	v8::ScriptOrigin origin( v8::String::NewFromUtf8( m_isolate, file ) );
	v8::Handle<v8::Script> script = v8::Script::Compile( source, &origin );

	if (script.IsEmpty())
		throw gcException(ERR_INVALID, "Failed to parse script file");
	else
		doRunScript(script);
}

void ScriptCoreInternal::doRunScript(v8::Handle<v8::Script> script)
{
	v8::TryCatch trycatch;

	// Run the script to get the result.
	v8::Handle<v8::Value> result = script->Run();

	if (result.IsEmpty())
	{
		v8::Handle<v8::Value> exception = trycatch.Exception();
		v8::String::Utf8Value exception_str(exception);
		throw gcException(ERR_INVALID, gcString("v8 had exception: {0}", *exception_str));
	}
}

gcString ScriptCoreInternal::reportException(v8::TryCatch* try_catch)
{
	gcString out;

	v8::HandleScope handle_scope( m_isolate );
	v8::String::Utf8Value exception(try_catch->Exception());

	const char* exception_string = ToCString(exception);
	v8::Handle<v8::Message> message = try_catch->Message();

	if (message.IsEmpty())
	{
		// V8 didn't provide any extra information about this error; just
		// print the exception.
		out += gcString("{0}\n", exception_string);
	}
	else
	{
		// Print (filename):(line number): (message).
		v8::String::Utf8Value filename(message->GetScriptResourceName());
		const char* filename_string = ToCString(filename);

		int linenum = message->GetLineNumber();
		out += gcString("{0}:{1} {2}\n", filename_string, linenum, exception_string);


		// Print line of source code.
		v8::String::Utf8Value sourceline(message->GetSourceLine());
		const char* sourceline_string = ToCString(sourceline);

		out += gcString("{0}\n", sourceline_string);

		// Print wavy underline (GetUnderline is deprecated).
		int start = message->GetStartColumn();
		for (int i = 0; i < start; i++)
		{
			out += " ";
		}

		int end = message->GetEndColumn();
		for (int i = start; i < end; i++)
		{
			out += "^";
		}

		out += "\n";
		v8::String::Utf8Value stack_trace(try_catch->StackTrace());

		if (stack_trace.length() > 0)
		{
			const char* stack_trace_string = ToCString(stack_trace);
			out += gcString("{0}\n", stack_trace_string);
		}
	}

	return out;
}
