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
#include "IPCParameter.h"

IPC::IPCParameterI* newParameterMacro(IPC::PVoid val)
{
	return new IPC::PVoid();
}

IPC::IPCParameterI* new_void()
{
	return new IPC::PVoid();
}

IPC::AutoReg<void> a = IPC::AutoReg<void>(&new_void);

//REGTYPE( PVoid, void );
REGTYPE(IPC::PUint32, uint8);
REGTYPE(IPC::PUint32, uint32);
REGTYPE(IPC::PInt32, int32);
REGTYPE(IPC::PUint64, uint64);
REGTYPE(IPC::PBool, bool);
REGTYPE(IPC::PException, gcException);
REGTYPEP(IPC::PString, char);
REGTYPEC(IPC::PString, char const, charconst);
REGTYPE(IPC::PDouble, double);

IPC::IPCParameterI* new_ipc_blob()
{
	return new IPC::PBlob();
}

IPC::AutoReg<IPC::PBlob> ar_ipc_blob = IPC::AutoReg<IPC::PBlob>(new_ipc_blob);

IPC::IPCParameterI* newParameterMacro(IPC::PBlob val)
{
	return new IPC::PBlob(val);
}




IPC::IPCParameterI* new_ipc_mapstringstring()
{
	return new IPC::PMapStringString();
}

IPC::AutoReg<IPC::PMapStringString> ar_ipc_mapstringstring = IPC::AutoReg<IPC::PMapStringString>(new_ipc_mapstringstring);

IPC::IPCParameterI* newParameterMacro(std::map<std::string, std::string> map)
{
	return new IPC::PMapStringString(map);
}

IPC::IPCParameterI* newParameterMacro(std::map<std::string, std::string>& map)
{
	return new IPC::PMapStringString(map);
}

IPC::IPCParameterI* newParameterMacro(std::map<std::string, std::string> *pMap)
{
	if (pMap)
		return new IPC::PMapStringString(*pMap);

	return new IPC::PMapStringString();
}




namespace IPC
{
	std::map<uint32, newFunc> *g_vParameterType = nullptr;

	class AutoCleanUp
	{
	public:
		~AutoCleanUp()
		{
			safe_delete(g_vParameterType);
		}
	};

	AutoCleanUp acu;

	void RegType(const std::type_info& type, newFunc funct)
	{
		if (!g_vParameterType)
		{
			g_vParameterType = new std::map<uint32, newFunc>;
		}

		uint32 hash = UTIL::MISC::RSHash_CSTR(type.name());
		(*g_vParameterType)[hash] = funct;
	}

	IPCParameterI* newParameter(uint32 type, const char* buff, uint32 size)
	{
		if (!g_vParameterType)
			return new PVoid();

		std::map<uint32, newFunc>::iterator it = g_vParameterType->find(type);

		if (it == g_vParameterType->end())
			return new PVoid();

		IPCParameterI* p = it->second();
		p->deserialize(buff, size);

		return p;
	}
}


using namespace IPC;



PVoid::PVoid()
{
}

uint32 PVoid::getSerializeSize()
{
	return 0;
}

void PVoid::serialize(char* szBuffer)
{
}

uint32 PVoid::deserialize(const char* buffer, uint32 size)
{
	return 0;
}

uint64 PVoid::getValue(bool dup)
{
	return 0;
}








PBool::PBool()
{
	m_bValue = false;
}

PBool::PBool(bool val)
{
	m_bValue = val;
}

uint32 PBool::getSerializeSize()
{
	return 1;
}

void PBool::serialize(char* szBuffer)
{
	szBuffer[0] = m_bValue;
}

uint32 PBool::deserialize(const char* buffer, uint32 size)
{
	if (size >= 1)
	{
		m_bValue = buffer[0] ? true : false;
		return 1;
	}

	return 0;
}

uint64 PBool::getValue(bool dup)
{
	return (uint64)m_bValue;
}






PUint32::PUint32()
{
	m_uiValue = 0;
}

PUint32::PUint32(uint32 val)
{
	m_uiValue = val;
}

uint32 PUint32::getSerializeSize()
{
	return 4;
}

void PUint32::serialize(char* szBuffer)
{
	szBuffer[0] = m_uiValue & 0xFF;
	szBuffer[1] = (m_uiValue >> 8) & 0xFF;
	szBuffer[2] = (m_uiValue >> 16) & 0xFF;
	szBuffer[3] = (m_uiValue >> 24) & 0xFF;
}

uint32 PUint32::deserialize(const char* buffer, uint32 size)
{
	if (size >= 4)
	{
		m_uiValue = buffToUint32(buffer);
		return 4;
	}

	return 0;
}

uint64 PUint32::getValue(bool dup)
{
	return (uint64)m_uiValue;
}






PInt32::PInt32()
{
	m_iValue = 0;
}

PInt32::PInt32(int32 value)
{
	m_iValue = value;
}

uint32 PInt32::getSerializeSize()
{
	return 4;
}

void PInt32::serialize(char* szBuffer)
{
	szBuffer[0] = m_iValue & 0xFF;
	szBuffer[1] = (m_iValue >> 8) & 0xFF;
	szBuffer[2] = (m_iValue >> 16) & 0xFF;
	szBuffer[3] = (m_iValue >> 24) & 0xFF;
}

