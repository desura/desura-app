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


#ifndef DESURA_NEWSITEM_H
#define DESURA_NEWSITEM_H
#ifdef _WIN32
#pragma once
#endif

namespace UserCore
{
	namespace Misc
	{

		//! Container class that stores information for one news item
		class NewsItem : public gcRefBase
		{
		public:
			//! Constructor
			//!
			//! @param i News item id
			//! @param c News item category
			//! @param t Title
			//! @param u Url
			//!
			NewsItem(uint32 i, uint8 c, const char* t, const char* u)
			{
				id = i;
				cat = c;
				szTitle = gcString(t);
				szUrl = gcString(u);
				hasBeenShown = false;
			}

			//! Copy Constructor
			//!
			//! @param item In NewsItem
			NewsItem(NewsItem* item)
			{
				id = 0;
				cat = 0;

				if (item)
				{
					id = item->id;
					cat = item->cat;
					szTitle = item->szTitle;
					szUrl = item->szUrl;

					hasBeenShown = item->hasBeenShown;
				}
			}

			uint32 id;
			uint8 cat;
			gcString szTitle;
			gcString szUrl;

			bool hasBeenShown;

			gc_IMPLEMENT_REFCOUNTING(NewsItem)
		};

	}
}

#endif //DESURA_NEWSITEM_H
