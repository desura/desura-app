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

using namespace UTIL::FS;

namespace UnitTest
{
	TEST(Util_Fs_Path, startsWith_same)
	{
		Path a("C:\\abcd\\1234");
		Path b("C:\\abcd\\1234");

		ASSERT_TRUE(a.startsWith(b));
		ASSERT_TRUE(b.startsWith(a));
	}

	TEST(Util_Fs_Path, startsWith_sameWithOneFile)
	{
		Path a("C:\\abcd\\1234");
		Path b("C:\\abcd\\1234", "someFile");

		ASSERT_TRUE(a.startsWith(b));
		ASSERT_TRUE(b.startsWith(a));
	}

	TEST(Util_Fs_Path, startsWith_sameWithDiffFile)
	{
		Path a("C:\\abcd\\1234", "fileSome");
		Path b("C:\\abcd\\1234", "someFile");

		ASSERT_TRUE(a.startsWith(b));
		ASSERT_TRUE(b.startsWith(a));
	}

	TEST(Util_Fs_Path, startsWith_diffDrive)
	{
		Path a("C:\\abcd\\1234");
		Path b("D:\\abcd\\1234");

		ASSERT_FALSE(a.startsWith(b));
		ASSERT_FALSE(b.startsWith(a));
	}

	TEST(Util_Fs_Path, startsWith_diffPath1)
	{
		Path a("C:\\abcd\\1234");
		Path b("C:\\abc1\\1234");

		ASSERT_FALSE(a.startsWith(b));
		ASSERT_FALSE(b.startsWith(a));
	}

	TEST(Util_Fs_Path, startsWith_diffPath2)
	{
		Path a("C:\\abcd\\1234");
		Path b("C:\\abcd\\123a");

		ASSERT_FALSE(a.startsWith(b));
		ASSERT_FALSE(b.startsWith(a));
	}
}
