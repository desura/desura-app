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

#ifndef DESURA_SHAREDOBJECTLOADER_H
#define DESURA_SHAREDOBJECTLOADER_H
#ifdef _WIN32
#pragma once
#endif

typedef void* (*FactoryFn)(const char*);

#ifdef WIN32
	#include <Windows.h>
#else
	#include <dlfcn.h>
#endif

#include "util/UtilOs.h"

class SharedObjectLoader
{
public:
#if defined(NIX) || defined(MACOS)
	typedef void* SOHANDLE;
#else
	typedef HINSTANCE SOHANDLE;
#endif

	SharedObjectLoader()
	: m_hHandle(nullptr)
	, m_bHasFailed(false)
	{
	}

	SharedObjectLoader(const SharedObjectLoader& sol)
	: m_hHandle(sol.m_hHandle)
	, m_bHasFailed(sol.m_bHasFailed)
	{
		sol.m_hHandle = nullptr;
		sol.m_bHasFailed = false;
	}

	~SharedObjectLoader()
	{
		unload();
	}

    void dontUnloadOnDelete()
    {
        m_bIgnoreUnload = true;
    }

	bool load(const char* module)
	{
		if (m_hHandle)
			unload();

		m_bHasFailed = false;

#if defined (NIX) || defined(MACOS)
		gcString strModule(convertToLinuxModule(module));
		m_hHandle = dlopen((UTIL::OS::getRuntimeLibPath() + strModule).c_str(), RTLD_NOW);

		if (!m_hHandle)
			fprintf(stderr, "%s:%d - Error loading library %s: '%s' [LD_LIBRARY_PATH=%s]\n", __FILE__, __LINE__, strModule.c_str(), dlerror(), getenv("LD_LIBRARY_PATH"));
#else
		m_hHandle = LoadLibraryA(module);
#endif

		return m_hHandle?true:false;
	}

	void unload()
	{
		if (!m_hHandle)
			return;

        if (!m_bIgnoreUnload)
        {
#if defined(NIX) || defined(MACOS)
            if (dlclose(m_hHandle) != 0)
                printf("%s:%d - Error unloading library: '%s'\n", __FILE__, __LINE__, dlerror());
#else
            FreeLibrary(m_hHandle);
#endif
        }

		m_hHandle = nullptr;
	}

	template <typename T>
	T getFunction(const char* functionName, bool failIfNotFound = true)
	{
		if (!m_hHandle)
			return nullptr;
#if defined(NIX) || defined(MACOS)
		char* error;
		T fun = (T)dlsym(m_hHandle, functionName);
		if ((error = dlerror()) != nullptr)
		{
			printf("%s:%d - Error getting function %s: '%s'\n", __FILE__, __LINE__, functionName, dlerror());
			return nullptr;
		}

#else
		T fun = (T)GetProcAddress(m_hHandle, functionName);
#endif

		if (!fun && failIfNotFound)
			m_bHasFailed = true;

		return fun;
	}

	bool hasFailed()
	{
		return m_bHasFailed;
	}

	SOHANDLE handle()
	{
		return m_hHandle;
	}

	void resetFailed()
	{
		m_bHasFailed = false;
	}

	std::string convertToLinuxModule(const char* szModule)
	{
		gcString strModule(szModule);

		if (strModule.find(".dll") == strModule.size() - 4)
			strModule = gcString("lib") + strModule.substr(0, strModule.size() - 3) + "so";

		return strModule;
	}

	std::string convertToMacModule(const char* szModule)
	{
		gcString strModule(szModule);

		if (strModule.find(".dll") == strModule.size() - 4)
			strModule = gcString("lib") + strModule.substr(0, strModule.size() - 3) + "dylib";

		return strModule;
	}

private:
    bool m_bIgnoreUnload = false;
	mutable bool m_bHasFailed;
	mutable SOHANDLE m_hHandle;
};

#endif //DESURA_SHAREDOBJECTLOADER_H
