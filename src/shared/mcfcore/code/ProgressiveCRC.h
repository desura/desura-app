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

#ifndef DESURA_PROGRESSIVECRC_H
#define DESURA_PROGRESSIVECRC_H
#ifdef _WIN32
#pragma once
#endif

namespace MCFCore
{
namespace Misc
{

//! Allows calculation of crc values as data becomes avaliable (i.e. while compressing the files)
class ProgressiveCRC
{
public:
	//! Constuctor
	//!
	//! @param blockSize Size of blocks to do crc on
	//!
	ProgressiveCRC(uint32 blockSize);
	~ProgressiveCRC();

	//! Adds data to calc the crc's on
	//!
	//! @param buff Data buffer
	//! @param size Data size
	//!
	void addData(const unsigned char* buff, uint32 size);

	//! Gets the vector of crc's
	//!
	//! @return CRC vector
	//!
	std::vector<uint32>& getVector();

protected:
	//! Finishes the current crc generation and starts the next
	//!
	void finishCRC();

private:
	uint32 m_uiBlockSize;
	uint32 m_uiDone;

	unsigned long m_uiCurCRC;

	std::vector<uint32> m_vCRCList;
};


}
}

#endif //DESURA_PROGRESSIVECRC_H
