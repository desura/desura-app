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

#ifndef DESURA_TRACER_H
#define DESURA_TRACER_H

#include "Common.h"
#include "util_thread/BaseThread.h"
#include <atomic>
#include <list>

#pragma pack(push)
#pragma pack(1)

typedef struct
{
	uint32 pid;
	uint32 lock;
	uint16 segCount;
	uint16 segSize;
	char data;
} TracerHeader_s;

#pragma pack(pop)


#ifdef WIN32

class TracerStorage : public TracerI
{
public:
	TracerStorage(const wchar_t* szSharedMemName);
	~TracerStorage();

	void trace(const std::string &strTrace, std::map<std::string, std::string> *mpArgs) override;

	const wchar_t* getSharedMemName();

protected:
	uint32 getTotalSize() const;

	std::string formatTrace(const std::string &strTrace, std::map<std::string, std::string> *mpArgs);
	std::string cleanUpString(const std::string &string);

private:
	//this number of segments should cause perfect roll around
	const uint16 m_nNumSegments = 4096;
	const uint16 m_nSegmentSize = 512;

	volatile uint32* m_nCurLock = nullptr;

	HANDLE m_hMappedFile = INVALID_HANDLE_VALUE;
	char* m_szMappedMemory = nullptr;

	TracerHeader_s* m_pHeader = nullptr;
	const wchar_t* m_szSharedMemName = nullptr;
};

#else

class TracerStorage : public TracerI
{
public:
	TracerStorage(const wchar_t* szSharedMemName)
	{
	}

	void trace(const std::string &strTrace, std::map<std::string, std::string> *mpArgs) override
	{
	}
};

#endif

extern TracerStorage g_Tracer;

#endif