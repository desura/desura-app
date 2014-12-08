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

#ifndef DESURA_BZIP2_H
#define DESURA_BZIP2_H
#ifdef _WIN32
#pragma once
#endif

#include "bzlib.h"

namespace UTIL
{
	namespace BZIP
	{

	inline void BZ2CBuff( char* dest, uint32* destLen, char* source, uint32  sourceLen)
	{
		int res =  BZ2_bzBuffToBuffCompress(dest, destLen, source, sourceLen, 9, 0, 30);

		if (res != 0)
			throw gcException(ERR_BZ2CFAIL, res, gcString("Failed to compress, Bzip2 Library error: {0}", res));
	}

	inline void BZ2DBuff( char* dest, uint32* destLen, char* source, uint32  sourceLen)
	{
		int res = BZ2_bzBuffToBuffDecompress(dest, destLen, source, sourceLen, 0, 0);

		if (res != 0)
			throw gcException(ERR_BZ2DFAIL, res, gcString("Failed to decompress, Bzip2 Library error: {0}", res));
	}

	//Make sure you delete the return buff
	inline char* BZ2DBuff(uint32* destLen, char* source, uint32 sourceLen)
	{
		if (*destLen == 0)
			*destLen = sourceLen* 15;

		char* dest = new char[*destLen];

		while (true)
		{
			int res = BZ2_bzBuffToBuffDecompress(dest, destLen, source, sourceLen, 0, 0);

			if (res == 0)
			{
				break;
			}
			if (res == -8)
			{
				delete [] dest;
				*destLen *= 2;
				dest = new char[*destLen];
			}
			else
			{
				throw gcException(ERR_BZ2DFAIL, res, gcString("Failed to decompress, Bzip2 Library error: {0}", res));
			}
		}

		return dest;
	}


	//Make sure you delete the return buff
	inline char* BZ2CBuff(uint32* destLen, char* source, uint32 sourceLen)
	{
		if (!destLen)
			return nullptr;

		if (*destLen == 0)
			*destLen = sourceLen* 15;

		char* dest = new char[*destLen];

		while (true)
		{
			int res = BZ2_bzBuffToBuffCompress(dest, destLen, source, sourceLen, 9, 0, 30);

			if (res == 0)
			{
				break;
			}
			if (res == -8)
			{
				delete [] dest;
				*destLen *= 2;
				dest = new char[*destLen];
			}
			else
			{
				throw gcException(ERR_BZ2DFAIL, res, gcString("Failed to compress, Bzip2 Library error: {0}", res));
			}
		}

		return dest;
	}



}
}

#endif //DESURA_BZIP2_H
