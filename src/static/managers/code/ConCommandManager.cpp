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

#include "Common.h"
#include "ConCommandManager.h"

ConsoleCommandManager* g_pConComMang = nullptr;


void InitConComManger()
{
	if (!g_pConComMang)
		g_pConComMang = new ConsoleCommandManager();
}

void DestroyConComManager()
{
	safe_delete(g_pConComMang);
}


ConsoleCommandManager::ConsoleCommandManager() : BaseManager()
{

}

ConsoleCommandManager::~ConsoleCommandManager()
{
}

bool ConsoleCommandManager::RegCCom(const gcRefPtr<ConCommand> &var)
{
	auto temp = findItem(var->getName());

	if (temp)
		return false;

	addItem(var);
	return true;
}

//if this screws up its too late any way
void  ConsoleCommandManager::UnRegCCom(const gcRefPtr<ConCommand> &var)
{
	removeItem(var->getName());
}

gcRefPtr<ConCommand> ConsoleCommandManager::findCCommand(const char* name)
{
	return findItem(name);
}

void ConsoleCommandManager::getConCommandList(std::vector<gcRefPtr<ConCommand>> &vList)
{
	for (uint32 x=0; x<getCount(); x++)
		vList.push_back(getItem(x));
}
