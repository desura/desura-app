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


#ifndef DESURA_MCF_H
#define DESURA_MCF_H

#include "Common.h"

#include "mcfcore/MCFI.h"
#include "MCFFile.h"
#include "MCFHeader.h"

#include <vector>
#include <string>
#include <algorithm>
#include <string.h>
#include <atomic>

#include "util_thread/BaseThread.h"
#include "thread/UpdateThread.h"


#ifdef WIN32
#include <direct.h>
#endif

//this is disable the warning for std::vector not being an export class.
#pragma warning( disable: 4251 )


//max number of threads to download at any one time
#define MAX_DL_THREADS 3

//unknown_item is defined in common.h
#define MCF_NOINDEX UNKNOWN_ITEM


class CourgetteInstance;



namespace MCFCore
{

	namespace Thread
	{
		class BaseMCFThread;
	}

	namespace MISC
	{
		class DownloadProvider;
	}

	namespace Misc
	{
		class MCFServerCon;
	}

	class MCF : public MCFI
	{
	public:
		MCF();
		MCF(std::shared_ptr<MCFCore::Misc::DownloadProvidersI> pDownloadProviders);
		virtual ~MCF();


		//mcf functions

		//! Adds a file to the file list
		//! 
		//! @param file MCFFile to add
		//!
		void addFile(std::shared_ptr<MCFCore::MCFFile>&& file);

		//! Finds a files index by its hash
		//!
		//! @param hash File hash
		//! @return File index
		//!
		uint32 findFileIndexByHash(uint64 hash);

		//! Gets the file at an index
		//!
		//! @param index File index
		//! @return MCFFile
		//!
		std::shared_ptr<MCFCore::MCFFile> getFile(uint32 index);

		//! Gets the file list
		//!
		//! @return MCFile list
		//!
		std::vector<std::shared_ptr<MCFCore::MCFFile>>& getFileList()
		{
			return m_pFileList;
		}

		//! Sorts the file list by hash
		//!
		void sortFileList();

		//! Prints all the file information to the console
		//!
		void printAll();


		//inhereted functions
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Getters
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		MCFCore::MCFHeaderI* getHeader() override;
		uint64 getDLSize() override;
		uint64 getINSize() override;
		uint64 getFileSize() override;
		uint32 getFileCount() override;
		MCFCore::MCFFileI* getMCFFile(uint32 index) override;
		const char* getFile() override;

		void setDownloadProvider(std::shared_ptr<MCFCore::Misc::DownloadProvidersI> pDownloadProviders) override;

		std::shared_ptr<const Misc::GetFile_s> getAuthInfo() override;
		Event<MCFCore::Misc::ProgressInfo>& getProgEvent() override;
		Event<gcException>&	getErrorEvent() override;
		Event<MCFCore::Misc::DP_s>& getNewProvider() override;
		bool isCompressed() override;
		bool isComplete() override;
		bool isComplete(MCFI* exsitingFiles) override;


		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Setters
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		void setHeader(MCFCore::MCFHeaderI* head) override;
		void setHeader(DesuraId id, MCFBranch branch, MCFBuild build) override;
		void setFile(const char* file) override;
		void setFile(const char* file, uint64 offset) override;
		void setWorkerCount(uint16 count) override;
		void disableCompression() override;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		// File processing
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		void parseFolder(const char *path, bool hashFile = false, bool reportProgress = false) override;
		void parseMCF() override;
		void saveMCF() override;
		void saveFiles(const char* path) override;
		bool verifyInstall(const char* path, bool flagMissing = false, bool useDiffs = false) override;
		void removeFiles(const char* path, bool removeNonSave = true) override;
		void hashFiles() override;
		void hashFiles(MCFI* inMcf) override;
		bool crcCheck() override;
		void makeCRC() override;
		void removeIncompleteFiles() override;
		void saveXml(const char* file) override;
		void saveBlankMcf() override;
		void saveMCFHeader() override;

		void preAllocateFile() override;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Downloading
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		void dlHeaderFromWeb() override;
		void dlFilesFromWeb() override;
		void dlHeaderFromHttp(const char* url) override;
		void dlFilesFromHttp(const char* url, const char* installDir = nullptr) override;


		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Threads
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		void pause() override;
		void unpause() override;
		void stop() override;
		bool isPaused() override;


		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Mcf processing
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		void getPatchStats(MCFI* inMcf, uint64* dlSize, uint32* fileCount) override;
		void makePatch(MCFI* inMcf) override;
		void makeFullFile(MCFI* inMcf, const char* path) override;
		void makeBackPatchMCF(MCFI* inMcf, const char* path) override;
		bool verifyMCF() override;
		void copyMissingFiles(MCFI *sourceMcf) override;
		void markFiles(MCFI* inMcf, bool tagSame, bool tagChanged, bool tagDeleted, bool tagNew) override;
		void exportMcf(const char* path) override;
		void markChanged(MCFI* inMcf) override;
		void resetSavedFiles() override;

		int32 verifyAll(const char* tempPath) override;
		bool verifyUnitTest(MCFI* mcf) override;

		void removeNonSavedFiles() override;
		bool fixMD5AndCRC() override;
		void optimiseAndSaveMcf(MCFI* prevMcf, const char* path) override;

		uint64 getFileOffset();

