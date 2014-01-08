/*
Desura is the leading indie game distribution platform
Copyright (C) 2013 Mark Chandler (Desura Net Pty Ltd)

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
#include "UtilFunction.h"

std::vector<UtilFunction*> *gs_FunctionList = nullptr; 

void AddFunction(UtilFunction* function)
{
	if (!gs_FunctionList)
		gs_FunctionList = new std::vector<UtilFunction*>();

	gs_FunctionList->push_back(function);
}

void GetFunctionList(std::vector<UtilFunction*> &list)
{
	list = *gs_FunctionList;
}


UtilFunction::UtilFunction()
{
	s_uiLastProg = -1;
}

void UtilFunction::printProgress(MCFCore::Misc::ProgressInfo& prog)
{
	if (s_uiLastProg == prog.percent)
		return;

	s_uiLastProg = prog.percent;

	printf("\r[");

	size_t tot = (prog.percent/2);

	for (size_t x=0; x<50; x++)
	{
		if (x<tot)
			printf("=");
		else
			printf(" ");
	}

	printf("] %u%%", prog.percent);
}


void UtilFunction::mcfError(gcException& e)
{
	m_LastException = e;

	gcString err("{0}", e);
	printf("\nMCF Error: %s\n", err.c_str());
}

void UtilFunction::checkException()
{
	if (m_LastException.getErrId() != 1)
		throw m_LastException;
}
