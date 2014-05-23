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

// interface: void getAllFiles(Path path, std::vector<Path> &outList, std::vector<std::string> *extsFilter);

// set up test env for util_fs testing
#define TEST_DIR "getAllFiles"
#include "util_fs/testFunctions.cpp"
#include "helper_functions.h"

#include "Common.h"
#include "util/UtilFs.h"
#include "util/UtilFsPath.h"
using namespace UTIL::FS;

namespace UnitTest
{
	TEST_F(FSTestFixture, getAllFiles_without_extsFilter)
	{
		std::vector<Path> content;
		std::vector<Path> content0;

		Path path(getTestDirectory().string(), "", false);
		Path path0((getTestDirectory() / "0").string(), "", false);

		getAllFiles(path, content, nullptr);
		getAllFiles(path0, content0, nullptr);

		ASSERT_EQ(content.size(), 0);
		ASSERT_EQ(content0.size(), 4);


		ASSERT_TRUE(isInVector(Path((getTestDirectory() / "0" / "0").string(), "", true), content0));
		ASSERT_TRUE(isInVector(Path((getTestDirectory() / "0" / "1.txt").string(), "", true), content0));
		ASSERT_TRUE(isInVector(Path((getTestDirectory() / "0" / "2.png").string(), "", true), content0));
		ASSERT_TRUE(isInVector(Path((getTestDirectory() / "0" / UNICODE_EXAMPLE_FILE).string(), "", true), content0));
	}

	TEST_F(FSTestFixture, getAllFiles_with_extsfiler)
	{
		std::vector<Path> content;
		std::vector<std::string> filter;
		filter.push_back("txt");

		Path path((getTestDirectory() / "0").string(), "", false);
		getAllFiles(path, content, &filter);

		ASSERT_EQ(content.size(), 1);
		ASSERT_EQ(content[0].getFullPath(), (getTestDirectory() / "0" / "1.txt").string());
	}
}