		void createCourgetteDiffs(MCFI* oldMcf, const char* outPath) override;

	protected:
		//! A struct that holds the position of a MCFFile in two different Mcfs.
		//! Used for compariting two Mcf's
		//!
		typedef struct
		{
			uint32 thisMcf;		//!< Index in first mcf
			uint32 otherMcf;	//!< Index in second mcf
		} mcfDif_s;

		Event<MCFCore::Misc::ProgressInfo> onProgressEvent;	//!< Progress event
		Event<gcException> onErrorEvent;					//!< Error event
		Event<MCFCore::Misc::DP_s> onProviderEvent;						//!< Provider event

		//! Generates the xml for the file
		//!
		//! @param doc Xml Document node
		//!
		void genXml(XMLSaveAndCompress *sac);

		//! Parses an xml buffer and generates MCFFiles from it
		//!
		//! @param buff Xml buffer
		//! @param buffLen Buffer size
		//!
		void parseXml(char* buff, uint32 buffLen);

		//! Parases a folder generating MCFFiles. This is a recursive function
		//!
		//! @param path Path to the current folder
		//! @param oPath the original folder path
		//! @param hashFile Should make md5 hashs as it parses
		//!
		void parseFolder(const char *path, const char *oPath);

		//! Finds changes between a Mcf and this Mcf
		//!
		//! @param newFile Input mcf to check against
		//! @param vSame Vector to store the same files into
		//! @param vDiff Vector to store the diff files into
		//! @param vDel Vector to store the deleted files into (in this mcf not in newMcf)
		//! @param vNew Vector to store the new files into (in newMcf not in this mcf)
		//!
		void findChanges(MCF* newFile, std::vector<mcfDif_s> *vSame, std::vector<mcfDif_s> *vDiff, std::vector<mcfDif_s> *vDel, std::vector<mcfDif_s> *vNew = nullptr);

		//! Finds files in both that have the same hash
		//!
		void findSameHashFile(MCF* newFile, std::vector<mcfDif_s> &vSame, std::vector<size_t> &vOther);

		//! Copies one file and data from the MCF
		//!
		//! @param file Source mcf file
		//! @param lastOffset last offset in current mcf
		//! @param hFileSrc File handle to the source mcf
		//! @param hFileDest File handle to the dest mcf
		//!
		void copyFile(std::shared_ptr<MCFCore::MCFFile> file, uint64 &lastOffset, UTIL::FS::FileHandle& hFileSrc, UTIL::FS::FileHandle& hFileDest);

		//! Reports an error to objects using the error event
		//!
		//! @param excpt Exception to report
		//!
		void error(gcException &excpt);

		//! A sub function for saveMCF which saves and compresses the files
		//!
		void saveMCF_CandSFiles();

		//! Setus up header to be saved
		//!
		void saveMCF_Header();

		//! Writes the header and xml to the MCF file
		//!
		void saveMCF_Header(char* xml, uint32 xmlSize, uint64 offset);

		//! Gets a read handle. Applys file offset for use with mcf embeded in another file
		void getReadHandle(UTIL::FS::FileHandle& handle);
		void getWriteHandle(UTIL::FS::FileHandle& handle);


		void createCourgetteDiff(CourgetteInstance* ci, UTIL::MISC::Buffer &oldBuff, UTIL::MISC::Buffer &newBuff, const char* oldHash, std::shared_ptr<MCFFile>& file, UTIL::FS::FileHandle& dest);
		void extractFile(const char* mcfPath, std::shared_ptr<MCFFile>& file, UTIL::MISC::Buffer &outBuff);

		void setServerCon(MCFCore::Misc::MCFServerCon *pMCFServerCon);
		void doDlHeaderFromWeb(MCFCore::Misc::MCFServerCon &msc);


		void runThread(MCFCore::Thread::BaseMCFThread* pThread);

	private:
		uint16 m_uiWCount = 0;
		gcString m_szFile;

		std::atomic<bool> m_bStopped;
		std::atomic<bool> m_bPaused;

		uint32 m_iLastSorted = 0;
		uint32 m_uiChunkCount = 0;
		uint64 m_uiFileOffset = 0;

		::Thread::BaseThread *m_pTHandle = nullptr;

		std::shared_ptr<MCFCore::MCFHeader> m_sHeader;
		std::vector<std::shared_ptr<MCFCore::MCFFile>> m_pFileList;
		std::shared_ptr<MCFCore::Misc::DownloadProvidersI> m_pDownloadProviders;

		std::mutex m_mThreadMutex;
		MCFCore::Misc::MCFServerCon *m_pMCFServerCon = nullptr;
	};


	inline uint32 MCF::getFileCount()
	{
		return (uint32)m_pFileList.size();
	}

	inline std::shared_ptr<const Misc::GetFile_s> MCF::getAuthInfo()
	{
		if (m_pDownloadProviders)
			return m_pDownloadProviders->getDownloadAuth();

		return std::shared_ptr<const Misc::GetFile_s>();
	}

	inline Event<MCFCore::Misc::ProgressInfo>& MCF::getProgEvent()
	{
		return onProgressEvent;
	}

	inline Event<gcException>& MCF::getErrorEvent()
	{
		return onErrorEvent;
	}

	inline Event<MCFCore::Misc::DP_s>& MCF::getNewProvider()
	{
		return onProviderEvent;
	}
}

#endif