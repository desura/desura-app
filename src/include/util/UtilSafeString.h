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

#ifndef DESURA_UTILSAFESTRING_H
#define DESURA_UTILSAFESTRING_H


namespace Safe
{
	inline int strcpy(char* dest, size_t destSize, const char* source)
	{
#ifdef WIN32
		return ::strncpy_s(dest, destSize, source, _TRUNCATE);
#else
		return ::strncpy(dest, source, destSize) == source ? 0 : 1;
#endif
	}

	inline void strncpy(char* dest, size_t destSize, const char* source, size_t count)
	{
#ifdef WIN32
		::strncpy_s(dest, destSize, source, count);
#else
		::strncpy(dest, source, count);
#endif
	}

	inline void wcscpy(wchar_t* dest, size_t destSize, const wchar_t* source)
	{
#ifdef WIN32
		::wcsncpy_s(dest, destSize, source, _TRUNCATE);
#else
		::wcsncpy(dest, source, destSize);
#endif
	}

	inline void wcsncpy(wchar_t* dest, size_t destSize, const wchar_t* source, size_t count)
	{
#ifdef WIN32
		::wcsncpy_s(dest, destSize, source, count);
#else
		::wcsncpy(dest, source, count);
#endif
	}

	inline void snprintf(char* dest, size_t destSize, const char* format, ...)
	{
		va_list args;
		va_start(args, format);

#if defined(WIN32) && !defined(__MINGW32__)
		_vsnprintf_s(dest, destSize, _TRUNCATE, format, args);
#else
		::vsnprintf(dest, destSize, format, args);
#endif

		va_end(args);
	}

	inline void snwprintf(wchar_t* dest, size_t destSize, const wchar_t* format, ...)
	{
		va_list args;
		va_start(args, format);

#ifdef WIN32
		_vsnwprintf_s(dest, destSize, _TRUNCATE, format, args);
#else
		::vswprintf(dest, destSize, format, args);
#endif

		va_end(args);
	}

	inline FILE* fopen(const char* file, const char* args)
	{
#ifdef WIN32
		FILE* fileHandle = nullptr;
		fopen_s(&fileHandle, file, args);
		return fileHandle;
#else
		return ::fopen(file, args);
#endif
	}

	inline char* strtok(char *strToken, const char *strDelimit, char **context)
	{
#ifdef WIN32
		return strtok_s(strToken, strDelimit, context);
#else
		return ::strtok_r(strToken, strDelimit, context);
#endif
	}

	inline int stricmp(const char* str1, const char* str2)
	{
#ifdef WIN32
		return _stricmp(str1, str2);
#else
		return ::strcasecmp(str1, str2);
#endif
	}

	inline int strnicmp(const char* str1, const char* str2, size_t count)
	{
#ifdef WIN32
		return _strnicmp(str1, str2, count);
#else
		return ::strncasecmp(str1, str2, count);
#endif
	}

	inline void strcat(char* dest, size_t destSize, const char* source)
	{
#if defined(WIN32) && !defined(__MINGW32__)
		strcat_s(dest, destSize, source);
#else
		::strncat(dest, source, destSize);
#endif
	}

	inline size_t strlen(const char* str, size_t strSize)
	{
#if defined(WIN32) && !defined(__MINGW32__)
		return strnlen_s(str, strSize);
#else
		return ::strnlen(str, strSize);
#endif
	}

	inline size_t wcslen(const wchar_t* str, size_t strSize)
	{
#if defined(WIN32) && !defined(__MINGW32__)
		return wcsnlen_s(str, strSize);
#else
		return ::wcsnlen(str, strSize);
#endif
	}


	inline void strcpy(char** dest, const char* src, size_t srcSize)
	{
		safe_delete(*dest);

		if (!src)
			return;

		size_t nLen = Safe::strlen(src, srcSize);

		(*dest) = new char[nLen + 1];
		Safe::strcpy(*dest, nLen + 1, src);
	}

	inline void wcscpy(wchar_t** dest, const wchar_t* src, size_t srcSize)
	{
		safe_delete(*dest);

		if (!src)
			return;

		size_t nLen = Safe::wcslen(src, srcSize);

		(*dest) = new wchar_t[nLen + 1];
		Safe::wcscpy(*dest, nLen + 1, src);
	}


	inline int wcsicmp(const wchar_t* str1, const wchar_t* str2)
	{
#ifdef WIN32
		return _wcsicmp(str1, str2);
#else
		return ::wcscasecmp(str1, str2);
#endif
	}

	inline uint64 atoll(const char* szString)
	{
		if (!szString)
			return 0;
#if defined(__APPLE__)
		return strtoull(szString, nullptr, 0);
#else
		return std::strtoull(szString, nullptr, 0);
#endif		
	}

	//Make sure we handle the last bit correctly by using 64bit then down cast to 32bit
	inline uint32 atoi(const char* szString)
	{
		if (!szString)
			return 0;

#if defined(__APPLE__)
		return (uint32)strtoull(szString, nullptr, 0);
#else
		return (uint32)std::strtoull(szString, nullptr, 0);
#endif		
	}
}

#endif
