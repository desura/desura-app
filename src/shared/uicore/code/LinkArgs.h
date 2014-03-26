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

#ifndef DESURA_LINKARGS_H
#define DESURA_LINKARGS_H
#ifdef _WIN32
#pragma once
#endif



class LinkArgs
{
public:
	LinkArgs()
	{
	}

	LinkArgs(const LinkArgs& args)
		: m_mArgMap(args.m_mArgMap)
	{
	}

	LinkArgs(const std::vector<std::string> &args)
	{
		for (auto x : args)
		{
			std::vector<std::string> out;
			UTIL::STRING::tokenize(x, out, "&");

			for (auto y : out)
				push_back(y);
		}
	}

	void push_back(const std::string &strArg)
	{
		std::vector<std::string> out2;
		UTIL::STRING::tokenize(strArg, out2, "=");

		if (out2.size() == 2)
			m_mArgMap[out2[0]] = out2[1];
		else
			m_mArgMap[out2[0]] = "";
	}

	void push_back(const LinkArgs &args)
	{
		m_mArgMap.insert(begin(args.m_mArgMap), end(args.m_mArgMap));
	}

	bool containsArg(const std::string &key) const
	{
		return (m_mArgMap.find(key) != m_mArgMap.end());
	}

	std::string getArgValue(const std::string &key) const
	{
		auto it = m_mArgMap.find(key);

		if (it != end(m_mArgMap))
			return it->second;

		return "";
	}

	bool empty() const
	{
		return m_mArgMap.empty();
	}

	std::string first() const
	{
		if (empty())
			return "";

		return m_mArgMap.begin()->first;
	}

private:
	std::map<std::string, std::string> m_mArgMap;
};


inline int pushString(LinkArgs &args, const gcString &arg)
{
	args.push_back(arg);
	return 0;
}

inline int pushString(LinkArgs &args, const LinkArgs &arg)
{
	args.push_back(arg);
	return 0;
}

template <typename ... A>
void emptyFunction(A&&... args)
{
}

template <typename ... A>
inline LinkArgs FormatArgs(A&& ... args)
{
	LinkArgs res;
	emptyFunction(pushString(res, args)...);
	return res;
}

#endif