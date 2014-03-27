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


//make sure to BOOST_FILESYSTEM_VERSION=2

#include "Common.h"
#include "util/UtilFs.h"
#include "util/UtilMisc.h"
#include "boost/filesystem.hpp"

#include <string>

namespace bf = boost::filesystem;

#ifdef WIN32

#include "boost/filesystem/path.hpp"
// fix for https://svn.boost.org/trac/boost/ticket/6320

class BoostFSCrashFix
{
public:
	BoostFSCrashFix()
	{
		boost::filesystem::path::imbue(std::locale(""));
	}
};

static BoostFSCrashFix gs_BoostFSFix;

#endif


namespace
{
	class DefaultUTILFS : public UTIL::FS::UtilFSI
	{
	public:
		bool isValidFile(const UTIL::FS::Path& file) override
		{
			return UTIL::FS::isValidFile(file);
		}

		void recMakeFolder(const UTIL::FS::Path& path) override
		{
			UTIL::FS::recMakeFolder(path);
		}
	};

	static DefaultUTILFS g_DefaultUTILFS;
}

namespace UTIL
{
	namespace FS
	{

		UtilFSI* g_pDefaultUTILFS = &g_DefaultUTILFS;


		//file handling class
		FileHandle::FileHandle()
		{
			m_hFileHandle = nullptr;
			m_bIsOpen = false;
		}

		FileHandle::FileHandle(const char* fileName, FILE_MODE mode, uint64 offset)
		{
			m_hFileHandle = nullptr;
			m_bIsOpen = false;
			open(fileName, mode, offset);
		}

		FileHandle::FileHandle(const Path& path, FILE_MODE mode, uint64 offset)
		{
			m_hFileHandle = nullptr;
			m_bIsOpen = false;
			open(path.getFullPath().c_str(), mode, offset);
		}

		FileHandle::~FileHandle()
		{
			if (m_bIsOpen)
				close();
		}

		void FileHandle::open(const Path& path, FILE_MODE mode, uint64 offset)
		{
			open(path.getFullPath().c_str(), mode, offset);
		}

		void FileHandle::readCB(uint64 size, UTIL::CB::CallbackI* callback)
		{
			unsigned char buff[10 * 1024];
			uint32 buffSize = 10 * 1024;

			while (size > 0)
			{
				if (buffSize > size)
					buffSize = (uint32)size;

				read((char*)buff, buffSize);

				if (callback->onData(buff, buffSize))
					break;

				size -= buffSize;
			};
		}

		//////////////////////////


		void printError(bf::filesystem_error e);

		uint64 getFileSize(const Path& szfile)
		{
			try
			{
				boost::uintmax_t fs = bf::file_size(bf::path(szfile.getFullPath()));
				return (uint64)fs;
			}
			catch (bf::filesystem_error e)
			{
				printError(e);
			}
			return 0;
		}

		uint64 getFolderSize(const Path& folder)
		{
			uint64 ret = 0;

			try
			{
				bf::path path(folder.getFolderPath());

				bf::recursive_directory_iterator endIter;
				bf::recursive_directory_iterator dirIter(path);

				while (endIter != dirIter)
				{
					if (bf::is_directory(*dirIter) == false)
						ret += (uint64)bf::file_size(*dirIter);

					++dirIter;
				}
			}
			catch (bf::filesystem_error e)
			{
				printError(e);
			}

			return ret;
		}

		void makeFolder(const Path& name)
		{
			try
			{
				bf::create_directory(bf::path(name.getFolderPath()));
			}
			catch (bf::filesystem_error e)
			{
				printError(e);
			}
		}

		void recMakeFolder(const Path& name)
		{
			try
			{
				bf::create_directories(bf::path(name.getFolderPath()));
			}
			catch (bf::filesystem_error e)
			{
				printError(e);
			}

		}

		void moveFolder(const Path& src, const Path& dest)
		{
			if (!isValidFolder(src))
				return;

			recMakeFolder(dest);

			try
			{
				bf::rename(bf::path(src.getFolderPath()), bf::path(dest.getFolderPath()));
			}
			catch (bf::filesystem_error e)
			{
			}
		}

