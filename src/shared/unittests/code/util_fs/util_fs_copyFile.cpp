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

// interface: void copyFile(Path src, Path dest);
//            void copyFile(std::string src, std::string dest)

// set up test env for util_fs testing
#define TEST_DIR "copyFile"
#include "util_fs/testFunctions.cpp"

#include "Common.h"
#include "util/UtilFs.h"
#include "util/UtilFsPath.h"
using namespace UTIL::FS;

#define SRC (getTestDirectory()/"0"/"1.txt")
#define DES1 (getTestDirectory()/"0"/"jsdakl1")
#define DES2 (getTestDirectory()/"0"/"jsdakl2")

namespace UnitTest
{
	TEST_F(FSTestFixture, copyFile_Path)
	{
		Path src(SRC.string(), "", false);
		Path des(DES1.string(), "", false);

		ASSERT_TRUE(!fs::exists(DES1));
		copyFile(src, des);
		ASSERT_TRUE(fs::exists(DES1));
		ASSERT_EQ_FILES(SRC, DES1);
	}

	TEST_F(FSTestFixture, copyFile_string)
	{
		ASSERT_TRUE(!fs::exists(DES2));
		copyFile(SRC.string(), DES2.string());
		ASSERT_TRUE(fs::exists(DES2));
		ASSERT_EQ_FILES(SRC, DES2);
	}
}

