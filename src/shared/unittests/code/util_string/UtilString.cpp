

#include "Common.h"


using namespace UTIL::STRING;

namespace UnitTest
{
	TEST(UtilString, base64encode)
	{
		auto str = std::string("HelloImAString! ");
		auto res = base64_encode(str.c_str(), str.size());

		ASSERT_STREQ("SGVsbG9JbUFTdHJpbmchIA==", res.c_str());
	}

	TEST(UtilString, base64decode)
	{
		auto str = std::string("SGVsbG9JbUFTdHJpbmchIA==");
		auto res = base64_decode(str);

		auto strRes = std::string(res.c_ptr(), res.size());

		ASSERT_STREQ("HelloImAString! ", strRes.c_str());
	}


	class UtilStringP : public ::testing::TestWithParam<std::pair<int, int>>
	{
	public:

	};

	INSTANTIATE_TEST_CASE_P(zeroBuffer_char,
		UtilStringP,
		::testing::Values(std::make_pair(0, 10), std::make_pair(0, 5), std::make_pair(5, 10),
		std::make_pair(2, 8)));

	INSTANTIATE_TEST_CASE_P(zeroBuffer_wchar,
		UtilStringP,
		::testing::Values(std::make_pair(0, 10), std::make_pair(0, 5), std::make_pair(5, 10),
		std::make_pair(2, 8)));

	TEST_P(UtilStringP, zeroBuffer_char)
	{
		char szTest[10] = {(char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF};

		auto off = GetParam().first;
		auto end = GetParam().second;

		UTIL::STRING::zeroBuffer(&szTest[off], end - off);

		for (int x = 0; x < off; x++)
			ASSERT_EQ((char)0xFF, szTest[x]);

		for (int x=off; x<end; x++)
			ASSERT_EQ((char)0, szTest[x]);

		for (int x = end; x < 10; x++)
			ASSERT_EQ((char)0xFF, szTest[x]);
	}

	TEST_P(UtilStringP, zeroBuffer_wchar)
	{
		wchar_t szTest[10] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};

		auto off = GetParam().first;
		auto end = GetParam().second;

		UTIL::STRING::zeroBuffer(&szTest[off], end - off);

		for (int x = 0; x < off; x++)
			ASSERT_EQ((wchar_t)0xFFFF, szTest[x]);

		for (int x = off; x < end; x++)
			ASSERT_EQ((wchar_t)0, szTest[x]);

		for (int x = end; x < 10; x++)
			ASSERT_EQ((wchar_t)0xFFFF, szTest[x]);
	}
}

