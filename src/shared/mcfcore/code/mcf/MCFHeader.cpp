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
#include "MCFHeader.h"

namespace MCFCore
{


MCFHeader::MCFHeader() : UMcfHeader()
{
}

MCFHeader::MCFHeader(MCFHeaderI* head) : UMcfHeader(head)
{
}

MCFHeader::MCFHeader(const uint8* string) : UMcfHeader()
{
	strToHeader(string);
}

void MCFHeader::saveToFile(UTIL::FS::FileHandle& hFile)
{
	const uint8* str = headerToStr();

	try
	{
	    hFile.seek(0);
		hFile.write((const char*)str, getSize());
		safe_delete(str);
	}
	catch (gcException &)
	{
		safe_delete(str);
		throw;
	}
}

void MCFHeader::readFromFile(UTIL::FS::FileHandle& hFile)
{
	uint8* str = new uint8[getSize()];

	try
	{
		hFile.read((char*)str, getSize());
	}
	catch (gcException &)
	{
		safe_delete(str);
		throw;
	}

	strToHeader(str);
	safe_delete(str);

	if (!isValid())
		throw gcException((ERROR_ID)ERR_INVALID, "The file is not a MCF file");
}



}