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

// interface: void getAllFolders(Path path, std::vector<Path> &outList)

#define TEST_DIR "getAllFolders"
#include "util_fs/testFunctions.cpp"

#include "Common.h"
#include "util/UtilFs.h"
#include "util/UtilFsPath.h"
using namespace UTIL::FS;

namespace UnitTest
{
	TEST_F(FSTestFixture, getAllFolders1)
	{
		Path path(getTestDirectory().string(), "", false);
		std::vector<Path> content;

		getAllFolders(path, content);

		ASSERT_EQ(content.size(), 1);
		ASSERT_EQ(content[0].getFullPath(), (getTestDirectory() / "0").string());
	}

	TEST_F(FSTestFixture, getAllFolder2)
	{
		Path path((getTestDirectory() / "0").string(), "", false);
		std::vector<Path> content;

		getAllFolders(path, content);

		ASSERT_EQ(content.size(), 0);
	}
}

