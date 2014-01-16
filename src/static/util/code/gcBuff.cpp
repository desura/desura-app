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
#include "util/gcBuff.h"

gcBuff::gcBuff(uint32 size)
{
	m_cBuff = new char[size];
	m_uiSize = size;
}	

gcBuff::gcBuff(const char* src, uint32 size)
{
	m_cBuff = new char[size];
	m_uiSize = size;

	cpy(src, size);
}

gcBuff::gcBuff(const gcBuff &buff)
{
	uint32 size = buff.size();

	if (size > 0)
	{
		m_cBuff = new char[size];
		m_uiSize = size;
		cpy(buff.m_cBuff, size);
	}
}

gcBuff::gcBuff(gcBuff &&buff)
{
	m_cBuff = buff.m_cBuff;
	m_uiSize = buff.m_uiSize;

	buff.m_cBuff = nullptr;
	buff.m_uiSize = 0;
}

gcBuff::gcBuff(gcBuff *buff)
{
	if (!buff)
		return;

	uint32 size = buff->size();

 	if (size > 0)
	{
		m_cBuff = new char[size];
		m_uiSize = size;
		cpy(buff->c_ptr(), size);
	}
}

gcBuff::~gcBuff()
{
	safe_delete(m_cBuff);
}

char gcBuff::operator[] (uint32 i) const
{
	if (i > m_uiSize)
		return '\0';

	return m_cBuff[i];
}

gcBuff::operator char*()
{
	return m_cBuff;
}

void gcBuff::cpy(const char* src, uint32 size)
{
	if (!src)
		return;

	if (size > m_uiSize)
		size = m_uiSize;

	memcpy(m_cBuff, src, size);
}

gcBuff& gcBuff::operator=(const gcBuff &buff)
{
	if (&buff == this)
		return *this;

	if (buff.size() > m_uiSize)
	{
		safe_delete(m_cBuff);
		m_cBuff = new char[buff.size()];
	}

	m_uiSize = buff.size();
	cpy(buff.m_cBuff, m_uiSize);

	return *this;
}

gcBuff& gcBuff::operator=(gcBuff &&buff)
{
	if (&buff == this)
		return *this;

	safe_delete(m_cBuff);

	m_cBuff = buff.m_cBuff;
	m_uiSize = buff.m_uiSize;

	buff.m_cBuff = nullptr;
	buff.m_uiSize = 0;

	return *this;
}

