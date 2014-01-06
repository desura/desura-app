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
#include "gcJSUserInfo.h"
#include "MainApp.h"

REGISTER_JSEXTENDER(DesuraJSUserInfo);

DesuraJSUserInfo::DesuraJSUserInfo() : DesuraJSBase("user", "native_binding_userinfo.js")
{
	REG_SIMPLE_JS_FUNCTION( isUserAdmin, DesuraJSUserInfo );
	REG_SIMPLE_JS_VOIDFUNCTION( forceUpdatePoll, DesuraJSUserInfo );
	REG_SIMPLE_JS_FUNCTION( getUserUserId, DesuraJSUserInfo );
	REG_SIMPLE_JS_FUNCTION( getUserAvatar, DesuraJSUserInfo );
	REG_SIMPLE_JS_FUNCTION( getUserProfileUrl, DesuraJSUserInfo );
	REG_SIMPLE_JS_FUNCTION( getUserProfileEditUrl, DesuraJSUserInfo );
	REG_SIMPLE_JS_FUNCTION( getUserUserNameId, DesuraJSUserInfo );
	REG_SIMPLE_JS_FUNCTION( getUserUserName, DesuraJSUserInfo );
	REG_SIMPLE_JS_FUNCTION( getUserPmCount, DesuraJSUserInfo );
	REG_SIMPLE_JS_FUNCTION( getUserUpCount, DesuraJSUserInfo );
	REG_SIMPLE_JS_FUNCTION( getUserCartCount, DesuraJSUserInfo );
}

bool DesuraJSUserInfo::isUserAdmin()
{
	return GetUserCore()->isAdmin();
}

void DesuraJSUserInfo::forceUpdatePoll()
{
	GetUserCore()->forceUpdatePoll();
}

int32 DesuraJSUserInfo::getUserUserId()
{
	return GetUserCore()->getUserId();
}

gcString DesuraJSUserInfo::getUserAvatar()
{
	return GetUserCore()->getAvatar();
}

gcString DesuraJSUserInfo::getUserProfileUrl()
{
	return GetUserCore()->getProfileUrl();
}

gcString DesuraJSUserInfo::getUserProfileEditUrl()
{
	return GetUserCore()->getProfileEditUrl();
}

gcString DesuraJSUserInfo::getUserUserNameId()
{
	return GetUserCore()->getUserNameId();
}

gcString DesuraJSUserInfo::getUserUserName()
{
	return GetUserCore()->getUserName();
}

int32 DesuraJSUserInfo::getUserPmCount()
{
	return GetUserCore()->getPmCount();
}

int32 DesuraJSUserInfo::getUserUpCount()
{
	return GetUserCore()->getUpCount();
}

int32 DesuraJSUserInfo::getUserCartCount()
{
	return GetUserCore()->getCartCount();
}

