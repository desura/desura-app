/*
Copyright (C) 2010 Mark Chandler (Desura Net Pty Ltd)
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
#include "gcJSBase.h"


static std::mutex g_JSObjectLock;
static std::map<std::string, std::function<void()>> g_JSObjectCleanup;

bool FindJSObjectCleanup(const std::string &strKey)
{
	std::lock_guard<std::mutex> gaurd(g_JSObjectLock);
	return g_JSObjectCleanup.find(strKey) != end(g_JSObjectCleanup);
}

void AddJSObjectCleanup(const std::string &strKey, std::function<void()> fnCleanup)
{
	gcAssert(fnCleanup);
	gcAssert(!FindJSObjectCleanup(strKey));

	std::lock_guard<std::mutex> gaurd(g_JSObjectLock);
	g_JSObjectCleanup[strKey] = fnCleanup;
}

void JSObjectCleanup()
{
	std::lock_guard<std::mutex> gaurd(g_JSObjectLock);

	for (auto p : g_JSObjectCleanup)
		p.second();

	g_JSObjectCleanup.clear();
}


JSDelegateI::~JSDelegateI()
{
}




gcString getString(JSObjHandle& arg)
{
	int size = arg->getStringValue(nullptr, 0);

	if (size == 0)
		return "";

	char *buff = new char[size+1];
	arg->getStringValue(buff, size+1);

	buff[size] = 0;

	gcString ret(buff);
	safe_delete(buff);

	return ret;
}

template <>
void FromJSObject<PVoid>(PVoid&, JSObjHandle& arg)
{
}

template <>
void FromJSObject<bool>(bool& ret, JSObjHandle& arg)
{
	if (arg->isBool())
		ret = arg->getBoolValue();
	else if (arg->isInt())
		ret = arg->getIntValue()?true:false;
	else if (arg->isDouble())
		ret = arg->getDoubleValue()?true:false;
	else if (arg->isString())
	{
		gcString b = getString(arg);
		ret = (b != "false" && b != "0");
	}
	else
	{
		throw gcException(ERR_INVALIDDATA, "Failed to convert javascript arg to bool");
	}
}

template <>
void FromJSObject<int32>(int32 &ret, JSObjHandle& arg)
{
	if (arg->isInt())
		ret = arg->getIntValue();
	else if (arg->isBool())
		ret = arg->getBoolValue();
	else if (arg->isString())
	{
		gcString b = getString(arg);
		int64 i = Safe::atoll(b.c_str());
		ret = (int32)i;
	}
	else
	{
		throw gcException(ERR_INVALIDDATA, "Failed to convert javascript arg to int32");
	}
}

template <>
void FromJSObject<double>(double& ret, JSObjHandle& arg)
{
	if (arg->isInt())
		ret = arg->getIntValue();
	else if (arg->isDouble())
		ret = arg->getDoubleValue();
	else
		throw gcException(ERR_INVALIDDATA, "Failed to convert javascript arg to double");
}

template <>
void FromJSObject<gcString>(gcString& ret, JSObjHandle& arg)
{
	if (arg->isString())
	{
		ret = getString(arg);
	}
	else if (arg->isBool())
	{
		ret = gcString("{0}", arg->getBoolValue());
	}
	else if (arg->isInt())
	{
		ret = gcString("{0}", arg->getIntValue());
	}
	else if (arg->isDouble())
	{
		ret = gcString("{0}", arg->getDoubleValue());
	}
	else if (arg->isNull() || arg->isUndefined())
	{
		ret = "";
	}
	else
	{
		throw gcException(ERR_INVALIDDATA, "Failed to convert javascript arg to string");
	}
}

template <>
void FromJSObject<gcWString>(gcWString& ret, JSObjHandle& arg)
{
	gcString str;
	FromJSObject(str, arg);
	ret = str;
}

JSObjHandle ToJSObject(ChromiumDLL::JavaScriptFactoryI* factory, const int32 intVal)
{
	return factory->CreateInt(intVal);
}

JSObjHandle ToJSObject(ChromiumDLL::JavaScriptFactoryI* factory, const bool boolVal)
{
	return factory->CreateBool(boolVal);
}

JSObjHandle ToJSObject(ChromiumDLL::JavaScriptFactoryI* factory, const double doubleVal)
{
	return factory->CreateDouble(doubleVal);
}

JSObjHandle ToJSObject(ChromiumDLL::JavaScriptFactoryI* factory, const gcString &stringVal)
{
	return factory->CreateString(stringVal.c_str());
}

JSObjHandle ToJSObject(ChromiumDLL::JavaScriptFactoryI* factory, const gcWString &stringVal)
{
	return factory->CreateString(gcString(stringVal).c_str());
}

JSObjHandle ToJSObject(ChromiumDLL::JavaScriptFactoryI* factory, const std::map<gcString, MapElementI*> &map)
{
	JSObjHandle obj = factory->CreateObject();

	std::for_each(map.begin(), map.end(), [&obj, factory](std::pair<gcString, MapElementI*> pair){
		obj->setValue(pair.first.c_str(), pair.second->toJSObject(factory));
		pair.second->destory();
	});

	return obj;
}

JSObjHandle ToJSObject(ChromiumDLL::JavaScriptFactoryI* factory, const std::map<gcString, gcString> &map)
{
	JSObjHandle obj = factory->CreateObject();

	std::for_each(map.begin(), map.end(), [&obj, factory](std::pair<gcString, gcString> pair){
		obj->setValue(pair.first.c_str(), ToJSObject(factory, pair.second));
	});

	return obj;
}

JSObjHandle ToJSObject(ChromiumDLL::JavaScriptFactoryI* factory, const std::vector<MapElementI*> &map)
{
	JSObjHandle obj = factory->CreateArray(map.size());

	for (size_t x=0; x<map.size(); x++)
	{
		obj->setValue(x, map[x]->toJSObject(factory));
		map[x]->destory();
	}

	return obj;
}

JSObjHandle ToJSObject(ChromiumDLL::JavaScriptFactoryI* factory, const MapElementI* cmap)
{
	MapElementI* map = const_cast<MapElementI*>(cmap);

	JSObjHandle ret = map->toJSObject(factory);
	map->destory();
	return ret;
}

JSObjHandle ToJSObject(ChromiumDLL::JavaScriptFactoryI* factory, const void* cobject)
{
	void* object = const_cast<void*>(cobject);
	return factory->CreateObject(object);
}


////////////////////////////////////////////
////////////////////////////////////////////

DesuraJSBaseNonTemplate::DesuraJSBaseNonTemplate(const char* name, const char* bindingFile)
{
	m_szName = gcString(L"Desura/{0}", name);
	m_szBindingFile = gcString("{2}{1}bindings{1}{0}", bindingFile, DIRS_STR,
		UTIL::OS::getDataPath());

	m_pContext = nullptr;
}

DesuraJSBaseNonTemplate::~DesuraJSBaseNonTemplate()
{
	for (size_t x=0; x<m_mDelegateList.size(); x++)
		safe_delete(m_mDelegateList[x].second);
}

const char* DesuraJSBaseNonTemplate::getName()
{
	return m_szName.c_str();
}

const char* DesuraJSBaseNonTemplate::getRegistrationCode()
{
	char* buff=nullptr;
	uint32 size = 0;

	try
	{
		size = UTIL::FS::readWholeFile(UTIL::FS::PathWithFile(m_szBindingFile), &buff);
		m_szRegCode.assign(buff, buff+size);
	}
	catch (gcException &e)
	{
		Warning("Failed to read js binding file {0}: {1}", m_szBindingFile, e);
	}

	safe_delete(buff);
	return m_szRegCode.c_str();
}

JSObjHandle DesuraJSBaseNonTemplate::execute(ChromiumDLL::JavaScriptFunctionArgs* args)
{
#ifdef DEBUG
	static std::thread::id s_IgnoreStack = UTIL::OS::ignoreStackTraceOnThisThread();
#endif

	m_pContext = args->context;

	try
	{
		if (!args->factory || !args->function)
			return nullptr;

		uint32 id = UTIL::MISC::RSHash_CSTR(args->function, strlen(args->function));
		uint32 index = find(id);

		if (index == UINT_MAX)
			throw gcException(ERR_INVALID, "Function not found");

		if (!preExecuteValidation(args->function, id, args->object, args->argc, args->argv))
			return nullptr;

		return m_mDelegateList[index].second->operator()(args->factory, args->context, args->object, args->argc, args->argv);
	}
	catch(std::exception &e)
	{
		return args->factory->CreateException(e.what());
	}
}

void DesuraJSBaseNonTemplate::registerFunction(const char* name, JSDelegateI *delegate)
{
	uint32 id = UTIL::MISC::RSHash_CSTR(name);
	uint32 index = find(id);

	if (index != UINT_MAX)
	{
		safe_delete(m_mDelegateList[index].second);
		m_mDelegateList.erase(m_mDelegateList.begin()+index);
	}

	m_mDelegateList.push_back(std::pair<uint32, JSDelegateI*>(id, delegate));

	std::sort(m_mDelegateList.begin(), m_mDelegateList.end(), [](std::pair<uint32, JSDelegateI*> a, std::pair<uint32, JSDelegateI*> b){
		return a.first <= b.first;
	});
}


uint32 DesuraJSBaseNonTemplate::find(uint32 hash)
{
	if (m_mDelegateList.size() == 0)
		return -1;

	return find(hash, 0, m_mDelegateList.size()-1);
}

uint32 DesuraJSBaseNonTemplate::find(uint32 hash, uint32 f, uint32 l)
{
	if (l == 0 && f == 0 && m_mDelegateList[0].first == hash)
		return 0;

	if (l==f)
		return -1;

	uint32 midIndex = (l-f)/2;

	if (midIndex == 0)
	{
		if (m_mDelegateList[f].first == hash)
			return f;
		else if (m_mDelegateList[l].first == hash)
			return l;
		else
			return -1;
	}

	midIndex += f;
	uint32 midHash = m_mDelegateList[midIndex].first;

	if (midHash == hash)
		return midIndex;
	else if (hash > midHash )
		return find(hash, midIndex, l);
	else
		return find(hash, f, midIndex);
}
