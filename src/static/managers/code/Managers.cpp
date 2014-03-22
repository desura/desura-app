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
#include "managers/Managers.h"

#include "ThemeManager.h"
#include "LanguageManager.h"
#include "WindowManager.h"

#include "CVarManager.h"
#include "ConCommandManager.h"

#include <mutex>

namespace Managers
{


const char* GetString( const char* str )
{
	return GetLanguageManager().getString( str );
}

const wchar_t* GetString( const wchar_t* str )
{
	return GetLanguageManager().getString( str );
}

}

/**
 * a simple thread-safe static storage SingletonHolder implementation
 */
template <typename Class>
class SingletonHolder
{
public:
	static Class &Instance();

private:
	static Class* instance;
	static std::mutex mutex;

	static void NewInstance();
	static void Destroy();

	// should be never ever called
	SingletonHolder();
};




template <typename Class>
inline Class &SingletonHolder<Class>::Instance()
{
	// create instance only if instance is not created
	if (!instance)
		NewInstance();

	return *instance;
}

static std::map<void*, std::function<void()>> g_SingletonCleanup;
static std::mutex g_SingletonCleanupLock;

template <typename Class>
inline void SingletonHolder<Class>::NewInstance()
{
	std::lock_guard<std::mutex> guard(mutex);

	// check again for creation (another thread could accessed the critical section before
	if (instance)
		return;

	instance = new Class();

	std::lock_guard<std::mutex> sguard(g_SingletonCleanupLock);

	g_SingletonCleanup[instance] = []()
	{
		SingletonHolder<Class>::Destroy();
	};
}

template <typename Class>
inline void SingletonHolder<Class>::Destroy()
{
	std::lock_guard<std::mutex> guard(mutex);
	safe_delete(instance);
}

template <typename Class>
Class *SingletonHolder<Class>::instance = nullptr;

template <typename Class>
std::mutex SingletonHolder<Class>::mutex;

class ManagersImpl
{
public:
	void DestroyManagers()
	{
		DestroyCVarManager();
		DestroyConComManager();
	}

	void InitManagers()
	{
		InitCVarManger();
		InitConComManger();
	}
};

ManagersImpl g_Managers;

void InitManagers()
{
	g_Managers.InitManagers();
}

void DestroyManagers()
{
	g_Managers.DestroyManagers();

	std::lock_guard<std::mutex> guard(g_SingletonCleanupLock);
	for (auto s : g_SingletonCleanup)
		s.second();

	g_SingletonCleanup.clear();
}

LanguageManagerI & GetLanguageManager()
{
	return SingletonHolder<LanguageManager>::Instance();
}

ThemeManagerI & GetThemeManager()
{
	return SingletonHolder<ThemeManager>::Instance();
}

WindowManagerI & GetWindowManager()
{
	return SingletonHolder<WindowManager>::Instance();
}

CVarManagerI* GetCVarManager()
{
	return g_pCVarMang;
}

CCommandManagerI* GetCCommandManager()
{
	return g_pConComMang;
}
