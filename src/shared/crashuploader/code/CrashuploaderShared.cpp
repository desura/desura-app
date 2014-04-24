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


#include "Common.h"
#include "Tracer.h"

#ifdef WIN32

void DumpTracerToFile(const std::string &szTracer, std::function<void(const char*, uint32)> &fh)
{
	if (!fh)
		return;

	uint32 nSize = sizeof(TracerHeader_s);

	HANDLE hMappedFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READONLY | SEC_COMMIT, 0, nSize, szTracer.c_str());

	if (!hMappedFile)
		return;

	TracerHeader_s *pHeader = (TracerHeader_s*)MapViewOfFile(hMappedFile, FILE_MAP_READ, 0, 0, nSize);

	if (!pHeader)
	{
		CloseHandle(hMappedFile);
		return;
	}

	uint32 nDataSize = pHeader->segCount * pHeader->segSize;

	UnmapViewOfFile(pHeader);
	CloseHandle(hMappedFile);

	hMappedFile = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READONLY | SEC_COMMIT, 0, nSize + nDataSize, szTracer.c_str());

	if (!hMappedFile)
		return;

	pHeader = (TracerHeader_s*)MapViewOfFile(hMappedFile, FILE_MAP_READ, 0, 0, nSize + nDataSize);

	if (!pHeader)
	{
		CloseHandle(hMappedFile);
		return;
	}


	try
	{
		fh("[", 1);
		auto bFirst = true;

		for (uint32 x = 0; x < nDataSize; x += pHeader->segSize)
		{
			const char* szStr = &pHeader->data + x;
			uint32 nLen = Safe::strlen(szStr, pHeader->segSize);

			if (nLen > 0)
			{
				if (!bFirst)
					fh(",\n\t", 3);
				else
					fh("\n\t", 2);

				fh(szStr, nLen);
				bFirst = false;
			}
		}

		fh("\n]", 2);
	}
	catch (...)
	{
	}

	UnmapViewOfFile(pHeader);
	CloseHandle(hMappedFile);
}

#else
void DumpTracerToFile(const std::string &szTracer, std::function<void(const char*, uint32)> &fh)
{

}
#endif
