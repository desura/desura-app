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
