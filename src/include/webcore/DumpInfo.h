/*
Desura is the leading indie game distribution platform
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)

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

#ifndef DESURA_DUMPINFO_H
#define DESURA_DUMPINFO_H
#ifdef _WIN32
#pragma once
#endif

#include "webcore/DLLVersion.h"

namespace WebCore
{
	namespace Misc
	{

		class DumpInfo
		{
		public:
			DumpInfo()
			{
				nextFileSize = 0;
				numberDone = 0;
				stop = false;
			}

			volatile bool stop;
			gcString dumpFolderPath;
			gcString user;

			Event<uint8> onProgressEvent;


			std::vector<gcString> fileStack;

			/// functions for upload
			uint32 nextFileSize;
			uint32 numberDone;

			void onProgress(Prog_s &prog)
			{
				double unitProg = (double)100/(double)fileStack.size();
				double curProg = prog.ultotal*unitProg/nextFileSize;

				uint8 porg = (uint8)(numberDone*unitProg + curProg);
				onProgressEvent(porg);

				prog.abort = stop;
			}
		};

	}
}

#endif //DESURA_DUMPINFO_H
