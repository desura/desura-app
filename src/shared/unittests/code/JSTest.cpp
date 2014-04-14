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
#include "gcJSBase.h"

using namespace testing;

namespace UnitTest
{
	class JavaScriptFactoryMock : public ChromiumDLL::JavaScriptFactoryI
	{
	public:
		MOCK_METHOD0(CreateUndefined, JSObjHandle());
		MOCK_METHOD0(CreateNull, JSObjHandle());
		MOCK_METHOD1(CreateBool, JSObjHandle(bool value));
		MOCK_METHOD1(CreateInt, JSObjHandle(int value));
		MOCK_METHOD1(CreateDouble, JSObjHandle(double value));
		MOCK_METHOD1(CreateString, JSObjHandle(const char* value));
		MOCK_METHOD0(CreateObject, JSObjHandle());
		MOCK_METHOD1(CreateObject, JSObjHandle(void* userData));
		MOCK_METHOD1(CreateException, JSObjHandle(const char* value));
		MOCK_METHOD0(CreateArray, JSObjHandle());
		MOCK_METHOD2(CreateFunction, JSObjHandle(const char* name, ChromiumDLL::JavaScriptExtenderI* handler));
	};

	class JavaScriptObjectMock : public ChromiumDLL::JavaScriptObjectI
	{
	public:
		MOCK_METHOD0(clone, ChromiumDLL::JavaScriptObjectI*());
		MOCK_METHOD0(isUndefined, bool());
		MOCK_METHOD0(isNull, bool());
		MOCK_METHOD0(isBool, bool());
		MOCK_METHOD0(isInt, bool());
		MOCK_METHOD0(isDouble, bool());
		MOCK_METHOD0(isString, bool());
		MOCK_METHOD0(isObject, bool());
		MOCK_METHOD0(isArray, bool());
		MOCK_METHOD0(isFunction, bool());
		MOCK_METHOD0(isException, bool());
		MOCK_METHOD0(getBoolValue, bool());
		MOCK_METHOD0(getIntValue, int());
		MOCK_METHOD0(getDoubleValue, double());
		MOCK_METHOD2(getStringValue, int(char* buff, size_t buffsize));
		MOCK_METHOD1(hasValue, bool(const char* key));
		MOCK_METHOD1(hasValue, bool(int index));
		MOCK_METHOD1(deleteValue, bool(const char* key));
		MOCK_METHOD1(deleteValue, bool(int index));
		MOCK_METHOD1(getValue, ChromiumDLL::JSObjHandle(const char* key));
		MOCK_METHOD1(getValue, ChromiumDLL::JSObjHandle(int index));
		MOCK_METHOD2(setValue, bool(const char* key, ChromiumDLL::JSObjHandle value));
		MOCK_METHOD2(setValue, bool(int index, ChromiumDLL::JSObjHandle value));
		MOCK_METHOD0(getNumberOfKeys, int());
		MOCK_METHOD3(getKey, void(int index, char* buff, size_t buffsize));
		MOCK_METHOD0(getArrayLength, int());
		MOCK_METHOD2(getFunctionName, void(char* buff, size_t buffsize));
		MOCK_METHOD0(getFunctionHandler, ChromiumDLL::JavaScriptExtenderI*());
		MOCK_METHOD1(executeFunction, ChromiumDLL::JSObjHandle(ChromiumDLL::JavaScriptFunctionArgs*));
		MOCK_METHOD0(getUserObject, void*());

		void addRef() override
		{
			++m_nRefCount;
		}

		void delRef() override
		{
			--m_nRefCount;

			if (m_nRefCount == 0)
				delete this;
		}

		int m_nRefCount = 0;
	};


	class FromJSObjectFixture : public ::testing::Test
	{
	public:
		MOCK_METHOD5(showContextMenu, void(int32 xPos, int32 yPos, std::vector<std::map<gcString, gcString>> args, JSObjHandle thisObj, JSObjHandle callBack));

		JSObjHandle generateInt(int nVal)
		{
			auto val = new JavaScriptObjectMock();

			ON_CALL(*val, isInt()).WillByDefault(Return(true));
			ON_CALL(*val, getIntValue()).WillByDefault(Return(nVal));

			return JSObjHandle(val);
		}

