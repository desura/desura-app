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

#ifndef DESURA_TASKTESTINGBASE_H
#define DESURA_TASKTESTINGBASE_H
#ifdef _WIN32
#pragma once
#endif


#ifdef LINK_WITH_GMOCK

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "usercore/ToolManagerI.h"
#include "BranchInfo.h"
#include "BranchInstallInfo.h"
#include "ItemInfo.h"
#include "mcfcore/MCFI.h"
#include "mcfcore/MCFHeaderI.h"

using namespace IPC;
using namespace UserCore;
using namespace WebCore;
using namespace MCFCore;
using namespace UserCore::Item;

namespace UnitTest
{


	template <typename Base = ::testing::Test>
	class BaseTaskTestingFixture : public Base
	{
	public:
		BaseTaskTestingFixture()
			: m_Id("123", "games")
			, m_Branch(MCFBranch::BranchFromInt(1))
			, m_Build(MCFBuild::BuildFromInt(2))
			, m_ItemInfo(&m_User, m_Id)
		{
			ON_CALL(m_User, getItemManager()).WillByDefault(Return(&m_ItemManager));
			ON_CALL(m_User, getServiceMain()).WillByDefault(Return(&m_ServiceMain));
			ON_CALL(m_User, getWebCore()).WillByDefault(Return(&m_WebCore));
			ON_CALL(m_User, getGameExplorerManager()).WillByDefault(Return(&m_GameExplorerManager));
			ON_CALL(m_User, getUserId()).WillByDefault(Return(m_nUserId));
			ON_CALL(m_User, getItemsAddedEvent()).WillByDefault(Return(&m_ItemAddedEvent));
			ON_CALL(m_User, getInternal()).WillByDefault(Return(&m_UserInternal));
			ON_CALL(m_User, getToolManager()).WillByDefault(Return(&m_ToolManager));

			ON_CALL(m_UserInternal, getMCFManager()).WillByDefault(Return(&m_McfManager));

			ON_CALL(m_Mcf, getHeader()).WillByDefault(Return(&m_McfHeader));
			ON_CALL(m_Mcf, getProgEvent()).WillByDefault(ReturnRef(m_McfProgressEvent));
			ON_CALL(m_Mcf, getErrorEvent()).WillByDefault(ReturnRef(m_McfErrorEvent));
			ON_CALL(m_Mcf, getINSize()).WillByDefault(Return(m_nInsSize));

			ON_CALL(m_McfHeader, getBuild()).WillByDefault(Return(m_Build));
			ON_CALL(m_McfHeader, getBranch()).WillByDefault(Return(m_Branch));
			ON_CALL(m_McfHeader, getId()).WillByDefault(Return(m_Id.getItem()));
			ON_CALL(m_McfHeader, getType()).WillByDefault(Return(m_Id.getType()));
	
			ON_CALL(m_ItemHandle, getInternal()).WillByDefault(Return(&m_ItemHandleInternal));
			ON_CALL(m_ItemHandle, getItemInfo()).WillByDefault(Return(&m_ItemInfo));

			ON_CALL(m_WebCore, getDownloadProviders(_, _, _, _)).WillByDefault(Invoke(this, &BaseTaskTestingFixture::getDownloadProviders));
		}

		void setup(const char* itemInfoXml)
		{
			WildcardManager wildcard;

			{
			static const char* gs_szWildcardXml =
				"<wcards>"
					"<wcard name=\"APPLICATION\" type=\"special\">APPLICATION</wcard>"
				"</wcards>";

				tinyxml2::XMLDocument doc;
				doc.Parse(gs_szWildcardXml);
				wildcard.parseXML(doc.RootElement());
			}

			wildcard.onNeedSpecialEvent += delegate(this, &ComboDownloadInstallTaskFixture::resolveWildcard);
			wildcard.onNeedInstallSpecialEvent += delegate(this, &ComboDownloadInstallTaskFixture::resolveWildcard);

			XML::gcXMLDocument doc(itemInfoXml, strlen(itemInfoXml));
			m_ItemInfo.loadXmlData(100, doc.GetRoot("game"), 0, &wildcard);	
		}

		const uint32 m_nUserId = 666;
		const uint64 m_nInsSize = 12345;
		const gcString m_strMcfSavePath = "C:\\TestPath";

		Event<uint32> m_ItemAddedEvent;
		Event<MCFCore::Misc::ProgressInfo> m_McfProgressEvent;
		Event<gcException> m_McfErrorEvent;

		DesuraId m_Id; 
		MCFBranch m_Branch;
		MCFBuild m_Build;

		MCFHeaderMock m_McfHeader;

		ItemInfo m_ItemInfo;
		
		ItemManagerMock m_ItemManager;
		ItemHandleMock m_ItemHandle;
		ItemHandleInternalMock m_ItemHandleInternal;
		UserMock m_User;
		UserInternalMock m_UserInternal;
		WebCoreMock m_WebCore;
		ServiceMainMock m_ServiceMain;
		MCFMock m_Mcf;
		GameExplorerManagerMock m_GameExplorerManager;
		MCFManagerMock m_McfManager;
		ToolManagerMock m_ToolManager;

		bool m_bUnAuthedDownload = false;

