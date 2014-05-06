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


#ifndef DESURA_CDKEYMANAGERI_H
#define DESURA_CDKEYMANAGERI_H
#ifdef _WIN32
#pragma once
#endif

namespace UserCore
{
	namespace Misc
	{
		class CDKeyCallBackI : public gcRefBase
		{
		public:
			//! Call back when complete
			//!
			//! @param id Item id
			//! @param cdkey Item cd key
			//!
			virtual void onCDKeyComplete(DesuraId id, gcString &cdKey) = 0;

			//! Call back when error occures
			//!
			//! @param id Item id
			//! @param e Error that occured
			//!
			virtual void onCDKeyError(DesuraId id, gcException& e) = 0;
		};

	}
	class CDKeyManagerI : public gcRefBase
	{
	public:
		//! Gets the cd key for currently installed branch of an item
		//!
		//! @param id Item id
		//! @param callback Callback to use when complete
		//!
		virtual void getCDKeyForCurrentBranch(DesuraId id, gcRefPtr<UserCore::Misc::CDKeyCallBackI> &pCallback) = 0;

		//! Cancels a request to get a cd key
		//!
		//! @param id Item id
		//! @param callback Request callback used in original request
		//!
		virtual void cancelRequest(DesuraId id, gcRefPtr<UserCore::Misc::CDKeyCallBackI> &pCallback) = 0;

		//! Does the current branch require a cd key
		//!
		//! @return true of false
		//!
		virtual bool hasCDKeyForCurrentBranch(DesuraId id) = 0;
	};
}

#endif //DESURA_CDKEYMANAGERI_H
