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
#include "util/UtilMisc.h"
#include "SharedObjectLoader.h"

namespace UnitTest
{
	TEST(UtilDateTime, fixDateTimeString_valid_string)
	{
		std::string input_string("20131025T123456");
		std::string expected_result(input_string);

		std::string s = UTIL::MISC::fixDateTimeString(input_string.c_str());

		ASSERT_STREQ(expected_result.c_str(), s.c_str());
	}

	TEST(UtilDateTime, fixDateTimeString_missing_t)
	{
		std::string input_string("20150410345612");
		std::string expected_result("20150410T345612");

		std::string s = UTIL::MISC::fixDateTimeString(input_string.c_str());

		ASSERT_STREQ(expected_result.c_str(), s.c_str());
	}

	TEST(UtilDateTime, fixDateTimeString_short_string)
	{
		std::string input_string("20110708");
		std::string expected_result("20110708T000000");

		std::string s = UTIL::MISC::fixDateTimeString(input_string.c_str());

		ASSERT_STREQ(expected_result.c_str(), s.c_str());
	}

	TEST(UtilDateTime, fixDateTimeString_very_short_string)
	{
		std::string input_string("201007");
		std::string expected_result("");

		std::string s = UTIL::MISC::fixDateTimeString(input_string.c_str());

		ASSERT_STREQ(expected_result.c_str(), s.c_str());
	}

	TEST(UtilDateTime, fixDateTimeString_simple_day_diff)
	{
		std::string dateTime1("20100101T000000");
		std::string dateTime2("20100102T000000");

		unsigned int days = 9999;
		unsigned int hours = 1000;

		UTIL::MISC::getTimeDiff(dateTime1.c_str(), dateTime2.c_str(), days, hours);

		int expected_days = 1;
		int expected_hours = 0;

		ASSERT_EQ(expected_days, days);
		ASSERT_EQ(expected_hours, hours);
	}

	TEST(UtilDateTime, fixDateTimeString_less_simple_day_diff)
	{
		std::string dateTime1("20100430T000000");
		std::string dateTime2("20100502T000000");

		unsigned int days = 9999;
		unsigned int hours = 1000;

		UTIL::MISC::getTimeDiff(dateTime1.c_str(), dateTime2.c_str(), days, hours);

		int expected_days = 2;
		int expected_hours = 0;

		ASSERT_EQ(expected_days, days);
		ASSERT_EQ(expected_hours, hours);
	}

	TEST(UtilDateTime, fixDateTimeString_less_simple_day_diff_invalid_dates)
	{
		std::string dateTime1("20100731000000");
		std::string dateTime2("20100805170000");

		unsigned int days = 9999;
		unsigned int hours = 1000;

		UTIL::MISC::getTimeDiff(dateTime1.c_str(), dateTime2.c_str(), days, hours);

		int expected_days = 5;
		int expected_hours = 17;

		ASSERT_EQ(expected_days, days);
		ASSERT_EQ(expected_hours, hours);
	}

	TEST(UtilDateTime, fixDateTimeString_less_simple_day_diff_short_dates)
	{
		std::string dateTime1("20101130");
		std::string dateTime2("20101220");

		unsigned int days = 9999;
		unsigned int hours = 1000;

		UTIL::MISC::getTimeDiff(dateTime1.c_str(), dateTime2.c_str(), days, hours);

		int expected_days = 20;
		int expected_hours = 0;

		ASSERT_EQ(expected_days, days);
		ASSERT_EQ(expected_hours, hours);
	}

	TEST(UtilDateTime, dateTimeToDisplay_simple_test)
	{
		// only do this for USA locales. Constructing locale here and using it to generate
		// expected string is what happens in code being tested so not a good unit test
		std::locale my_locale("");
		if (my_locale.name().find("United States") != std::string::npos)
		{
			std::string input_string("20110708T201530");
			std::string expected_result("07/08/11");

			std::string s = UTIL::MISC::dateTimeToDisplay(input_string.c_str());

			ASSERT_STREQ(expected_result.c_str(), s.c_str());
		}
		else
		{
			std::cerr << "Warning - locale not USA - dateTimeToDisplay_simple_test not executing" << std::endl;
		}
	}

	TEST(UtilDateTime, toIsoAndBack)
	{
		tm t = { 0 };
		t.tm_hour = 8;
		t.tm_min = 6;
		t.tm_sec = 56;

		t.tm_year = 113;
		t.tm_mon = 8;
		t.tm_mday = 10;

		auto timet = mktime(&t);

		auto strTestTimeStamp(gcTime::to_iso_string(timet));
		auto time = gcTime::from_iso_string(strTestTimeStamp);

		ASSERT_EQ(timet, time.to_time_t());
	}

	TEST(UtilDateTime, BadTimeT_Static)
	{
		auto out = gcTime::to_iso_string(-1);
		std::string expected = "";

		ASSERT_EQ(expected, out);
	}

	TEST(UtilDateTime, BadTimeT)
	{
		auto out = gcTime((time_t)-1).to_iso_string();
		std::string expected = "";

		ASSERT_EQ(expected, out);
	}

	TEST(SharedObjectLoader, ConvertToLinux_Win)
	{
		SharedObjectLoader sol;

		auto out = sol.convertToLinuxModule("uicore.dll");
		ASSERT_STREQ("libuicore.so", out.c_str());
	}


	TEST(SharedObjectLoader, ConvertToLinux_Nix)
	{
		SharedObjectLoader sol;

		auto out = sol.convertToLinuxModule("libuicore.so");
		ASSERT_STREQ("libuicore.so", out.c_str());
	}
}
