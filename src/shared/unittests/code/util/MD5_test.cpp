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
#include "Common.h"
#include "util/MD5Progressive.h"

#define EMPTY_MD5 "d41d8cd98f00b204e9800998ecf8427e"

static const char TEST_CSTRING[] = "this is a little test";
#define TEST_CSTRING_SIZE sizeof(TEST_CSTRING)
#define TEST_MD5_RESULT "419d085dcebc10a7e6d7115216e9820d"

namespace UnitTest
{
	TEST(MD5Progressive, cons)
	{
		MD5Progressive md5;
		ASSERT_STREQ(md5.finish().c_str(), EMPTY_MD5);
	}

	TEST(MD5Progressive, update)
	{
		MD5Progressive md5;
		md5.update(TEST_CSTRING, TEST_CSTRING_SIZE - 1);
		ASSERT_STREQ(md5.finish().c_str(), TEST_MD5_RESULT);
	}
}