	private:
		void getDownloadProviders(DesuraId id, XML::gcXMLDocument &xmlDocument, MCFBranch mcfBranch, MCFBuild mcfBuild)
		{
			gcString szProviders(
				"<itemdownloadurl>"
				"	<status code=\"0\"/>"
				"	<item sitearea=\"games\" siteareaid=\"123\">"
				"		<name>Sample Mod</name>"
				"		<mcf build=\"2\" id=\"789\" branch=\"1\">"
				"		<urls>"
				"			<url>"
				"				<link>mcf://server:62001</link>"
				"				<provider>desura.com</provider>"
				"				<banner>http://www.desura.com/banner.png</banner>"
				"				<provlink>http://www.desura.com</provlink>"
				"			</url>"
				"		</urls>"
				"		<version>1.0</version>"
				"		<installsize>1024</installsize>"
				"		<filesize>1024</filesize>"
				"		<filehash>##############</filehash>"
				"		<authhash>##############</authhash>"
				"		<authed>{0}</authed>"
				"		</mcf>"
				"	</item>"
				"</itemdownloadurl>", m_bUnAuthedDownload?1:0);

			xmlDocument.LoadBuffer(szProviders.c_str(), szProviders.size());
		}

		void resolveWildcard(WCSpecialInfo &info)
		{
			if (info.name == "APPLICATION")
			{
				info.result = UTIL::OS::getAppInstallPath();
				info.handled = true;
			}
		}
	};

#define GAME_TEST_BODY \
		"<game siteareaid=\"123\">" \
		"	<name>Test Game</name>" \
		"	<nameid>test-game</nameid>" \
		"	<settings>" \
		"		<executes>" \
		"			<execute>" \
		"				<name>Play</name>" \
		"				<exe>play.exe</exe>" \
		"				<args>args</args>" \
		"			</execute>" \
		"		</executes>" \
		"		<installlocations>" \
		"			<installlocation>" \
		"				<check>%APPLICATION%/play.exe</check>" \
		"				<path>%APPLICATION%</path>" \
		"			</installlocation>" \
		"		</installlocations>" \
		"	</settings>" \
		"	<branches>"

#define GAME_TEST_END  \
		"	</branches>" \
		"</game>";

	const char* g_szItemInfo_Default =
		GAME_TEST_BODY
		"		<branch id=\"1\" platformid=\"100\">"
		"			<name>Test</name>"
		"			<nameon>1</nameon>"
		"			<global>1</global>"
		"			<free>0</free>"
		"			<price>5,00€ EUR</price>"
		"			<cdkey>0</cdkey>"
		"			<demo>0</demo>"
		"			<test>0</test>"
		"			<inviteonly>1</inviteonly>"
		"			<regionlock>0</regionlock>"
		"			<preload>0</preload>"
		"			<onaccount>1</onaccount>"
		"			<mcf id=\"789\">"
		"				<build>2</build>"
		"			</mcf>"
		"		</branch>"
		GAME_TEST_END

	const char* g_szItemInfo_Preload =
		GAME_TEST_BODY
		"		<branch id=\"1\" platformid=\"100\">"
		"			<name>Test</name>"
		"			<nameon>1</nameon>"
		"			<global>1</global>"
		"			<free>0</free>"
		"			<price>5,00€ EUR</price>"
		"			<cdkey>0</cdkey>"
		"			<demo>0</demo>"
		"			<test>0</test>"
		"			<inviteonly>1</inviteonly>"
		"			<regionlock>0</regionlock>"
		"			<preload>20201001121200</preload>"
		"			<onaccount>1</onaccount>"
		"			<mcf id=\"789\">"
		"				<build>2</build>"
		"			</mcf>"
		"		</branch>"
		GAME_TEST_END

	const char* g_szItemInfo_Preorder =
		GAME_TEST_BODY
		"		<branch id=\"1\" platformid=\"100\">"
		"			<name>Test</name>"
		"			<nameon>1</nameon>"
		"			<global>1</global>"
		"			<free>0</free>"
		"			<price>5,00€ EUR</price>"
		"			<cdkey>0</cdkey>"
		"			<demo>0</demo>"
		"			<test>0</test>"
		"			<inviteonly>1</inviteonly>"
		"			<regionlock>0</regionlock>"
		"			<preload>20201001121200</preload>"
		"			<onaccount>1</onaccount>"
		"		</branch>"
		GAME_TEST_END

	const char* g_szItemInfo_Default_Tools =
		GAME_TEST_BODY
		"		<branch id=\"1\" platformid=\"100\">"
		"			<name>Test</name>"
		"			<nameon>1</nameon>"
		"			<global>1</global>"
		"			<free>0</free>"
		"			<price>5,00€ EUR</price>"
		"			<cdkey>0</cdkey>"
		"			<demo>0</demo>"
		"			<test>0</test>"
		"			<inviteonly>1</inviteonly>"
		"			<regionlock>0</regionlock>"
		"			<preload>0</preload>"
		"			<onaccount>1</onaccount>"
		"			<mcf id=\"789\">"
		"				<build>2</build>"
		"			</mcf>"
		"			<tools>"
		"				<tool>1</tool>"
		"			</tools>"
		"		</branch>"
		GAME_TEST_END




}

#endif

#endif