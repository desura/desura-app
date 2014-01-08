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



class CreateMCF : public UtilFunction
{
public:
	virtual uint32 getNumArgs()
	{
		return 2;
	}

	virtual const char* getArgDesc(size_t index)
	{
		if (index == 0)
			return "Src Folder";

		return "Dest Mcf";
	}

	virtual const char* getFullArg()
	{
		return "create";
	}

	virtual const char getShortArg()
	{
		return 'c';
	}

	virtual const char* getDescription()
	{
		return "Creates a new mcf from a folder";
	}

	virtual int performAction(std::vector<std::string> &args)
	{
		MCFCore::MCFI* mcfHandle = mcfFactory();
		mcfHandle->getProgEvent() += delegate((UtilFunction*)this, &UtilFunction::printProgress);
		mcfHandle->getErrorEvent() += delegate((UtilFunction*)this, &UtilFunction::mcfError);

		mcfHandle->setFile(args[1].c_str());
		mcfHandle->parseFolder(args[0].c_str());
		mcfHandle->saveMCF();

		mcfDelFactory(mcfHandle);
		return 0;
	}
};

class CreateMCFDiff : public UtilFunction
{
public:
	virtual uint32 getNumArgs()
	{
		return 3;
	}

	virtual const char* getArgDesc(size_t index)
	{
		if (index == 0)
			return "vA Mcf";
		else if (index == 1)
			return "vB Mcf";

		return "Out Mcf";
	}

	virtual const char* getFullArg()
	{
		return "creatediff";
	}

	virtual const char getShortArg()
	{
		return 'd';
	}

	virtual const char* getDescription()
	{
		return "Creates a new mcf from a folder and the diffs as well";
	}

	virtual int performAction(std::vector<std::string> &args)
	{
		MCFCore::MCFI* mcfA = mcfFactory();
		MCFCore::MCFI* mcfB = mcfFactory();

		mcfA->setFile(args[0].c_str());
		mcfA->parseMCF();

		mcfB->setFile(args[1].c_str());
		mcfB->parseMCF();

		mcfB->createCourgetteDiffs(mcfA, args[2].c_str());

		mcfDelFactory(mcfA);
		mcfDelFactory(mcfB);
		return 0;
	}
};

class CreateNCMCF : public UtilFunction
{
public:
	virtual uint32 getNumArgs()
	{
		return 2;
	}

	virtual const char* getArgDesc(size_t index)
	{
		if (index == 0)
			return "Src Folder";

		return "Dest Mcf";
	}

	virtual const char* getFullArg()
	{
		return "createnc";
	}

	virtual const char getShortArg()
	{
		return 'n';
	}

	virtual const char* getDescription()
	{
		return "Creates a new mcf that is not compressed from a folder";
	}

	virtual int performAction(std::vector<std::string> &args)
	{

		MCFCore::MCFI* mcfHandle = mcfFactory();
		mcfHandle->getProgEvent() += delegate((UtilFunction*)this, &UtilFunction::printProgress);
		mcfHandle->getErrorEvent() += delegate((UtilFunction*)this, &UtilFunction::mcfError);

		mcfHandle->setWorkerCount(2);
		mcfHandle->setFile(args[1].c_str());
		mcfHandle->disableCompression();

		mcfHandle->parseFolder(args[0].c_str());
		mcfHandle->saveMCF();


		mcfDelFactory(mcfHandle);
		return 0;
	}
};


REG_FUNCTION(CreateMCFDiff)
REG_FUNCTION(CreateMCF)
REG_FUNCTION(CreateNCMCF)
