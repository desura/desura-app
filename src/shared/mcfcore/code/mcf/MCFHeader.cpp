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
#include "MCFHeader.h"

namespace MCFCore
{


MCFHeader::MCFHeader() : UMcfHeader()
{
}

MCFHeader::MCFHeader(MCFHeaderI* head) : UMcfHeader(head)
{
}

MCFHeader::MCFHeader(const char* string) : UMcfHeader()
{
	strToHeader(string);
}

void MCFHeader::saveToFile(UTIL::FS::FileHandle& hFile)
{
	char* str = headerToStr();
	AutoDelete<char> ad(str);

	hFile.seek(0);
	hFile.write((const char*)str, getSize());
}

void MCFHeader::readFromFile(UTIL::FS::FileHandle& hFile)
{
	gcBuff buff(getSize());

	hFile.read(buff.c_ptr(), getSize());
	strToHeader(buff.c_ptr());

	if (!isValid())
		throw gcException((ERROR_ID)ERR_INVALID, "The file is not a MCF file");
}



}
