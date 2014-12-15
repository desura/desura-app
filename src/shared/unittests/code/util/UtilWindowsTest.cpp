/*
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
