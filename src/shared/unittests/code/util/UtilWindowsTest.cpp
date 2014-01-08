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