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
#ifndef TEST_DIR
#error you have to define TEST_DIR first
#endif

#include "Common.h"

#include <locale>
#include <codecvt>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace fs = boost::filesystem;

namespace UnitTest
{
	static const char* UNICODE_EXAMPLE_FILE = "ʨʬжװᇪ";

	#define ASSERT_EQ_FILES(/* const fs::path& */ file1, /* const fs::path& */ file2) \
	{ \
		fs::ifstream file1_stream(file1); \
		fs::ifstream file2_stream(file2); \
	 \
		std::string file1_string; \
		std::string file2_string; \
	 \
		for (std::string file1_string_buf; std::getline(file1_stream, file1_string_buf);) \
			file1_string += file1_string_buf; \
		for (std::string file2_string_buf; std::getline(file2_stream, file2_string_buf);) \
			file2_string += file2_string_buf; \
	 \
		 ASSERT_STREQ(file1_string.c_str(), file2_string.c_str()); \
	 \
		file1_stream.close(); \
		file2_stream.close(); \
	}


	class FSTestFixture : public ::testing::Test
	{
	public:
		void SetUp() override
		{
			createTestDirectory();
		}

		void TearDown() override
		{
			deleteTestDirectory();
		}

	protected:
		void createTestDirectory()
		{
			fs::create_directories(getTestDirectory());
			fillWithTestData();
		}

		void deleteTestDirectory()
		{
			fs::remove_all(getTestDirectory());
		}

		const fs::path& getTestDirectory()
		{
			static const fs::path TEST_DIR_ROOT = fs::current_path() / "unit_test" / "util_fs" / TEST_DIR;
			return TEST_DIR_ROOT;
		}

		/*
		* this will create the following directory tree:
		*  ./0/0
		*  ./0/1.txt
		*  ./0/2.png
		*  ./0/UNICODE_EXAMPLE_FILE
		*/
		void fillWithTestData()
		{
			const fs::path &testDir = getTestDirectory();
			fs::create_directories(testDir);

			std::vector<std::string> firstLevel;
			firstLevel.push_back("0");

			std::vector<std::string> secondLevel;
			secondLevel.push_back("0");
			secondLevel.push_back("1.txt");
			secondLevel.push_back("2.png");
			secondLevel.push_back( UNICODE_EXAMPLE_FILE );

			for (const std::string& i : firstLevel)
			{
				fs::create_directory(testDir / i);

				for (const std::string& j : secondLevel)
				{
					fs::path newFilePath = testDir / i;
					UTIL::FS::Path filePath( newFilePath.string(), j, false );
					std::filebuf fb;
					std::wstring nameW = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes( filePath.getFullPath() );
					fb.open( nameW, std::ios::out );
					std::ostream os( &fb );
					os << "this is a test file" << std::endl;
					fb.close();
				}
			}
		}
	};
}
