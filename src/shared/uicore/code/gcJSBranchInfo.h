/*
Copyright (C) 2011 Mark Chandler (Desura Net Pty Ltd)
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

#ifndef DESURA_GCJSBRANCHINFO_H
#define DESURA_GCJSBRANCHINFO_H
#ifdef _WIN32
#pragma once
#endif


#include "gcJSBase.h"

namespace UserCore
{
	namespace Item
	{
		class ItemInfoI;
		class BranchInfoI;
	}
}


class DesuraJSBranchInfo : public DesuraJSBase<DesuraJSBranchInfo>
{
public:
	DesuraJSBranchInfo();
	virtual bool preExecuteValidation(const char* function, uint32 functionHash, JSObjHandle object, size_t argc, JSObjHandle* argv);

private:
	bool isValidBranch(gcRefPtr<UserCore::Item::BranchInfoI> branch);
	int32 getBranchFlags(gcRefPtr<UserCore::Item::BranchInfoI> branch);

	gcString getItemId(gcRefPtr<UserCore::Item::BranchInfoI> branch);
	gcString getBranchId(gcRefPtr<UserCore::Item::BranchInfoI> branch);
	gcString getBranchName(gcRefPtr<UserCore::Item::BranchInfoI> branch);
	gcString getBranchCost(gcRefPtr<UserCore::Item::BranchInfoI> branch);
	gcString getBranchPreorderDate(gcRefPtr<UserCore::Item::BranchInfoI> branch);

	bool isWindows(gcRefPtr<UserCore::Item::BranchInfoI> branch);
	bool isLinux(gcRefPtr<UserCore::Item::BranchInfoI> branch);

	bool is32Bit(gcRefPtr<UserCore::Item::BranchInfoI> branch);
	bool is64Bit(gcRefPtr<UserCore::Item::BranchInfoI> branch);


	uint32 m_uiIsValidHash;
};



#endif //DESURA_GCJSBRANCHINFO_H