uint32 PInt32::deserialize(const char* buffer, uint32 size)
{
	if (size >= 4)
	{
		m_iValue = buffToInt32(buffer);
		return 4;
	}

	return 0;
}

uint64 PInt32::getValue(bool dup)
{
	return (uint64)m_iValue;
}





typedef union
{
	char data[8];
	uint64 num;
} uint64_u;


PUint64::PUint64()
{
	m_uiValue = 0;
}

PUint64::PUint64(uint64 val)
{
	m_uiValue = val;
}

uint32 PUint64::getSerializeSize()
{
	return 8;
}

void PUint64::serialize(char* szBuffer)
{
	uint64_u u;
	u.num = m_uiValue;
	memcpy(szBuffer, u.data, 8);
}

uint32 PUint64::deserialize(const char* buffer, uint32 size)
{
	if (size >= 8)
	{
		uint64_u u;
		memcpy(u.data, buffer, 8);
		m_uiValue = u.num;
		return 8;
	}

	return 0;
}

uint64 PUint64::getValue(bool dup)
{
	return (uint64)m_uiValue;
}







typedef union
{
	char data[8];
	double num;
} double_u;


PDouble::PDouble()
{
	m_dValue = 0.0;
}

PDouble::PDouble(double val)
{
	m_dValue = val;
}

uint32 PDouble::getSerializeSize()
{
	return 8;
}

void PDouble::serialize(char* szBuffer)
{
	double_u u;
	u.num = m_dValue;
	memcpy(szBuffer, u.data, 8);
}

uint32 PDouble::deserialize(const char* buffer, uint32 size)
{
	if (size >= 8)
	{
		double_u u;
		memcpy(u.data, buffer, 8);
		m_dValue = u.num;
		return 8;
	}

	return 0;
}



uint64 PDouble::getValue(bool dup)
{
	DoubleAndUint64 du;
	du.d = m_dValue;
	return du.u;
}




PString::PString()
{
}

PString::PString(const char* v)
{
	if (v)
		m_szValue = std::string(v);
	else
		m_bNull = true;
}

uint32 PString::getSerializeSize()
{
	if (m_bNull)
		return 0;

	return 4 + m_szValue.size();
}

void PString::serialize(char* szBuffer)
{
	if (m_bNull)
		return;

	uint32 strsize = m_szValue.size();

	szBuffer[0] = strsize & 0xFF;
	szBuffer[1] = (strsize >> 8) & 0xFF;
	szBuffer[2] = (strsize >> 16) & 0xFF;
	szBuffer[3] = (strsize >> 24) & 0xFF;

	if (strsize > 0)
		memcpy(szBuffer + 4, m_szValue.c_str(), strsize);
}

uint32 PString::deserialize(const char* buffer, uint32 size)
{
	m_szValue = "";
	m_bNull = true;

	if (size < 4)
		return 0;

	uint32 strsize = buffToInt32(buffer);

	if (strsize > size - 4)
		return 0;

	m_bNull = false;
	m_szValue = std::string(buffer + 4, strsize);
	return 4 + strsize;
}


uint64 PString::getValue(bool dup)
{
	if (m_bNull)
		return 0;

	if (dup)
	{
		char *res = nullptr;
		Safe::strcpy(&res, m_szValue.c_str(), m_szValue.size());
		return (uint64)res;
	}
	else
	{
		return (uint64)m_szValue.c_str();
	}
}






PException::PException()
{
	exception = new gcException();
}

PException::PException(gcException& e)
{
	exception = new gcException(e);
}

PException::~PException()
{
	safe_delete(exception);
}

uint32 PException::getSerializeSize()
{
	const char* msg = exception->getErrMsg();
	uint32 msgSize = strlen(msg);

	return 12 + msgSize;
}

void PException::serialize(char* szBuffer)
{
	uint32 e1 = exception->getErrId();
	uint32 e2 = exception->getSecErrId();

	const char* msg = exception->getErrMsg();
	uint32 msgSize = strlen(msg);

	szBuffer[0] = e1 & 0xFF;
	szBuffer[1] = (e1 >> 8) & 0xFF;
	szBuffer[2] = (e1 >> 16) & 0xFF;
	szBuffer[3] = (e1 >> 24) & 0xFF;

	szBuffer[4] = e2 & 0xFF;
	szBuffer[5] = (e2 >> 8) & 0xFF;
	szBuffer[6] = (e2 >> 16) & 0xFF;
	szBuffer[7] = (e2 >> 24) & 0xFF;

	szBuffer[8] = msgSize & 0xFF;
	szBuffer[9] = (msgSize >> 8) & 0xFF;
	szBuffer[10] = (msgSize >> 16) & 0xFF;
	szBuffer[11] = (msgSize >> 24) & 0xFF;

	memcpy(&szBuffer[12], msg, msgSize);
}

