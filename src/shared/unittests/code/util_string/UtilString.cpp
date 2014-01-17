
#include <gtest/gtest.h>
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
}

