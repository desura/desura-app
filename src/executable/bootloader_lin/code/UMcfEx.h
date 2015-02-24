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
#ifndef MC_UMCF_EX_H
#define MC_UMCF_EX_H
#ifdef _WIN32
#pragma once
#endif

#include "Common.h"
#include "umcf/UMcf.h"

class UMcfEx : public UMcf
{
public:
	//! Downloads update info from web
	void getUpdateInfo(bool save = false);

	//! downloads the MCF ready to install.
	void downloadMcf();
	
	//! Checks the mcf for errors
	void checkMcf();
	
	//! Deletes the mcf file
	void deleteMcf();
};

#endif //MC_UMCF_EX_H
