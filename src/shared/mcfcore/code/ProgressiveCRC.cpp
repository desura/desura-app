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
#include "ProgressiveCRC.h"

namespace MCFCore
{
namespace Misc
{

ProgressiveCRC::ProgressiveCRC(uint32 blockSize)
{
	m_uiBlockSize = blockSize;
	m_uiDone = 0;

	m_uiCurCRC = 0xFFFFFFFF;
}

ProgressiveCRC::~ProgressiveCRC()
{
}


void ProgressiveCRC::addData(const unsigned char* buff, uint32 size)
{
	for (uint32 x=0; x<size; x++)
	{
		if (m_uiDone >= m_uiBlockSize)
			finishCRC();

		m_uiCurCRC = UTIL::MISC::CRC32(buff[x], m_uiCurCRC);
		m_uiDone++;
	}
}

std::vector<uint32>& ProgressiveCRC::getVector()
{
	if (m_uiDone > 0)
		finishCRC();

	return m_vCRCList;
}

void ProgressiveCRC::finishCRC()
{
	m_vCRCList.push_back((~m_uiCurCRC));
	m_uiDone = 0;
	m_uiCurCRC = 0xFFFFFFFF;
}

}
}
