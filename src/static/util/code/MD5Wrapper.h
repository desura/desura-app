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

#ifndef DESURA_MD5WRAPPER_H
#define DESURA_MD5WRAPPER_H
#ifdef _WIN32
#pragma once
#endif

#include <string>

namespace md5wrapper
{
	std::string getHashFromString(std::string text);

	std::string getHashFromFile(std::string filename);

	std::string getHashFromBuffer(const unsigned char *buffer, uint32 size);

	void getHashFromBuffer(const unsigned char *buffer, uint32 size, char unsigned out[16]);

#ifdef WIN32
	std::string getHashFromFile(HANDLE file, uint64 size);
#else
	std::string getHashFromFile(FILE* file, uint64 size);
#endif
}

#endif //DESURA_MD5WRAPPER_H


