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

#ifndef DESURA_GAMEEXPLORERMANAGER_H
#define DESURA_GAMEEXPLORERMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "BaseManager.h"
#include "usercore/ItemInfoI.h"

#ifdef LINK_WITH_GMOCK
#include <gmock/gmock.h>
#endif

namespace sqlite3x
{
	class sqlite3_connection;
}

namespace UserCore
{
	class UserI;

	namespace Item
	{
		class ItemInfo;
	}

	namespace Misc
	{
		class GameExplorerInfo : public BaseItem
		{
		public:
			enum
			{
				FLAG_INSTALLED = 1,
				FLAG_NEEDSUPATE = 2,
			};

			GameExplorerInfo(DesuraId id, UserCore::UserI* user);
			~GameExplorerInfo();

			void loadFromDb(sqlite3x::sqlite3_connection *db);
			void saveToDb(sqlite3x::sqlite3_connection *db);

			void generateDll();
			void installDll();
			void removeDll();

			bool isInstalled();
			bool needsUpdate();
			bool isInstallable();
			bool needsInstall();

		protected:
			gcWString generateXml();
			gcString generateGuid();

			void regEvent();

			void translateIco(HANDLE handle, const char* icoPath);
			void onInfoChanged(UserCore::Item::ItemInfoI::ItemInfo_s &info);

		protected:
			gcString m_szDllPath;
			gcString m_szGuid;
			uint32 m_uiFlags;

			DesuraId m_Id;
			UserCore::UserI* m_pUser;
			UserCore::Item::ItemInfoI* m_pItemInfo;
		};
	}

	class GameExplorerManagerI
	{
	public:
		virtual void addItem(DesuraId item)=0;
		virtual void removeItem(DesuraId item)=0;

		virtual void loadItems()=0;
		virtual void saveItems()=0;

	protected:
		virtual ~GameExplorerManagerI(){}
	};

#ifdef LINK_WITH_GMOCK
	class GameExplorerManagerMock : public GameExplorerManagerI
	{
	public:
		MOCK_METHOD1(addItem, void(DesuraId));
		MOCK_METHOD1(removeItem, void(DesuraId));

		MOCK_METHOD0(loadItems, void());
		MOCK_METHOD0(saveItems, void());
	};
#endif

	class GameExplorerManager : public GameExplorerManagerI, public BaseManager<Misc::GameExplorerInfo>
	{
	public:
		GameExplorerManager(UserI* user);

		void addItem(DesuraId item) override;
		void removeItem(DesuraId item) override;

		void loadItems() override;
		void saveItems() override;

	protected:
		bool shouldInstallItems();

	private:
		UserI* m_pUser;
	};
}


#endif //DESURA_GAMEEXPLORERMANAGER_H
