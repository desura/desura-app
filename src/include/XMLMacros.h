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

#ifndef DESURA_XML_MACROS_H
#define DESURA_XML_MACROS_H
#ifdef _WIN32
#pragma once
#endif

#include <tinyxml2.h>
typedef void (*gcszFn)(const char*);


namespace XML
{
	class gcXMLElement
	{
	public:
		gcXMLElement()
			: m_XmlDoc(nullptr)
			, m_pElement(nullptr)
			, m_pConstElement(nullptr)
		{
		}

		gcXMLElement(const tinyxml2::XMLElement* pElement)
			: m_XmlDoc(nullptr)
			, m_pElement(nullptr)
			, m_pConstElement(pElement)
		{
		}

		gcXMLElement(tinyxml2::XMLDocument& xmlDoc, tinyxml2::XMLElement* pElement)
			: m_XmlDoc(&xmlDoc)
			, m_pElement(pElement)
			, m_pConstElement(pElement)
		{
		}

		template <typename T>
		void for_each_child(const char* name, const T& t) const
		{
			if (!m_pConstElement || !name)
				return;

			auto child = m_pConstElement->FirstChildElement(name);

			while (child)
			{
				const gcXMLElement c(child);
				t(c);

				child = child->NextSiblingElement(name);
			};
		}

		gcString GetAtt(const char* name) const
		{
			gcString outVal;
			GetAtt(name, outVal);
			return outVal;
		}

		gcString GetText() const
		{
			if (!m_pConstElement)
				return "";

			return m_pConstElement->GetText();
		}

		void GetAtt(const char* name, gcString& outVal) const
		{
			if (!m_pConstElement || !name)
				return;

			const char* val = m_pConstElement->Attribute(name);

			if (!val)
				return;

			outVal = val;
		}

		void GetAtt(const char* name, uint32& outVal) const
		{
			gcString r;
			GetAtt(name, r);

			if (r.size() > 0)
				outVal = Safe::atoi(r.c_str());
		}

		gcString GetChild(const char* name) const
		{
			gcString outVal;
			GetChild(name, outVal);
			return outVal;
		}

		template <class T>
		bool GetChild(const char* name, T* obj, void (T::*func)(const char*)) const
		{
			if (!name || !func || !m_pConstElement || !obj)
				return false;

			auto child = m_pConstElement->FirstChildElement(name);
	
			if (child)
				(*obj.*func)(child->GetText());

			return !!child;
		}

		bool GetChild(const char* name, gcszFn func) const
		{
			if (!name || !func || !m_pConstElement)
				return false;

			auto child = m_pConstElement->FirstChildElement(name);

			if (child)
				func(child->GetText());

			return !!child;
		}

		template <int SIZE>
		bool GetChild(const char* name, gcFixedString<SIZE>& str) const
		{
			gcString strTemp;
			auto ret = GetChild(name, strTemp);

			if (ret)
				str = strTemp;

			return ret;
		}

		bool GetChild(const char* name, gcString& str) const
		{
			if (!name || !m_pConstElement)
				return false;

			auto child = m_pConstElement->FirstChildElement(name);

			if (child)
				str = child->GetText();

			return !!child;
		}

		bool GetChild(const char* name, std::string& str) const
		{
			if (!name || !m_pConstElement)
				return false;

			auto child = m_pConstElement->FirstChildElement(name);

			if (child && child->GetText())
				str = child->GetText();

			return !!child;
		}

		bool GetChild(const char* name, char*& str) const
		{
			gcString string;
			bool res = GetChild(name, string);

			safe_delete(str);
			str = new char[string.length()+1];
		
		#ifdef WIN32
			strcpy_s(str, string.length()+1, string.c_str());
		#else
			strcpy(str, string.c_str());
		#endif

			return res;
		}

		bool GetChild(const char* name, int32& num) const
		{
			gcString string("0");
			bool res = GetChild(name, string);

			if (res)
				num = Safe::atoi(string.c_str());

			return res;
		}

		bool GetChild(const char* name, uint32& num) const
		{
			int32 value = 0;
			bool res = GetChild(name, value);

			if (res)
				num = (uint32)value;

			return res;
		}

		bool GetChild(const char* name, int64& num) const
		{
			gcString string("0");
			bool res = GetChild(name, string);

			if (res)
				num = Safe::atoll(string.c_str());

			return res;
		}

		bool GetChild(const char* name, uint64& num) const
		{
			int64 value = 0;
			bool res = GetChild(name, value);

			if (res)
				num = (uint64)value;

			return res;
		}

		bool GetChild(const char* name, uint16& num) const
		{
			int32 value = 0;
			bool res = GetChild(name, value);

			if (res)
				num = (uint16)value;

			return res;
		}

		bool GetChild(const char* name, uint8& num) const
		{
			int32 value = 0;
			bool res = GetChild(name, value);

			if (res)
				num = (uint8)value;

			return res;
		}

		bool GetChild(const char* name, bool& num) const
		{
			gcString value;
			bool res = GetChild(name, value);

			if (res)
				num = (value == "true" || value == "1" || value == "yes");

			return res;
		}


		void WriteChild(const char* name, const char* value)
		{
			if ( !value || !m_pElement || !name || !m_XmlDoc)
				return;

			auto newEle = m_XmlDoc->NewElement(name);
			auto newTextEle = m_XmlDoc->NewText(value);

			newEle->LinkEndChild(newTextEle);
			m_pElement->LinkEndChild( newEle );
		}

		void WriteChild(const char* name, const gcString &val)
		{
			WriteChild(name, val.c_str());
		}

		template <typename T>
		void WriteChild(const char* name, T &val)
		{
			WriteChild(name, gcString("{0}", val).c_str());
		}

