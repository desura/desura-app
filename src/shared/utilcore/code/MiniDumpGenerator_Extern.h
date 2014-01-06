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

#ifndef DESURA_MINIDUMPGENERATOR_EXTERN_H
#define DESURA_MINIDUMPGENERATOR_EXTERN_H
#ifdef _WIN32
#pragma once
#endif

#include "DbgHelp.h"

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
									CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
									CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
									CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
									);


class MiniDumpGenerator_Extern
{
public:
	MiniDumpGenerator_Extern(const char* exeName, const char* savePath);
	~MiniDumpGenerator_Extern();


	//! Generates crash dump
	//!
	//! @param returns 0 on failure, 1 on success and -1 if exe not found
	int generate();

	const char* getSavedFile();

	virtual BOOL callbackFilter(const PMINIDUMP_CALLBACK_INPUT pInput, PMINIDUMP_CALLBACK_OUTPUT pOutput);

protected:
	virtual void generateFileName();

	bool saveDump(MINIDUMPWRITEDUMP pDump, HANDLE process, DWORD processId, HANDLE hFile);

	HANDLE findProcess(DWORD &processId);

	bool isDataSectionNeeded(const wchar_t* pModuleName);

private:
	gcString m_szExeName;
	gcString m_szRootPath;
	gcString m_szSavePath;

	gcString m_szDumpFile;
};


#endif //DESURA_MINIDUMPGENERATOR_EXTERN_H
