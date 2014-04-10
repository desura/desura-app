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

#include "Common.h"
#include "gcJSBranchInfo.h"
#include "MainApp.h"

#include "usercore/ItemInfoI.h"
#include "usercore/BranchInfoI.h"

template <>
void FromJSObject<UserCore::Item::BranchInfoI*>(UserCore::Item::BranchInfoI* &branch, JSObjHandle& arg)
{
	if (arg->isObject())
		branch = arg->getUserObject<UserCore::Item::BranchInfoI>();
	else
		branch = nullptr;
}

REGISTER_JSEXTENDER(DesuraJSBranchInfo);


DesuraJSBranchInfo::DesuraJSBranchInfo() : DesuraJSBase("branch", "native_binding_branchinfo.js")
{
	REG_SIMPLE_JS_OBJ_FUNCTION( isValidBranch, DesuraJSBranchInfo );
	
	REG_SIMPLE_JS_OBJ_FUNCTION( getItemId, DesuraJSBranchInfo );
	REG_SIMPLE_JS_OBJ_FUNCTION( getBranchId, DesuraJSBranchInfo );
	REG_SIMPLE_JS_OBJ_FUNCTION( getBranchFlags, DesuraJSBranchInfo );
	REG_SIMPLE_JS_OBJ_FUNCTION( getBranchName, DesuraJSBranchInfo );
	REG_SIMPLE_JS_OBJ_FUNCTION( getBranchCost, DesuraJSBranchInfo );
	REG_SIMPLE_JS_OBJ_FUNCTION( getBranchPreorderDate, DesuraJSBranchInfo );

	REG_SIMPLE_JS_OBJ_FUNCTION( isWindows, DesuraJSBranchInfo );
	REG_SIMPLE_JS_OBJ_FUNCTION( isLinux, DesuraJSBranchInfo );
	REG_SIMPLE_JS_OBJ_FUNCTION( is32Bit, DesuraJSBranchInfo );
	REG_SIMPLE_JS_OBJ_FUNCTION( is64Bit, DesuraJSBranchInfo );

	m_uiIsValidHash = UTIL::MISC::RSHash_CSTR("isValidBranch");
}

bool DesuraJSBranchInfo::preExecuteValidation(const char* function, uint32 functionHash, JSObjHandle object, size_t argc, JSObjHandle* argv)
{
	if (functionHash == m_uiIsValidHash)
		return true;

	if (argc == 0)
		return false;

	UserCore::Item::BranchInfoI* branch = nullptr;
	FromJSObject(branch, argv[0]);

	return isValidBranch(branch);
}

bool DesuraJSBranchInfo::isValidBranch(UserCore::Item::BranchInfoI* branch)
{
	return branch?true:false;
}

int32 DesuraJSBranchInfo::getBranchFlags(UserCore::Item::BranchInfoI* branch)
{
	return branch->getFlags();
}

gcString DesuraJSBranchInfo::getItemId(UserCore::Item::BranchInfoI* branch)
{
	return branch->getItemId().toString();
}

gcString DesuraJSBranchInfo::getBranchId(UserCore::Item::BranchInfoI* branch)
{
	return gcString("{0}", branch->getBranchId());
}

gcString DesuraJSBranchInfo::getBranchName(UserCore::Item::BranchInfoI* branch)
{
	return branch->getName();
}

gcString DesuraJSBranchInfo::getBranchCost(UserCore::Item::BranchInfoI* branch)
{
	return branch->getCost();
}

gcString DesuraJSBranchInfo::getBranchPreorderDate(UserCore::Item::BranchInfoI* branch)
{
	return branch->getPreOrderExpDate();
}

bool DesuraJSBranchInfo::isWindows(UserCore::Item::BranchInfoI* branch)
{
	return branch->isWindows();
}

bool DesuraJSBranchInfo::isLinux(UserCore::Item::BranchInfoI* branch)
{
	return branch->isLinux();
}

bool DesuraJSBranchInfo::is32Bit(UserCore::Item::BranchInfoI* branch)
{
	return branch->is32Bit();
}

bool DesuraJSBranchInfo::is64Bit(UserCore::Item::BranchInfoI* branch)
{
	return branch->is64Bit();
}
