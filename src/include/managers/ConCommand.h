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

#ifndef DESURA_CONCOMMAND_H
#define DESURA_CONCOMMAND_H
#ifdef _WIN32
#pragma once
#endif

#include "BaseManager.h"

typedef void (*ConCommandCallBackFn)(std::vector<gcString> &vArgList);

#define CONCOMMAND( name, printname )		void name##_cc_func(std::vector<gcString> &vArgList);				\
											ConCommand name( printname, & name##_cc_func);		\
											void name##_cc_func(std::vector<gcString> &vArgList)

//! ConCommand repersents a console command provided in the application console
class ConCommand : public BaseItem
{
public:
	//! @param printName Name that shows up in the console to call this by
	//! @param callBack Function to call when the con command is executed
	ConCommand(const char* printName, ConCommandCallBackFn callBack);
	~ConCommand();

	//! Call the ConCommand with arg list
	//!
	//! @param vArgList List of arguments
	//!
	void Call(std::vector<gcString> &vArgList);

	//! Same as Call
	void operator() (std::vector<gcString> &vArgList);

	//! Same as Call with empty list
	void operator() ();

private:
	//! Callback function
	ConCommandCallBackFn m_pCallBack;

	//! Has this ConCommand been registered with the manager
	bool m_bReg;
};

#endif //DESURA_CONCOMMAND_H
