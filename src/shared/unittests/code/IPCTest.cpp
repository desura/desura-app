/*
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
#include <gtest/gtest.h>

#include "Common.h"
#include "IPCClass.h"
#include "IPCParameterI.h"

using namespace IPC;

class IPCFixture : public ::testing::Test
{
public:
	~IPCFixture()
	{
		free(m_szBuffer);
	}

	void voidFunct()
	{
		m_bVoidFunct = true;
	}

	void voidFunctOne(int a)
	{
		EXPECT_EQ(1234, a);
		m_bVoidFunctOne = true;
	}

	void voidFunctTwo(const char* str, int a)
	{
		EXPECT_EQ(6789, a);
		EXPECT_STREQ("apple", str);
		m_bVoidFunctTwo = true;
	}

	int normFunct()
	{
		m_bNormFunct = true;
		return 345;
	}

	char* normFunctOne(int a)
	{
		EXPECT_EQ(1234, a);
		m_bNormFunctOne = true;

		char* ret = new char[8];
		Safe::strcpy(ret, 8, "banana");

		return ret;
	}

	bool normFunctTwo(const char* str, int a)
	{
		EXPECT_EQ(6789, a);
		EXPECT_STREQ("cherry", str);
		m_bNormFunctTwo = true;
		return true;
	}

	void setCrashSettings(bool upload, const char* user)
	{
		EXPECT_EQ(upload, true);
		EXPECT_STREQ("lodle", user);
	}

	template <typename T>
	void addParameter(T t)
	{
		auto a = std::unique_ptr<IPCParameterI>(newParameterS(t));

		uint32 size = 0;
		char* out = a->serialize(size);

		IPCParameter p;
		p.size = size;
		p.type = IPC::getType<T>();

		if (m_nLastBuffPos + sizeofStruct(&p) > m_nBuffSize)
		{
			m_nBuffSize = (m_nLastBuffPos + sizeofStruct(&p)) * 2;
			m_szBuffer = (char*)realloc(m_szBuffer, m_nBuffSize);
		}

		memcpy(m_szBuffer + m_nLastBuffPos, &p, 8);
		memcpy(m_szBuffer + m_nLastBuffPos + 8, out, size);

		m_nParamCount++;
		m_nLastBuffPos += sizeofStruct(&p);

		safe_delete(out);
	}

	char* m_szBuffer = nullptr;
	uint32 m_nBuffSize = 0;
	uint32 m_nLastBuffPos = 0;
	uint8 m_nParamCount = 0;

	bool m_bVoidFunct = false;
	bool m_bVoidFunctOne = false;
	bool m_bVoidFunctTwo = false;

	bool m_bNormFunct = false;
	bool m_bNormFunctOne = false;
	bool m_bNormFunctTwo = false;
};

TEST_F(IPCFixture, voidFunct)
{
	auto pFunct = networkFunctionV<IPCFixture>(this, &IPCFixture::voidFunct, "voidFunct");
	auto pRet = std::unique_ptr<IPCParameterI>(pFunct->call(nullptr, 0, 0));

	ASSERT_TRUE(m_bVoidFunct);
	ASSERT_EQ(IPC::getType<void>(), pRet->getType());
}

TEST_F(IPCFixture, voidFunctOne)
{
	auto pFunct = networkFunctionV<IPCFixture>(this, &IPCFixture::voidFunctOne, "voidFunctOne");

	addParameter<int>(1234);

	auto pRet = std::unique_ptr<IPCParameterI>(pFunct->call(m_szBuffer, m_nBuffSize, m_nParamCount));

	ASSERT_TRUE(m_bVoidFunctOne);
	ASSERT_EQ(IPC::getType<void>(), pRet->getType());
}

TEST_F(IPCFixture, voidFunctTwo)
{
	auto pFunct = networkFunctionV<IPCFixture>(this, &IPCFixture::voidFunctTwo, "voidFunctTwo");

	addParameter<char*>("apple");
	addParameter<int>(6789);

	auto pRet = std::unique_ptr<IPCParameterI>(pFunct->call(m_szBuffer, m_nBuffSize, m_nParamCount));

	ASSERT_TRUE(m_bVoidFunctTwo);
	ASSERT_EQ(IPC::getType<void>(), pRet->getType());
}

TEST_F(IPCFixture, setCrashSettings)
{
	auto pFunct = networkFunctionV<IPCFixture>(this, &IPCFixture::setCrashSettings, "setCrashSettings");

	addParameter<bool>(true);
	addParameter<char*>("lodle");

	auto pRet = std::unique_ptr<IPCParameterI>(pFunct->call(m_szBuffer, m_nBuffSize, m_nParamCount));
	ASSERT_EQ(IPC::getType<void>(), pRet->getType());
}





TEST_F(IPCFixture, normFunct)
{
	auto pFunct = networkFunction<IPCFixture>(this, &IPCFixture::normFunct, "normFunct");
	auto pRet = std::unique_ptr<IPCParameterI>(pFunct->call(nullptr, 0, 0));

	ASSERT_TRUE(m_bNormFunct);
	ASSERT_EQ(IPC::getType<int>(), pRet->getType());
	ASSERT_EQ(345, getParameterValue<int>(pRet.get()));
}

TEST_F(IPCFixture, normFunctOne)
{
	auto pFunct = networkFunction<IPCFixture>(this, &IPCFixture::normFunctOne, "normFunctOne");

	addParameter<int>(1234);

	auto pRet = std::unique_ptr<IPCParameterI>(pFunct->call(m_szBuffer, m_nBuffSize, m_nParamCount));

	ASSERT_TRUE(m_bNormFunctOne);
	ASSERT_EQ(IPC::getType<char*>(), pRet->getType());
	ASSERT_STREQ("banana", getParameterValue<char*>(pRet.get()));
}

TEST_F(IPCFixture, normFunctTwo)
{
	auto pFunct = networkFunction<IPCFixture>(this, &IPCFixture::normFunctTwo, "normFunctTwo");

	addParameter<char*>("cherry");
	addParameter<int>(6789);

	auto pRet = std::unique_ptr<IPCParameterI>(pFunct->call(m_szBuffer, m_nBuffSize, m_nParamCount));

	ASSERT_TRUE(m_bNormFunctTwo);
	ASSERT_EQ(IPC::getType<bool>(), pRet->getType());
	ASSERT_EQ(true, getParameterValue<bool>(pRet.get()));
}