		void moveFile(const Path& src, const Path& dest)
		{
			if (!isValidFile(src))
				return;

			recMakeFolder(dest);

			try
			{
				bf::rename(bf::path(src.getFullPath()), bf::path(dest.getFullPath()));
			}
			catch (bf::filesystem_error e)
			{
			}
		}

		void eraseFolder(const Path& src)
		{
			try
			{
				bf::remove_all(bf::path(src.getFolderPath()));
			}
			catch (bf::filesystem_error e)
			{
				printError(e);
			}
		}


		bool isValidFile(const Path& file)
		{
			try
			{
				return (bf::exists(bf::path(file.getFullPath())) && !bf::is_directory(bf::path(file.getFullPath())));
			}
			catch (bf::filesystem_error e)
			{
				printError(e);
			}
			return false;
		}


		bool isValidFolder(const Path& folder)
		{
			try
			{
				return bf::is_directory(bf::path(folder.getFolderPath()));
			}
			catch (bf::filesystem_error e)
			{
				printError(e);
			}
			return false;
		}

		void delFile(const Path& file)
		{
			if (isValidFile(file))
			{
				try
				{
					bf::remove(bf::path(file.getFullPath()));
				}
				catch (bf::filesystem_error e)
				{
					printError(e);
				}
			}
		}

		void delFolder(const Path& filePath)
		{
			if (isValidFolder(filePath))
			{
				try
				{
					bf::remove_all(bf::path(filePath.getFolderPath()));
				}
				catch (bf::filesystem_error e)
				{
					printError(e);
				}
			}
		}

		bool isFolderEmpty(const Path& filePath)
		{
			if (isValidFolder(filePath))
			{
				return bf::is_empty(bf::path(filePath.getFolderPath()));
			}

			return true;
		}


		void delEmptyFolders(const Path& filePath)
		{
			if (!isValidFolder(filePath))
				return;

			std::vector<UTIL::FS::Path> folders;
			UTIL::FS::getAllFolders(filePath, folders);

			for (size_t x = 0; x<folders.size(); x++)
			{

#ifdef NIX
				if (bf::is_symlink(bf::path(filePath.getFolderPath())))
					continue;
#endif

		delEmptyFolders(folders[x]);
	}

	if (isFolderEmpty(filePath))
		delFolder(filePath);
};

void printError( bf::filesystem_error e)
{
#ifndef NIX
	const char* what = e.what();
	//has a problem under linux about path1

	if (strcmp("not_found_error", what) == 0)
	{
		WarningS("File %ls dne.\n", e.path1().string().c_str());
	}
	else if (strcmp("path_error", what) == 0)
	{
		WarningS("Bad Path %ls.\n", e.path1().string().c_str());
	}
#endif
}


		uint32 readWholeFile(const Path& path, char** buffer)
		{
			uint64 size = UTIL::FS::getFileSize(path);

			if (size >> 32)
				throw gcException(ERR_INVALID, gcString("File {0} is bigger than 4gb. Cant read whole file.", path.getFullPath()));

			uint32 size32 = (uint32)size;

			safe_delete(*buffer);
			*buffer = new char[size32 + 1];

			UTIL::FS::FileHandle fh(path, UTIL::FS::FILE_READ);
			fh.read(*buffer, size32);

			(*buffer)[size32] = 0;

			return size32;
		}

		void copyFile(const Path& src, const Path& dest)
		{
			try
			{
				bf::remove(bf::path(dest.getFullPath()));
				bf::copy_file(bf::path(src.getFullPath()), bf::path(dest.getFullPath()));
			}
			catch (bf::filesystem_error e)
			{
				printError(e);
			}
		}

