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
#include "bzip2.h"


typedef int  (__stdcall * BZ2_bzCompressInitFN)(bz_stream*, int, int, int);
typedef int  (__stdcall * BZ2_bzCompressFN)(bz_stream*, int);
typedef int  (__stdcall * BZ2_bzCompressEndFN)(bz_stream*);

HINSTANCE g_hBZ2 = nullptr;


BZ2_bzCompressInitFN pBZ2_bzCompressInit = nullptr;
BZ2_bzCompressFN pBZ2_bzCompress = nullptr;
BZ2_bzCompressEndFN pBZ2_bzCompressEnd = nullptr;

bool initFactory()
{
	HINSTANCE g_hBZ2 = LoadLibraryA("libbz2.dll");

    if (!g_hBZ2)
    {
        printf("Error loading libbz2.dll\n");
		return false;
    }

	pBZ2_bzCompressInit = (BZ2_bzCompressInitFN)GetProcAddress(g_hBZ2, "BZ2_bzCompressInit");

    if (!pBZ2_bzCompressInit)
    {
        printf("Error loading BZ2_bzCompressInit\n");
        return false;
    }

	pBZ2_bzCompress = (BZ2_bzCompressFN)GetProcAddress(g_hBZ2, "BZ2_bzCompress");

    if (!pBZ2_bzCompress)
    {
        printf("Error loading BZ2_bzCompress\n");
        return false;
    }

	pBZ2_bzCompressEnd = (BZ2_bzCompressEndFN)GetProcAddress(g_hBZ2, "BZ2_bzCompressEnd");

    if (!pBZ2_bzCompressEnd)
    {
        printf("Error loading BZ2_bzCompressEnd\n");
        return false;
    }

	return true;
}





void bz2CompressFile(const char* src, const char* dest)
{
	if (!g_hBZ2 && !initFactory())
		return;

	UTIL::FS::FileHandle srcFH(src, UTIL::FS::FILE_READ);
	UTIL::FS::FileHandle destFH(dest, UTIL::FS::FILE_WRITE);

	uint64 tot = 0;
	uint64 fileSize = UTIL::FS::getFileSize(UTIL::FS::PathWithFile(src));

	bz_stream* bzs = new bz_stream;
	bzs->bzalloc = nullptr;
	bzs->bzfree = nullptr;
	bzs->opaque = nullptr;

	int32 res = pBZ2_bzCompressInit(bzs, 9, 0, 0);

	if (res != BZ_OK)
	    throw gcException(ERR_BZ2DFAIL);

	size_t buffSize = 512*1024;
	char *inBuff = new char[buffSize];
	char *outBuff = new char[buffSize];

	bool endFile = false;
	

	do
	{
		uint32 readSize = buffSize;
		if (fileSize - tot < buffSize)
		{
			readSize = fileSize - tot;
			endFile = true;
		}

		if (readSize == 0)
			break;

		bzs->next_in = inBuff;
		bzs->avail_in = readSize;

		srcFH.read(inBuff, readSize);
		tot += readSize;
		
		int32 res = BZ_OK;

		do
		{
			bzs->next_out = outBuff;
			bzs->avail_out = buffSize;

			if (endFile)
				res = pBZ2_bzCompress(bzs, BZ_FINISH);
			else
				res = pBZ2_bzCompress(bzs, BZ_RUN);

			uint32 done = buffSize - bzs->avail_out;

			if (done > 0)
				destFH.write(outBuff, done);
		}
		while (endFile && res != BZ_STREAM_END || bzs->avail_in > 0);
	}
	while (res != BZ_STREAM_END);

	pBZ2_bzCompressEnd(bzs);
	safe_delete(bzs);
}




