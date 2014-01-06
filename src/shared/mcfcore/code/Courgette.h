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

#ifndef DESURA_COURGETTE_H
#define DESURA_COURGETTE_H
#ifdef _WIN32
#pragma once
#endif

class CourgetteI;

class CourgetteWriteCallbackI
{
public:
	virtual bool writeData(const char* buff, size_t size)=0;
};

template <typename T>
class CourgetteWriteCallback : public CourgetteWriteCallbackI
{
public:
	CourgetteWriteCallback(T &t) : m_Callback(t)
	{
	}

	virtual bool writeData(const char* buff, size_t size)
	{
		return m_Callback(buff, size);
	}

private:
	T &m_Callback;
};

class CourgetteInstance
{
public:
	CourgetteInstance();
	~CourgetteInstance();

	template <typename T>
	bool createDiff(const char* buffOld, size_t sizeOld, const char* buffNew, size_t sizeNew, T &callback)
	{
		CourgetteWriteCallback<T> c(callback);
		return createDiffCB(buffOld, sizeOld, buffNew, sizeNew, &c);
	}

	template <typename T>
	bool createDiff(const char* buffOld, size_t sizeOld, const char* buffNew, size_t sizeNew, const T &callback)
	{
		CourgetteWriteCallback<const T> c(callback);
		return createDiffCB(buffOld, sizeOld, buffNew, sizeNew, &c);
	}

	template <typename T>
	bool applyDiff(const char* fileOld, const char* fileDiff, T &callback)
	{
		CourgetteWriteCallback<T> c(callback);
		return applyDiffCB(fileOld, fileDiff, &c);
	}

	template <typename T>
	bool applyDiff(const char* fileOld, const char* fileDiff, const T &callback)
	{
		CourgetteWriteCallback<const T> c(callback);
		return applyDiffCB(fileOld, fileDiff, &c);
	}

	bool createDiffCB(const char* buffOld, size_t sizeOld, const char* buffNew, size_t sizeNew, CourgetteWriteCallbackI* callback);
	bool applyDiffCB(const char* fileOld, const char* fileDiff, CourgetteWriteCallbackI* callback);

private:
	CourgetteI* m_pCourgette;
};



#endif //DESURA_COURGETTE_H
