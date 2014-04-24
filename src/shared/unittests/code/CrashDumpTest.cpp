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
#include "CrashuploaderShared.cpp"

#include <gtest/gtest.h>

#include "Tracer.cpp"

namespace UnitTest
{
#ifdef WITH_TRACING

	TEST(DumpTracerToFile, check)
	{
		static const std::vector<std::string> vExpectedOut =
		{
			"[",
			"\t{ \"message\": \"first\" },",
			"\t{ \"message\": \"second\" },",
			"\t{ \"message\": \"third\" }",
			"]",
		};

		TracerStorage tracer(L"DumpTracerToFile_TEST");

		gcString sharedMem(tracer.getSharedMemName());

		tracer.trace("first", nullptr);
		tracer.trace("second", nullptr);
		tracer.trace("third", nullptr);

		std::string strOutput;
		std::function<void(const char*, uint32)> write = [&strOutput](const char* szData, uint32 nSize)
		{
			strOutput += std::string(szData, nSize);
		};

		DumpTracerToFile(sharedMem, write);

		std::vector<std::string> vTokens;
		UTIL::STRING::tokenize(strOutput, vTokens, "\n");

		ASSERT_EQ(vExpectedOut.size(), vTokens.size());

		auto x = 0;
		for (auto e : vExpectedOut)
		{
			ASSERT_EQ(e, vTokens[x]);
			x++;
		}
	}

	TEST(DumpTracerToFile, invalidInputCheck)
	{
		static const std::vector<std::string> vExpectedOut =
		{
			"[",
			"\t{ \"message\": \"first \\\"out\\\"\", \"name with space\": \"\\n\\t\" }",
			"]",
		};

		TracerStorage tracer(L"DumpTracerToFile_TEST");

		gcString sharedMem(tracer.getSharedMemName());

		std::map<std::string, std::string> vArgs;
		vArgs["name with space"] = "\n\t";

		tracer.trace("first \"out\"", &vArgs);

		std::string strOutput;
		std::function<void(const char*, uint32)> write = [&strOutput](const char* szData, uint32 nSize)
		{
			strOutput += std::string(szData, nSize);
		};

		DumpTracerToFile(sharedMem, write);

		std::vector<std::string> vTokens;
		UTIL::STRING::tokenize(strOutput, vTokens, "\n");

		ASSERT_EQ(vExpectedOut.size(), vTokens.size());

		auto x = 0;
		for (auto e : vExpectedOut)
		{
			ASSERT_EQ(e, vTokens[x]);
			x++;
		}
	}

#endif
}