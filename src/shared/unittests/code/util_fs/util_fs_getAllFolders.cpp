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

