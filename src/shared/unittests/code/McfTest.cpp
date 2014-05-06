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
#include "mcfcore/MCFMain.h"

class MCFTestFixture : public ::testing::Test
{
public:
	void SetUp() override
	{
		UTIL::FS::delFolder("unit_test\\mcftest");
		UTIL::FS::recMakeFolder("unit_test\\mcftest");
	}

	void TearDown() override
	{
		UTIL::FS::delFolder("unit_test\\mcftest");
	}

	void createFile(const char* szFileName, const char* szData)
	{
		UTIL::FS::recMakeFolder(UTIL::FS::PathWithFile(szFileName));

		UTIL::FS::FileHandle fh(szFileName, UTIL::FS::FILE_WRITE);
		fh.write(szData, sizeof(szData));
	}

	void copyFile(const char* szSource, const char* szDest)
	{
		UTIL::FS::recMakeFolder(UTIL::FS::PathWithFile(szSource));
		UTIL::FS::recMakeFolder(UTIL::FS::PathWithFile(szDest));
		UTIL::FS::copyFile(szSource, szDest);
	}

	void compareFolders(UTIL::FS::Path p1, UTIL::FS::Path p2)
	{
		std::vector<UTIL::FS::Path> vOriginal;
		std::vector<UTIL::FS::Path> vMerged;

		UTIL::FS::getAllFiles(p1, vOriginal, nullptr);
		UTIL::FS::getAllFiles(p2, vMerged, nullptr);

		ASSERT_EQ(vOriginal.size(), vMerged.size());

		for (size_t x = 0; x<vOriginal.size(); ++x)
		{
			ASSERT_EQ(vOriginal[x].getFile(), vMerged[x].getFile());

			std::string strMd5O = UTIL::MISC::hashFile(vOriginal[x].getFullPath());
			std::string strMd5M = UTIL::MISC::hashFile(vMerged[x].getFullPath());

			ASSERT_EQ(strMd5O, strMd5M);
		}

		vOriginal.clear();
		vMerged.clear();

		UTIL::FS::getAllFolders(p1, vOriginal);
		UTIL::FS::getAllFolders(p2, vMerged);

		ASSERT_EQ(vOriginal.size(), vMerged.size());

		for (size_t x = 0; x < vOriginal.size(); ++x)
		{
			ASSERT_EQ(vOriginal[x].getLastFolder(), vMerged[x].getLastFolder());
			compareFolders(vOriginal[x], vMerged[x]);
		}
	}
};


TEST_F(MCFTestFixture, MCF_MergePatch)
{
	createFile("unit_test\\mcftest\\ver1\\a.txt", "123");
	createFile("unit_test\\mcftest\\ver1\\b\\a.txt", "456");
	createFile("unit_test\\mcftest\\ver1\\c\\a.txt", "789");

	copyFile("unit_test\\mcftest\\ver1\\a.txt",		"unit_test\\mcftest\\ver2\\d.txt");
	createFile(										"unit_test\\mcftest\\ver2\\b\\a.txt", "abc");
	copyFile("unit_test\\mcftest\\ver1\\c\\a.txt",	"unit_test\\mcftest\\ver2\\c\\a.txt");

	copyFile("unit_test\\mcftest\\ver1\\a.txt", "unit_test\\mcftest\\ver2_patch\\d.txt");
	createFile("unit_test\\mcftest\\ver2_patch\\b\\a.txt", "abc");

	McfHandle mcf1;
	mcf1->setFile("unit_test\\mcftest\\ver1_full.mcf");
	mcf1->parseFolder("unit_test\\mcftest\\ver1");
	mcf1->hashFiles();
	mcf1->saveMCF();

	{
		McfHandle mcf2;
		mcf2->setFile("unit_test\\mcftest\\ver2_patch.mcf");
		mcf2->parseFolder("unit_test\\mcftest\\ver2");
		mcf2->hashFiles();
		mcf2->makePatch(mcf1.handle());
		mcf2->saveMCF();
		mcf2->saveFiles("unit_test\\mcftest\\patch");
	}

	compareFolders(UTIL::FS::Path("unit_test\\mcftest\\ver2_patch"), UTIL::FS::Path("unit_test\\mcftest\\patch"));

	{
		McfHandle mcf3;
		mcf3->setFile("unit_test\\mcftest\\ver2_patch.mcf");;
		mcf3->parseMCF();
		mcf1->makeFullFile(mcf3.handle(), "unit_test\\mcftest\\ver2_full.mcf");
	}

	{
		McfHandle mcf4;
		mcf4->setFile("unit_test\\mcftest\\ver2_full.mcf");
		mcf4->parseMCF();
		mcf4->saveFiles("unit_test\\mcftest\\merged");
	}

	compareFolders(UTIL::FS::Path("unit_test\\mcftest\\ver2"), UTIL::FS::Path("unit_test\\mcftest\\merged"));
}