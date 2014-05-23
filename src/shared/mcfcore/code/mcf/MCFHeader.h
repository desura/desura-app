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


#ifndef MCF_HEADER_H
#define MCF_HEADER_H

#include "mcfcore/MCFHeaderI.h"
#include "umcf/UMcfHeader.h"


/// \brief MCFCore namespace houses all functionality relating to MCF files.
namespace MCFCore
{
	//! MCFHeader represents the binary header section of a MCF file which holds
	//! the key information about the content within the MCF
	//! 
	//! If you make changes here make sure you update UMCFHeader as well.
	class MCFHeader : public UMcfHeader
	{
	public:
		//! Default Constructor
		MCFHeader();

		//! Copy Constructor
		MCFHeader(MCFHeaderI* head);

		//! Load from string Constructor
		MCFHeader(const char* string);


		//! Gets the size of the header for in a MCF file always the newest version
		//!
		//! @return size of the header in a MCF file
		//! 
		static uint8 getSizeS(){ return MCF_HEADERSIZE_V2; }


		//! Saves the MCF header to a MCF file
		//!
		//! @param hFile File Handle to save the header to
		//!
		void saveToFile(UTIL::FS::FileHandle& hFile);

		//! Reads the MCF header from a MCF file
		//!
		//! @param hFile File Handle to read the header from
		//!
		void readFromFile(UTIL::FS::FileHandle& hFile);
	};
}



#endif
