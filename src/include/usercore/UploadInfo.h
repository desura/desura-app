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


#ifndef DESURA_UPLOADINFO_H
#define DESURA_UPLOADINFO_H
#ifdef _WIN32
#pragma once
#endif

namespace UserCore
{
namespace Misc
{

//! Holds information about upload progress
class UploadInfo
{
public:
	UploadInfo()
	{
		hour = 0;
		min = 0;
		rate = 0;
		percent = 0;
		num = 0;
		milestone = false;
		paused = false;
		totalAmmount = 0;
		doneAmmount = 0;
	}

	UploadInfo(UploadInfo* i)
	{
		min = 0;
		hour = 0;
		rate = 0;
		percent = 0;
		num = 0;
		paused = false;
		milestone = false;
		totalAmmount = 0;
		doneAmmount = 0;

		if (i)
		{
			min = i->min;
			hour= i->hour;
			rate= i->rate;
			percent = i->percent;
			num = i->num;
			milestone = i->milestone;
			paused = i->paused;
			msg = i->msg;
			totalAmmount = i->totalAmmount;
			doneAmmount = i->doneAmmount;
		}
	}

	uint8 min;
	uint8 hour;
	uint32 rate;
	uint8 percent;

	uint64 totalAmmount;
	uint64 doneAmmount;

	bool milestone;
	bool paused;

	int32 num;
	gcString msg;
};

}
}

#endif //DESURA_UPLOADINFO_H