uint32 PException::deserialize(const char* buffer, uint32 size)
{
	uint32 e1;
	uint32 e2;
	uint32 msgSize = 0;
	char* msg = nullptr;

	if (size < 8)
		return 0;

	e1 = buffToUint32(buffer);
	e2 = buffToUint32(buffer + 4);
	msgSize = buffToUint32(buffer + 8);

	if (msgSize > size - 12)
		return 0;

	msg = new char[msgSize + 1];
	memcpy(msg, buffer + 12, msgSize);
	msg[msgSize] = '\0';

	safe_delete(exception);
	exception = new gcException((ERROR_ID)e1, e2, msg);
	safe_delete(msg);

	return 12 + msgSize;
}

uint64 PException::getValue(bool dup)
{
	if (dup)
		return (uint64)(new gcException(exception));
	else
		return (uint64)exception;
}










PBlob::PBlob()
{
	m_szData = nullptr;
	m_uiSize = 0;
}

PBlob::PBlob(const PBlob& e)
{
	m_uiSize = e.getSize();
	m_szData = nullptr;

	if (m_uiSize > 0)
	{
		m_szData = new char[m_uiSize];
		memcpy(m_szData, e.getData(), m_uiSize);
	}
}

PBlob::PBlob(PBlob* blob)
{
	m_uiSize = blob->getSize();

	m_szData = nullptr;

	if (m_uiSize > 0)
	{
		m_szData = new char[m_uiSize];
		memcpy(m_szData, blob->getData(), m_uiSize);
	}
}

PBlob::PBlob(uint64 val)
{
	PBlob* blob = (PBlob*)val;

	m_uiSize = blob->getSize();
	m_szData = nullptr;

	if (m_uiSize > 0)
	{
		m_szData = new char[m_uiSize];
		memcpy(m_szData, blob->getData(), m_uiSize);
	}
}

PBlob::PBlob(const char* data, uint32 size)
{
	m_uiSize = size;
	m_szData = nullptr;

	if (size > 0)
	{
		m_szData = new char[m_uiSize];
		memcpy(m_szData, data, m_uiSize);
	}
}

PBlob::~PBlob()
{
	safe_delete(m_szData);
}

uint32 PBlob::getSerializeSize()
{
	return 4 + m_uiSize;
}

void PBlob::serialize(char* szBuffer)
{
	szBuffer[0] = m_uiSize & 0xFF;
	szBuffer[1] = (m_uiSize >> 8) & 0xFF;
	szBuffer[2] = (m_uiSize >> 16) & 0xFF;
	szBuffer[3] = (m_uiSize >> 24) & 0xFF;

	if (m_uiSize > 0)
		memcpy(szBuffer + 4, m_szData, m_uiSize);
}

uint32 PBlob::deserialize(const char* buffer, uint32 size)
{
	if (size >= 4)
		m_uiSize = buffToInt32(buffer);
	else
		return 0;

	if (m_uiSize > size - 4)
		return 0;

	safe_delete(m_szData);

	if (m_uiSize > 0)
	{
		m_szData = new char[m_uiSize];
		memcpy(m_szData, buffer + 4, m_uiSize);
	}

	return 4 + m_uiSize;
}

uint64 PBlob::getValue(bool dup)
{
	if (dup)
	{
		return (uint64)new PBlob(m_szData, m_uiSize);
	}
	else
	{
		return (uint64)this;
	}
}





















PMapStringString::PMapStringString()
{
}

PMapStringString::PMapStringString(const std::map<std::string, std::string>& map)
	: m_Map(map)
{
}

PMapStringString::~PMapStringString()
{
}

uint32 PMapStringString::getSerializeSize()
{
	auto size = 4;

	for (auto s : m_Map)
	{
		size += 4;
		size += s.first.size();

		size += 4;
		size += s.second.size();
	}

	return size;
}

void PMapStringString::serialize(char* szBuffer)
{
	char* temp = szBuffer;

	Uint32ToBuff(temp, m_Map.size());
	temp += 4;

	for (auto s : m_Map)
	{
		Uint32ToBuff(temp, s.first.size());
		temp += 4;

		Uint32ToBuff(temp, s.second.size());
		temp += 4;

		memcpy(temp, s.first.c_str(), s.first.size());
		temp += s.first.size();

		memcpy(temp, s.second.c_str(), s.second.size());
		temp += s.second.size();
	}
}

uint32 PMapStringString::deserialize(const char* buffer, uint32 size)
{
	m_Map.clear();

	const char* temp = buffer;

	uint32 nCount = buffToUint32(temp);
	temp += 4;

	for (uint32 x = 0; x < nCount; ++x)
	{
		uint32 nKeySize = buffToUint32(temp);
		temp += 4;

		uint32 nValSize = buffToUint32(temp);
		temp += 4;

		if ((temp - buffer) + nKeySize + nValSize > size)
		{
			gcAssert(false);
			break;
		}

		std::string key(temp, nKeySize);
		temp += nKeySize;

		std::string val(temp, nValSize);
		temp += nValSize;

		m_Map[key] = val;
	}

	return temp - buffer;
}

uint64 PMapStringString::getValue(bool dup)
{
	if (dup)
		return (uint64)(new std::map<std::string, std::string>(m_Map));
	else
		return (uint64)&m_Map;
}