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
#include "XMLSaveAndCompress.h"

#define BUFFSIZE 10024

XMLSaveAndCompress::XMLSaveAndCompress(UTIL::FS::FileHandle *fh, bool compress) : m_BZ2Worker(UTIL::MISC::BZ2_COMPRESS), m_fhFile(fh)
{
	m_bCompress = compress;
	m_szReadBuf = nullptr;

	if (compress)
		m_szReadBuf = new char[BUFFSIZE+1];

	m_uiLastWritePos = 0;
	m_uiTotalSize = 0;
	m_uiRawSize = 0;

	m_szWriteBuf = new char[BUFFSIZE+1];
}

XMLSaveAndCompress::~XMLSaveAndCompress()
{
	safe_delete(m_szReadBuf);
	safe_delete(m_szWriteBuf);
}

void XMLSaveAndCompress::finish()
{
	write(m_szWriteBuf, m_uiLastWritePos, true);
}

void XMLSaveAndCompress::save(const char* str, size_t size)
{
	if (!str || size == 0)
		return;

	if (m_uiLastWritePos + size < BUFFSIZE)
	{
		memcpy(m_szWriteBuf+m_uiLastWritePos, str, size);
		m_uiLastWritePos += size;
	}
	else
	{
		write(m_szWriteBuf, m_uiLastWritePos, false);
		m_uiLastWritePos = 0;

		if (size > BUFFSIZE)
		{
			write(const_cast<char*>(str), size, false);
		}
		else
		{
			memcpy(m_szWriteBuf, str, size);
			m_uiLastWritePos = size;
		}
	}
}

uint64 XMLSaveAndCompress::getTotalSize()
{
	return m_uiTotalSize;
}

void XMLSaveAndCompress::write(char* buff, size_t size, bool end)
{
	m_uiRawSize += size;

	if (m_bCompress)
	{
		if ((buff && size > 0) || end)
			m_BZ2Worker.write(buff, size, end);

		m_BZ2Worker.doWork();

		size_t readSize = m_BZ2Worker.getReadSize();

		if (readSize > BUFFSIZE || end)
		{
			size_t readAmmount = BUFFSIZE;

			do
			{
				readAmmount = BUFFSIZE;
				m_BZ2Worker.read(m_szReadBuf, readAmmount);
				m_fhFile->write(m_szReadBuf, readAmmount);

				m_uiTotalSize += readAmmount;
			}
			while (end && readAmmount > 0);
		}
	}
	else
	{
		m_uiTotalSize += size;

		if (size > 0 && buff)
			m_fhFile->write(buff, size);
	}
}
