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

#ifndef DESURA_GAMEEXPLORERMANAGER_H
#define DESURA_GAMEEXPLORERMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "BaseManager.h"
#include "usercore/ItemInfoI.h"

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

			GameExplorerInfo(DesuraId id, gcRefPtr<UserCore::UserI> &user);
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
			gcRefPtr<UserCore::UserI> m_pUser;
			gcRefPtr<UserCore::Item::ItemInfoI> m_pItemInfo;
		};
	}

	class GameExplorerManagerI : public gcRefBase
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

		gc_IMPLEMENT_REFCOUNTING(GameExplorerManager);
	};
#endif

	class GameExplorerManager : public GameExplorerManagerI, public BaseManager<Misc::GameExplorerInfo>
	{
	public:
		GameExplorerManager(gcRefPtr<UserI> pUser);

		void addItem(DesuraId item) override;
		void removeItem(DesuraId item) override;

		void loadItems() override;
		void saveItems() override;

	protected:
		bool shouldInstallItems();

	private:
		gcRefPtr<UserI> m_pUser;

		gc_IMPLEMENT_REFCOUNTING(GameExplorerManager);
	};
}


#endif //DESURA_GAMEEXPLORERMANAGER_H
