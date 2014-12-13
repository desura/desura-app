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
#include "CrashuploaderShared.cpp"

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
