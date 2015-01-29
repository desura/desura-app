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
#include <iostream>
#include "v8.h"
#include "ScriptCoreInternal.h"


void MessageCallback(v8::Handle<v8::Message> message, v8::Handle<v8::Value> data)
{
	v8::String::Utf8Value exception_str( message->Get() );
	const char* ex = *exception_str;

	WarningS("v8 Message Callback: {0}\n", ex);
}

template <typename F>
void JSPrint(const v8::Handle<v8::Value>& arg, F messageFunct)
{
	if (arg->IsObject())
	{
		//convert the args[i] type to normal char* string
		messageFunct("Obj: ");

		v8::Local< v8::String > str( arg->ToObject()->ObjectProtoToString() );

		char* buffer = new char[ str->Length() + 1 ];
		str->WriteUtf8( buffer );
		messageFunct( buffer );
		delete [] buffer;

		for (int32 x=0; x<arg->ToObject()->InternalFieldCount(); x++)
			JSPrint(arg->ToObject()->Get(x), messageFunct);
	}
	else if (arg->IsString())
	{
		//convert the args[i] type to normal char* string
		messageFunct("Str: ");

		char* buffer = new char[ arg->ToString()->Length() + 1 ];
		arg->ToString()->WriteUtf8( buffer );
		messageFunct( buffer );
		delete[] buffer;
	}
	else if (arg->IsInt32() || arg->IsUint32())
	{
		//convert the args[i] type to normal char* string
		v8::Local<v8::String> str( arg->ToInteger()->ToString() );

		messageFunct("Int: ");

		char* buffer = new char[ str->Length() + 1 ];
		str->WriteUtf8( buffer );
		messageFunct( buffer );
		delete[] buffer;
	}
	else if (arg->IsNumber())
	{
		//convert the args[i] type to normal char* string
		v8::Local<v8::String> str( arg->ToNumber()->ToString() );
		messageFunct("Num: ");

		char* buffer = new char[ str->Length() + 1 ];
		str->WriteUtf8( buffer );
		messageFunct( buffer );
		delete[] buffer;
	}
	else if (arg->IsNull())
	{
		messageFunct("[nullptr]");
	}
	else if (arg->IsUndefined())
	{
		messageFunct("[Undefined]");
	}
	else
	{
		//convert the args[i] type to normal char* string
		v8::Local<v8::String> str( arg->ToDetailString() );

		char* buffer = new char[ str->Length() + 1 ];
		str->WriteUtf8( buffer );
		messageFunct( buffer );
		delete[] buffer;
	}
}

template <typename F>
void JSPrintArgs( const v8::FunctionCallbackInfo<v8::Value>& args, F messageFunct )
{
	bool first = true;
	for (int i = 0; i < args.Length(); i++)
	{
		v8::HandleScope handle_scope( ScriptCoreInternal::getIsolate() );
		if (first)
		{
			first = false;
		}
		else
		{
			fprintf(stdout, " ");
		}

		JSPrint(args[i], messageFunct);
	}

	messageFunct("\n");
}

void JSDebug( const v8::FunctionCallbackInfo<v8::Value>& args )
{
	std::string out;
	JSPrintArgs( args, [&out]( const char* msg ){
			out += msg;
	});

	Msg(out.c_str());
}

void JSWarning( const v8::FunctionCallbackInfo<v8::Value>& args )
{
	std::string out;

	JSPrintArgs( args, [&out]( const char* msg ){
			out += msg;
	});

	WarningS(out.c_str());
}