		JSObjHandle generateString(gcString &strVal)
		{
			auto val = new JavaScriptObjectMock();

			ON_CALL(*val, isString()).WillByDefault(Return(true));
			ON_CALL(*val, getStringValue(_, _)).WillByDefault(Invoke([strVal](char* buff, size_t buffsize) -> int
			{
				if (!buff)
					return strVal.size();

				return Safe::strcpy(buff, buffsize, strVal.c_str());
			}));

			return JSObjHandle(val);
		}

		JSObjHandle generateVector(std::vector<JSObjHandle> &vItems)
		{
			auto obj = new JavaScriptObjectMock();

			ON_CALL(*obj, isArray()).WillByDefault(Return(true));
			ON_CALL(*obj, getArrayLength()).WillByDefault(Return(vItems.size()));

			int x = 0;
			for (auto i : vItems)
			{
				ON_CALL(*obj, getValue(Matcher<int>(Eq(x)))).WillByDefault(Return(i));
				x++;
			}

			return JSObjHandle(obj);
		}

		JSObjHandle generateMap(std::map<gcString, int> &mItems)
		{
			auto obj = new JavaScriptObjectMock();

			ON_CALL(*obj, isObject()).WillByDefault(Return(true));
			ON_CALL(*obj, getNumberOfKeys()).WillByDefault(Return(mItems.size()));

			int x = 0;
			for (auto k : mItems)
			{
				std::string strKey = k.first;
	
				ON_CALL(*obj, getKey(Eq(x), _, _)).WillByDefault(Invoke([strKey](int index, char* buff, size_t buffsize){
					Safe::strcpy(buff, buffsize, strKey.c_str());
				}));

				JSObjHandle val = generateInt(k.second);
				ON_CALL(*obj, getValue(Matcher<const char*>(StrEq(strKey)))).WillByDefault(Return(val));

				x++;
			}

			return JSObjHandle(obj);
		}

		JSObjHandle generateMap(std::map<gcString, gcString> &mItems)
		{
			auto obj = new JavaScriptObjectMock();

			ON_CALL(*obj, isObject()).WillByDefault(Return(true));
			ON_CALL(*obj, getNumberOfKeys()).WillByDefault(Return(mItems.size()));

			int x = 0;
			for (auto k : mItems)
			{
				std::string strKey = k.first;

				ON_CALL(*obj, getKey(Eq(x), _, _)).WillByDefault(Invoke([strKey](int index, char* buff, size_t buffsize){
					Safe::strcpy(buff, buffsize, strKey.c_str());
				}));

				JSObjHandle val = generateString(k.second);
				ON_CALL(*obj, getValue(Matcher<const char*>(StrEq(strKey)))).WillByDefault(Return(val));

				x++;
			}

			return JSObjHandle(obj);
		}
	};

	TEST_F(FromJSObjectFixture, std_map_int)
	{
		std::map<gcString, int> mBase;

		mBase["key-1"] = 1;
		mBase["key-2"] = 2;

		auto handle = generateMap(mBase);

		std::map<gcString, int> mRes;
		FromJSObject(mRes, handle);

		EXPECT_EQ(mBase.size(), mRes.size());

		for (auto p : mBase)
		{
			EXPECT_EQ(p.second, mRes[p.first]);
		}

		EXPECT_EQ(mBase.size(), mRes.size());
	}

	TEST_F(FromJSObjectFixture, std_vector_int)
	{
		std::vector<JSObjHandle> vBase = { generateInt(1), generateInt(2) };
		auto handle = generateVector(vBase);

		std::vector<int> vRes;
		FromJSObject(vRes, handle);

		EXPECT_EQ(vBase.size(), vRes.size());

		for (size_t x = 0; x < vBase.size(); x++)
		{
			EXPECT_EQ(vBase[x]->getIntValue(), vRes[x]);
		}
	}