		void copyFolder(const Path& src, const Path& dest, std::vector<std::string> *vIgnoreList, bool copyOverExisting)
		{
			UTIL::FS::recMakeFolder(dest);
			bf::directory_iterator end_itr;

			try
			{
				for (bf::directory_iterator itr(src.getFolderPath()); itr != end_itr; ++itr)
				{
					bf::path path(itr->path());

					std::string szPath = path.string();
					std::string szNode = path.filename().string();

					bool found = false;
					bool isDir = bf::is_directory(itr->status());

					if (vIgnoreList)
					{
						for (size_t x = 0; x<(*vIgnoreList).size(); x++)
						{
							if ((*vIgnoreList)[x] == szNode)
							{
								found = true;
								break;
							}
						}
					}

					if (found)
						continue;

					Path tdest = dest;

					if (isDir)
					{
						tdest += szNode;
						UTIL::FS::copyFolder(UTIL::FS::Path(szPath, "", false), tdest, vIgnoreList);
					}
					else if (copyOverExisting == false || UTIL::FS::isValidFile(tdest) == false)
					{
						tdest += UTIL::FS::File(szNode);
						UTIL::FS::copyFile(UTIL::FS::Path(szPath, "", true), tdest);
					}
				}
			}
			catch (bf::filesystem_error e)
			{
				printError(e);
			}
		}

		gcTime lastWriteTime(const Path& path)
		{
			return gcTime(boost::filesystem::last_write_time(bf::path(path.getFullPath())));
		}

		void setLastWriteTime(const Path& path, const gcTime& t)
		{
			boost::filesystem::last_write_time(bf::path(path.getFullPath()), t.to_time_t());
		}

		void getAllFiles(const Path& path, std::vector<Path> &outList, std::vector<std::string> *extsFilter)
		{
			bf::path full_path(path.getFolderPath());

			if (!isValidFolder(path))
				return;

			bf::directory_iterator end_iter;
			for (bf::directory_iterator dirIt(full_path); dirIt != end_iter; ++dirIt)
			{
				if (!bf::is_directory(dirIt->status()))
				{
					std::string filePath(dirIt->path().filename().string());

					Path subPath(path);
					subPath += File(gcString(filePath));

					bool bValidFile = UTIL::FS::isValidFile(subPath);

					if (!extsFilter)
					{
						if (!bValidFile)
							throw gcException(ERR_INVALID, gcString("Failed to convert filename to usable UTF8 string. ({0})", subPath.getFullPath()));

						outList.push_back(subPath);
					}
					else
					{
						auto ext = subPath.getFile().getFileExt();

						if (std::find(begin(*extsFilter), end(*extsFilter), ext) != end(*extsFilter))
						{
							if (!bValidFile)
								throw gcException(ERR_INVALID, gcString("Failed to convert filename to usable UTF8 string. ({0})", subPath.getFullPath()));

							outList.push_back(subPath);
						}
					}
				}
			}
		}

		void getAllFolders(const Path& path, std::vector<Path> &outList)
		{
			bf::path full_path(path.getFolderPath());

			if (!isValidFolder(path))
				return;

			bf::directory_iterator end_iter;
			for (bf::directory_iterator dirIt(full_path); dirIt != end_iter; ++dirIt)
			{
				if (bf::is_directory(dirIt->status()))
				{
					Path subPath(path);

					gcWString wpath(dirIt->path().leaf().c_str());
					subPath += Path(gcString(wpath), "", false);

					outList.push_back(subPath);
				}
			}
		}



		uint32 CRC32(const char* file)
		{
			if (!file)
				return -1;


			unsigned long ulCRC = 0xFFFFFFFF; //Initilaize the CRC.

			uint64 size = UTIL::FS::getFileSize(file);
			uint64 done = 0;

			uint32 buffSize = 10 * 1024;
			char buff[10 * 1024];

			try
			{
				UTIL::FS::FileHandle fh(file, UTIL::FS::FILE_READ);

				while (done < size)
				{
					if (buffSize >(size - done))
						buffSize = (uint32)(size - done);

					fh.read(buff, buffSize);

					for (size_t x = 0; x<buffSize; x++)
						ulCRC = UTIL::MISC::CRC32(buff[x], ulCRC);

					done += buffSize;
				}
			}
			catch (...)
			{
				return -1;
			}

			return ~ulCRC; //Finalize the CRC and return.;
		}
	}
}
