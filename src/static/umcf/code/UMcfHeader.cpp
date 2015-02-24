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


#pragma warning( disable : 4627 )

#include "Common.h"
#include "umcf/UMcfHeader.h"

namespace MCFCore
{
	MCFHeaderI::~MCFHeaderI()
	{
	}
}

static char default_header[ 5 ] = "LMCF";

UMcfHeader::UMcfHeader()
: m_iFileVer(MCF_CURRENTVERSION)
, m_iBuild()
, m_iId(0)
, m_iType(0)
, m_iXmlStart(0)
, m_iXmlSize(0)
, m_iFlags(0)
, m_iParentMcf(0)
, m_iBranch()
{
	strcpy( m_szId, default_header );
}

UMcfHeader::UMcfHeader(MCFCore::MCFHeaderI* head)
: m_iFileVer(MCF_CURRENTVERSION)
, m_iBuild()
, m_iId(0)
, m_iType(0)
, m_iXmlStart(0)
, m_iXmlSize(0)
, m_iFlags(0)
, m_iParentMcf(0)
, m_iBranch()
{
	strcpy( m_szId, default_header );

	if (!head)
		return;

	m_iFileVer = head->getFileVer();
	m_iBuild = head->getBuild();
	m_iId = head->getId();
	m_iType = head->getType();
	m_iXmlStart = head->getXmlStart();
	m_iXmlSize = head->getXmlSize();
	m_iFlags = head->getFlags();
	m_iParentMcf = head->getParent();
	m_iBranch = head->getBranch();
}

#ifdef WIN32
uint8 UMcfHeader::readFromFile(HANDLE hFile)
#else
uint8 UMcfHeader::readFromFile(FILE* hFile)
#endif
{
	if (!hFile)
		return UMcfHeader::ERR_NULLHANDLE;

	uint32 size = getSize();
	gcBuff szBuff(size);

#ifdef WIN32
	DWORD dwRead = 0;
	int readRet = ReadFile(hFile, szBuff.c_ptr(), size, &dwRead, nullptr);

	if (dwRead != size)
		return UMcfHeader::ERR_PARTREAD;
#else
	size_t amount = fread(szBuff.c_ptr(), 1, size, hFile);

	if (amount != size)
		return UMcfHeader::ERR_PARTREAD;
#endif

	strToHeader(szBuff.c_ptr());

	if (isValid())
		return UMcfHeader::OK;
	else
		return UMcfHeader::ERR_INVALID;
}

char* UMcfHeader::headerToStr() const
{
	char* temp = new char[getSize()];

	temp[0]  = m_szId[0];
	temp[1]  = m_szId[1];
	temp[2]  = m_szId[2];
	temp[3]  = m_szId[3];
	temp[4]  = (uint8) m_iFileVer;
	temp[5]  = (uint8)(m_iBuild>>0)&0xFF;
	temp[6]  = (uint8)(m_iBuild>>8)&0xFF;
	temp[7]  = (uint8)(m_iBuild>>16)&0xFF;
	temp[8]  = (uint8)(m_iBuild>>24)&0xFF;
	temp[9]  = (uint8)(m_iId>>0)&0xFF;
	temp[10] = (uint8)(m_iId>>8)&0xFF;
	temp[11] = (uint8)(m_iId>>16)&0xFF;
	temp[12] = (uint8)(m_iId>>24)&0xFF;
	temp[13] = (uint8) m_iType;
	temp[14] = (uint8)(m_iXmlStart>>0)&0xFF;
	temp[15] = (uint8)(m_iXmlStart>>8)&0xFF;
	temp[16] = (uint8)(m_iXmlStart>>16)&0xFF;
	temp[17] = (uint8)(m_iXmlStart>>24)&0xFF;
	temp[18] = (uint8)(m_iXmlStart>>32)&0xFF;
	temp[19] = (uint8)(m_iXmlStart>>40)&0xFF;
	temp[20] = (uint8)(m_iXmlStart>>48)&0xFF;
	temp[21] = (uint8)(m_iXmlStart>>52)&0xFF;
	temp[22] = (uint8)(m_iXmlSize>>0)&0xFF;
	temp[23] = (uint8)(m_iXmlSize>>8)&0xFF;
	temp[24] = (uint8)(m_iXmlSize>>16)&0xFF;
	temp[25] = (uint8)(m_iXmlSize>>24)&0xFF;
	temp[26] = (uint8) m_iFlags;
	temp[27] = (uint8)(m_iParentMcf>>0)&0xFF;
	temp[28] = (uint8)(m_iParentMcf>>8)&0xFF;
	temp[29] = (uint8)(m_iParentMcf>>16)&0xFF;
	temp[30] = (uint8)(m_iParentMcf>>24)&0xFF;

	if (m_iFileVer >= 0x02)
	{
		temp[31] = (uint8)(m_iBranch>>0)&0xFF;
		temp[32] = (uint8)(m_iBranch>>8)&0xFF;
		temp[33] = (uint8)(m_iBranch>>16)&0xFF;
		temp[34] = (uint8)(m_iBranch>>24)&0xFF;
	}

	return temp;
};


template <typename T>
T readInt(const uint8* str)
{
	T res = 0;

	for (size_t x = 0; x < sizeof(T); ++x)
		res += ((T)str[x]) << (x * 8);

	return res;
}

template <typename T>
T readInt(const char* str)
{
	return readInt<T>((uint8*)str);
}

void UMcfHeader::strToHeader(const char* str)
{
	m_szId[0] = str[0];
	m_szId[1] = str[1];
	m_szId[2] = str[2];
	m_szId[3] = str[3];
	m_iFileVer = str[4];

	m_iBuild = MCFBuild::BuildFromInt(readInt<uint32>(&str[5]));
	m_iId = readInt<uint32>(&str[9]);
	m_iType = str[13];
	m_iXmlStart = readInt<uint64>(&str[14]);
	m_iXmlSize = readInt<uint32>(&str[22]);
	m_iFlags = str[26];
	m_iParentMcf = readInt<uint32>(&str[27]);

	if (m_iFileVer >= 0x02)
		m_iBranch = MCFBranch::BranchFromInt(readInt<uint32>(&str[31]));
	else
		m_iBranch = MCFBranch();
}


bool UMcfHeader::isValid() const
{
	if ( !(m_szId[0] == 'L' && m_szId[1] == 'M' && m_szId[2] == 'C' && m_szId[3] == 'F') )
		return false;

	return true;
}

bool UMcfHeader::matches(MCFCore::MCFHeaderI* other) const
{
	return (other && getDesuraId() == other->getDesuraId() && getBranch() == other->getBranch());
}

uint8 UMcfHeader::getSize() const
{
	switch (m_iFileVer)
	{
	default:
	case 1:
		return MCF_HEADERSIZE_V1;

	case 2:
		return MCF_HEADERSIZE_V2;
	};

	return 0;
}

void UMcfHeader::updateFileVersion()
{
	m_iFileVer = 2;
}
