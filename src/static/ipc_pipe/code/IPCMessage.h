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

#ifndef DESURA_IPCMESSAGE_H
#define DESURA_IPCMESSAGE_H
#ifdef _WIN32
#pragma once
#endif

#pragma pack(1)

#ifdef NIX
	#define PACK __attribute__((packed))
#else
	#define PACK
#endif

typedef struct
{
	uint32 tsize;
	uint32 id;
	uint32 type;
	uint32 size;
	uint8 part;		//if message is split this is the part
	uint8 totparts; //total number of parts
	uint64 serial;  //part message serial number
	char data;
} PACK IPCMessage;

#define IPCMessageSIZE 26

typedef struct
{
	uint32 functionHash;
	uint32 size;
	uint32 id;
	uint32 numP;
	char data;
} PACK IPCFunctionCall;

#define IPCFunctionCallSIZE 16

typedef struct
{
	uint32 hash;
	uint32 id;
	uint32 lock;
} PACK IPCCreateClass;

#define IPCCreateClassSIZE 12

typedef struct
{
	uint32 id;
	uint32 lock;
	uint32 size;
	char data;
} PACK IPCCreateClassRet;

#define IPCCreateClassRetSIZE 12

typedef struct
{
	uint32 type;
	uint32 size;
	char data;
} PACK IPCParameter;

#define IPCParameterSIZE 8


typedef struct
{
	uint32 eventHash;
	uint32 size;
	char data;
} PACK IPCEventTrigger;

#define IPCEventTriggerSIZE 8



#pragma pack()




inline uint32 sizeofStruct(IPCParameter* par)
{
	if (!par)
		return 0;

	return IPCParameterSIZE + par->size;
}

inline uint32 sizeofStruct(IPCCreateClassRet* ccr)
{
	if (!ccr)
		return 0;

	return IPCCreateClassRetSIZE + ccr->size;
}

inline uint32 sizeofStruct(IPCCreateClass* cc)
{
	if (!cc)
		return 0;

	return IPCCreateClassSIZE;
}

inline uint32 sizeofStruct(IPCFunctionCall* fc)
{
	if (!fc)
		return 0;

	return IPCFunctionCallSIZE + fc->size;
}

inline uint32 sizeofStruct(IPCMessage* m)
{
	if (!m)
		return 0;

	return IPCMessageSIZE + m->size;
}

inline uint32 sizeofStruct(IPCEventTrigger* m)
{
	if (!m)
		return 0;

	return IPCEventTriggerSIZE + m->size;
}




enum MESSAGE_TYPE
{
	MT_NULL,
	//manager messages
	MT_CREATECLASS,
	MT_CREATECLASSRETURN,
	MT_KILLCLASS,
	MT_STOP,

	//class messages
	MT_FUNCTIONCALL,
	MT_FUNCTIONCALL_ASYNC,
	MT_FUNCTIONRETURN,
	MT_EVENTTRIGGER,
	MT_KILL,
	MT_KILL_COMPLETE,
};

#endif //DESURA_IPCMESSAGE_H