		template <typename T>
		void WriteChild(const char* name, T* val)
		{
			if (val == nullptr)
				WriteChild(name, "");
			else
				WriteChild(name, gcString("{0}", val).c_str());
		}

		bool IsValid() const
		{
			return !!m_pConstElement;
		}

		gcXMLElement FirstChildElement(const char* name)
		{
			assert(m_pConstElement);

			if (!m_pConstElement)
				return gcXMLElement();

			if (m_pElement)
				return gcXMLElement(*m_XmlDoc, m_pElement->FirstChildElement(name));

			return gcXMLElement(m_pConstElement->FirstChildElement(name));
		}

		const gcXMLElement FirstChildElement(const char* name) const
		{
			assert(m_pConstElement);

			if (!m_pConstElement)
				return gcXMLElement();

			return gcXMLElement(m_pConstElement->FirstChildElement(name));
		}

		gcXMLElement NewElement(const char* szName)
		{
			assert(m_pElement && m_XmlDoc);

			if (!m_pElement || !m_XmlDoc)
				return gcXMLElement();

			auto pNode = m_XmlDoc->NewElement(szName);
			m_pElement->InsertEndChild(pNode);

			return gcXMLElement(*m_XmlDoc, pNode);
		}

		template <typename T>
		void SetAttribute(const char* szName, T tVal)
		{
			assert(m_pElement);

			if (!m_pElement)
				return;

			m_pElement->SetAttribute(szName, tVal);
		}

		void SetText(const char* szText)
		{
			assert(m_pElement);

			if (!m_pElement)
				return;

			m_pElement->LinkEndChild(m_XmlDoc->NewText(szText));
		}

	private:
		tinyxml2::XMLDocument* m_XmlDoc;
		tinyxml2::XMLElement* m_pElement;
		const tinyxml2::XMLElement* m_pConstElement;
	};



	class gcXMLDocument
	{
	public:
		gcXMLDocument()
		{
		}

		explicit gcXMLDocument(const char* szFile)
		{
			m_eXMLLoadError = m_XmlDoc.LoadFile(szFile);
		}

		explicit gcXMLDocument(const char* szBuffer, uint32 nLength)
		{
			m_eXMLLoadError = m_XmlDoc.Parse(szBuffer, nLength);
		}

		gcXMLDocument(const gcXMLDocument&) = delete;

		gcXMLElement GetRoot()
		{
			return gcXMLElement(m_XmlDoc, m_XmlDoc.FirstChildElement());
		}

		gcXMLElement GetRoot(const std::string& strRoot)
		{
			return gcXMLElement(m_XmlDoc, m_XmlDoc.FirstChildElement(strRoot.c_str()));
		}

		const gcXMLElement GetRoot() const
		{
			return gcXMLElement(m_XmlDoc.FirstChildElement());
		}

		const gcXMLElement GetRoot(const std::string& strRoot) const
		{
			return gcXMLElement(m_XmlDoc.FirstChildElement(strRoot.c_str()));
		}


		//! Returns version
		uint32 ProcessStatus(const std::string& strRoot) const
		{
			int s;
			return ProcessStatus(strRoot, s);
		}

		//! Returns version
		uint32 ProcessStatus(const std::string& strRoot, int &status) const
		{
			uint32 v = 1;

			auto uNode = m_XmlDoc.FirstChildElement(strRoot.c_str());

			if (!uNode)
				uNode = m_XmlDoc.FirstChildElement("servererror");

			if (!uNode)
				throw gcException(ERR_BADXML, "Missing root node");

			auto sNode = uNode->FirstChildElement("status");

			if (!sNode)
				throw gcException(ERR_BADXML, "Missing status node");

			status = -1;
			const char* statStr = sNode->Attribute("code");

			if (!statStr)
				throw gcException(ERR_BADXML, "No status code");
		
			status = Safe::atoi(statStr);

			if (status != 0)
				throw gcException(ERR_BADSTATUS, status, gcString("Status: {0}", sNode->GetText()));

			const char* verStr = uNode->Attribute("version");

			if (verStr)
				v = Safe::atoi(verStr);

			if (v == 0)
				v = 1;

			return v;
		}

		void LoadBuffer(const char* buff, size_t buffLen)
		{
			m_XmlDoc.DeleteChildren();
			m_eXMLLoadError = m_XmlDoc.Parse(buff, buffLen);
		}

		bool IsValid() const
		{
			return m_eXMLLoadError == tinyxml2::XML_SUCCESS;
		}

		gcXMLElement Create(const char* szRoot, const char* str = nullptr)
		{
			m_XmlDoc.DeleteChildren();
			m_eXMLLoadError = tinyxml2::XML_SUCCESS;

			auto root = m_XmlDoc.NewElement(szRoot);

			m_XmlDoc.InsertEndChild(m_XmlDoc.NewDeclaration(str));
			m_XmlDoc.InsertEndChild(root);

			return gcXMLElement(m_XmlDoc, root);

		}

		bool SaveFile(const char* szPath)
		{
			return m_XmlDoc.SaveFile(szPath) == tinyxml2::XML_SUCCESS;
		}

		gcString ToString(bool bCompact = false)
		{
			tinyxml2::XMLPrinter printer(0, bCompact);
			m_XmlDoc.Accept(&printer);

			return std::string(printer.CStr(), printer.CStrSize());
		}

		gcWString ToWString(bool bCompact = false)
		{
			return gcWString(ToString(bCompact));
		}

	private:
		tinyxml2::XMLError m_eXMLLoadError = tinyxml2::XML_ERROR_EMPTY_DOCUMENT;
		tinyxml2::XMLDocument m_XmlDoc;
	};
}

#endif //DESURA_XML_MACROS_H
