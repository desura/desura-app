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
#include "util/UtilFs.h"

using namespace UTIL::STRING;

namespace UnitTest
{
	TEST(sanitizeFilePath, slash1)
	{
		std::string str = "/hi/ho/hihihi\\";

		ASSERT_STREQ("/hi/ho/hihihi/", sanitizeFilePath(str, '/').c_str());
	}

	TEST(sanitizeFilePath, slash_with_dot)
	{
		std::string str = "/hi/./ho/hihihi\\";

		ASSERT_STREQ("/hi/./ho/hihihi/", sanitizeFilePath(str, '/').c_str());
	}

	TEST(sanitizeFilePath, slash_with_dots)
	{
		std::string str = "/hi/../ho/hihihi\\";

		ASSERT_STREQ("/hi/../ho/hihihi/", sanitizeFilePath(str, '/').c_str());
	}
}

