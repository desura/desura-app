#include <gtest/gtest.h>

#include "Common.h"

namespace UnitTest
{
	TEST(UtilWindows, sanitiseFileName_Normal)
	{
		auto res = UTIL::WIN::sanitiseFileName("Abcde.txt");
		ASSERT_STREQ("Abcde.txt", res.c_str());
	}

	TEST(UtilWindows, sanitiseFileName_BadChar)
	{
		auto res = UTIL::WIN::sanitiseFileName("\"A?b*c:d<>e|12\\3/.txt");
		ASSERT_STREQ("Abcde123.txt", res.c_str());
	}

	TEST(UtilWindows, sanitiseFileName_Spaces)
	{
		auto res = UTIL::WIN::sanitiseFileName("Abcde.txt   ");
		ASSERT_STREQ("Abcde.txt", res.c_str());
	}
}