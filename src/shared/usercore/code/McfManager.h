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

#ifndef DESURA_MCFMANAGER_H
#define DESURA_MCFMANAGER_H
#ifdef _WIN32
#pragma once
#endif

class mcfInfo
{
public:
	mcfInfo(const char* p, uint32 v, MCFBranch b)
	{
		path = gcString(p);
		version = v;
		branch = b;
	}

	gcString path;
	uint32 version;
	MCFBranch branch;
};

namespace UnitTest
{
	class MCFManagerFixture;
}

namespace sqlite3x
{
	class sqlite3_connection;
}

namespace UserCore
{

	namespace Item
	{
		class ItemInfoI;
	}

	class McfPathData
	{
	public:
		gcString path;
		MCFBuild build;
		MCFBranch branch;
	};

	class MigrateInfo;

	class MCFManagerI : public gcRefBase
	{
	public:
		virtual gcString getMcfPath(gcRefPtr<UserCore::Item::ItemInfoI> item, bool isUnAuthed = false) = 0;
		virtual gcString getMcfPath(DesuraId id, MCFBranch branch, MCFBuild build, bool isUnAuthed = false)=0;
		virtual gcString newMcfPath(DesuraId id, MCFBranch branch, MCFBuild build, bool isUnAuthed = false)=0;

		virtual gcString getMcfBackup(DesuraId gid, DesuraId mid)=0;
		virtual gcString newMcfBackup(DesuraId gid, DesuraId mid)=0;
		virtual void delMcfBackup(DesuraId gid, DesuraId mid)=0;

		virtual void getAllMcfPaths(DesuraId id, std::vector<McfPathData> &vList)=0;
		virtual void delMcfPath(DesuraId id, MCFBranch branch, MCFBuild build)=0;
		virtual void delAllMcfPath(DesuraId id)=0;

		virtual gcString getMcfSavePath()=0;
	};

#ifdef LINK_WITH_GMOCK
	class MCFManagerMock : public MCFManagerI
	{
	public:
		MOCK_METHOD2(getMcfPath, gcString(gcRefPtr<UserCore::Item::ItemInfoI> item, bool isUnAuthed));
		MOCK_METHOD4(getMcfPath, gcString(DesuraId id, MCFBranch branch, MCFBuild build, bool isUnAuthed));
		MOCK_METHOD4(newMcfPath, gcString(DesuraId id, MCFBranch branch, MCFBuild build, bool isUnAuthed));

		MOCK_METHOD2(getMcfBackup, gcString(DesuraId gid, DesuraId mid));
		MOCK_METHOD2(newMcfBackup, gcString(DesuraId gid, DesuraId mid));
		MOCK_METHOD2(delMcfBackup, void(DesuraId gid, DesuraId mid));

		MOCK_METHOD2(getAllMcfPaths, void(DesuraId id, std::vector<McfPathData> &vList));
		MOCK_METHOD3(delMcfPath, void(DesuraId id, MCFBranch branch, MCFBuild build));
		MOCK_METHOD1(delAllMcfPath, void(DesuraId));

		MOCK_METHOD0(getMcfSavePath, gcString());

		gc_IMPLEMENT_REFCOUNTING(MCFManagerMock);
	};
#endif

	class MCFManager : public MCFManagerI
	{
	public:
		MCFManager(const char* appDataPath, const char* mcfDataPath);

		gcString getMcfPath(gcRefPtr<UserCore::Item::ItemInfoI> item, bool isUnAuthed = false) override;
		gcString getMcfPath(DesuraId id, MCFBranch branch, MCFBuild build, bool isUnAuthed = false) override;
		gcString newMcfPath(DesuraId id, MCFBranch branch, MCFBuild build, bool isUnAuthed = false) override;

		gcString getMcfBackup(DesuraId gid, DesuraId mid) override;
		gcString newMcfBackup(DesuraId gid, DesuraId mid) override;
		void delMcfBackup(DesuraId gid, DesuraId mid) override;

		void getAllMcfPaths(DesuraId id, std::vector<McfPathData> &vList) override;
		void delMcfPath(DesuraId id, MCFBranch branch, MCFBuild build) override;
		void delAllMcfPath(DesuraId id) override;

		gcString getMcfSavePath() override;

		void init();

		gc_IMPLEMENT_REFCOUNTING(MCFManager);

	protected:
		friend class UnitTest::MCFManagerFixture;

		void properDelMcfBackup(DesuraId gid, DesuraId mid);

		void scanForMcf();
		void recScanForMcf(const char* root);
		void validateMcf(const char* mcf);

		void getMCFFiles(std::vector<mcfInfo*> &validFiles, DesuraId id);
		gcString generatePath(DesuraId id, MCFBranch branch, MCFBuild build, bool isUnAuthed);

		gcString getMcfDb(const char* dataPath);
		void createMcfDbTables(const char* dataPath);

		void migrateOldFiles();

		void getListOfBadMcfPaths(const gcString &szItemDb, std::vector<MigrateInfo> &delList, std::vector<MigrateInfo> &updateList);
		void getListOfBadMcfPaths(sqlite3x::sqlite3_connection &db, std::vector<MigrateInfo> &delList, std::vector<MigrateInfo> &updateList);

	private:
		const gcString m_szAppDataPath;
		const gcString m_szMCFSavePath;
	};
}

#endif //DESURA_MCFMANAGER_H
