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

#ifndef DESURA_GCBUFFA_H
#define DESURA_GCBUFFA_H
#ifdef _WIN32
#pragma once
#endif

class gcBuff
{
public:
	gcBuff(uint32 size);
	gcBuff(const char* src, uint32 size);

	gcBuff(const gcBuff &buff);
	gcBuff(gcBuff &&buff);
	gcBuff(gcBuff *buff);

	~gcBuff();

	char operator[] (uint32 i) const;
	operator char*();

	void cpy(const char* src, uint32 size);

	char* c_ptr(){return m_cBuff;}
	uint32 size() const {return m_uiSize;}

	gcBuff& operator=(const gcBuff &buff);
	gcBuff& operator=(gcBuff &&buff);

private:
	char* m_cBuff = nullptr;
	uint32 m_uiSize = 0;

};

#endif //DESURA_GCBUFF_H