	TEST_F(FromJSObjectFixture, std_vector_map_int)
	{
		std::map<gcString, int> m1;

		m1["m1-key-1"] = 1;
		m1["m1-key-2"] = 2;

		std::map<gcString, int> m2;

		m2["m2-key-1"] = 8;
		m2["m2-key-2"] = 9;

		std::vector<JSObjHandle> vBase = { generateMap(m1), generateMap(m2) };
		auto handle = generateVector(vBase);

		std::vector<std::map<gcString, int>> vRes;
		FromJSObject(vRes, handle);

		EXPECT_EQ(vBase.size(), vRes.size());

		EXPECT_EQ(m1.size(), vRes[0].size());

		for (auto p : m1)
		{
			EXPECT_EQ(p.second, vRes[0][p.first]);
		}

		EXPECT_EQ(m1.size(), vRes[0].size());

		EXPECT_EQ(m2.size(), vRes[1].size());

		for (auto p : m2)
		{
			EXPECT_EQ(p.second, vRes[1][p.first]);
		}

		EXPECT_EQ(m2.size(), vRes[1].size());
	}


	TEST_F(FromJSObjectFixture, std_vector_map_string)
	{
		std::map<gcString, gcString> m1;

		m1["m1-key-1"] = "m1-val-1";
		m1["m1-key-2"] = "m1-val-2";

		std::map<gcString, gcString> m2;

		m2["m2-key-1"] = "m2-val-1";
		m2["m2-key-2"] = "m2-val-2";

		std::vector<JSObjHandle> vBase = { generateMap(m1), generateMap(m2) };
		auto handle = generateVector(vBase);

		std::vector<std::map<gcString, gcString>> vRes;
		FromJSObject(vRes, handle);

		EXPECT_EQ(vBase.size(), vRes.size());

		EXPECT_EQ(m1.size(), vRes[0].size());

		for (auto p : m1)
		{
			EXPECT_EQ(p.second, vRes[0][p.first]);
		}

		EXPECT_EQ(m1.size(), vRes[0].size());

		EXPECT_EQ(m2.size(), vRes[1].size());

		for (auto p : m2)
		{
			EXPECT_EQ(p.second, vRes[1][p.first]);
		}

		EXPECT_EQ(m2.size(), vRes[1].size());
	}

	TEST_F(FromJSObjectFixture, contextMenuInvoke)
	{
		std::map<gcString, gcString> m1;

		m1["m1-key-1"] = "m1-val-1";
		m1["m1-key-2"] = "m1-val-2";

		std::map<gcString, gcString> m2;

		m2["m2-key-1"] = "m2-val-1";
		m2["m2-key-2"] = "m2-val-2";

		std::vector<JSObjHandle> vBase = { generateMap(m1), generateMap(m2) };

		EXPECT_CALL(*this, showContextMenu(1, 2, _, _, _)).WillOnce(Invoke([m1, m2](int32 xPos, int32 yPos, std::vector<std::map<gcString, gcString>> vRes, JSObjHandle thisObj, JSObjHandle callBack){

			EXPECT_EQ(2, vRes.size());

			EXPECT_EQ(m1.size(), vRes[0].size());

			for (auto p : m1)
			{
				EXPECT_EQ(p.second, vRes[0][p.first]);
			}

			EXPECT_EQ(m1.size(), vRes[0].size());

			EXPECT_EQ(m2.size(), vRes[1].size());

			for (auto p : m2)
			{
				EXPECT_EQ(p.second, vRes[1][p.first]);
			}

			EXPECT_EQ(m2.size(), vRes[1].size());

		}));


		auto pDel = std::unique_ptr<JSDelegateI>(newJSVoidFunctionDelegate((FromJSObjectFixture*)this, &FromJSObjectFixture::showContextMenu, false));

		JavaScriptFactoryMock mock;
		ON_CALL(mock, CreateUndefined()).WillByDefault(Return(nullptr));

		std::vector<JSObjHandle> vArgs = { generateInt(1), generateInt(2), generateVector(vBase), JSObjHandle(), JSObjHandle() };
		EXPECT_TRUE(vArgs[2]->isArray());

		(*pDel.get())(&mock, nullptr, JSObjHandle(), vArgs.size(), &vArgs[0]);
	}
}