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

#ifndef DESURA_MCF_I_H
#define DESURA_MCF_I_H

#include "MCFHeaderI.h"
#include "MCFFileI.h"
#include "ProgressInfo.h"
#include "DownloadProvider.h"
#include "Event.h"

#ifdef LINK_WITH_GMOCK
#include <gmock/gmock.h>
#endif

#include <array>

#define MCF_FACTORY "MCF_INTERFACE_001"

namespace MCFCore
{
	namespace Misc
	{ 
		class DownloadProvidersI;
		class DownloadProvider;

		//! Structer to hold information for the download provider event
		typedef struct
		{
			DownloadProvider::PROVIDER action;	//!< Action (add or remove)
			std::shared_ptr<const DownloadProvider> provider;			//!< Provider information
		} DP_s;

		class GetFile_s
		{
		public:
			void zero()
			{
				authhash[0] = '\0';
				authkey[0] = '\0';
			}

			std::array<char, 33> authhash;
			std::array<char, 10> authkey;
		};
	}

	//! MCFI is the interface file for MCF's. A MCF stores content for the desura application and allows part downloads 
	//! and patching to be more efferent than normal http downloads.
	//!
	class MCFI
	{
	public:
		virtual ~MCFI(){};


		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Getters
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		//! Gets the header of this class.
		//! 
		//! @return Header object
		//!
		virtual MCFHeaderI* getHeader()=0;

		//! Gets the size of all files that need to be downloaded
		//!
		//! @return Download size
		//!
		virtual uint64 getDLSize()=0;

		//! Gets the size of all files that are in the mcf
		//!
		//! @return Download size
		//!
		virtual uint64 getINSize()=0;

		//! Gets the sum of all file sizes saved in the MCF 
		//!
		//! @return Total size
		//!
		virtual uint64 getFileSize()=0;

		//! Gets the total file count for this MCF
		//!
		//! @return File count
		//!
		virtual uint32 getFileCount()=0;

		//! Gets the file info for this file
		//!
		//! @param index File index
		//! @return Mcf file
		//!
		virtual MCFFileI* getMCFFile(uint32 index)=0;

		//! This gets the current file path for the MCF
		//!
		//! @return Mcf path
		//!
		virtual const char* getFile()=0;

		//! Sets the interface to use to get download providers
		//!
		virtual void setDownloadProvider(std::shared_ptr<Misc::DownloadProvidersI> pDownloadProviders)=0;

		//! Gets the authirzation information for testing purpuses
		//!
		//! @return Auth information
		//!
		virtual std::shared_ptr<const Misc::GetFile_s> getAuthInfo()=0;

		//! Gets the progress event
		//!
		//! @return Progress event
		//!
		virtual Event<Misc::ProgressInfo>& getProgEvent()=0;

		//! Gets the error event
		//!
		//! @return Error event
		//!
		virtual Event<gcException>&	getErrorEvent()=0;

		//! Gets the new provider event (i.e. when downloads starts downloading from a server)
		//!
		//! @return Provider event
		//!
		virtual Event<Misc::DP_s>& getNewProvider()=0;

		//! See if we should compress the files for this MCF
		//!
		//! @return True if compressed, false if not
		//!
		virtual bool isCompressed()=0;

		//! Does this MCF contain every file
		//!
		//! @return True if complete, false if not
		//!
		virtual bool isComplete()=0;

		//! Does this MCF contain every file that is not in exsitingFiles
		//!
		//! @return True if complete, false if not
		//!
		virtual bool isComplete(MCFI* exsitingFiles)=0;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Setters
		/////////////////////////////////////////////////////////////////////////////////////////////////////////


		//! Sets the header
		//!
		//! @param head Header object
		//!
		virtual void setHeader(MCFHeaderI* head)=0;

		//! Sets the header using raw information
		//!
		//! @param id DesuraId
		//! @param brach Mcf branch
		//! @param build Mcf content version
		//!
		virtual void setHeader(DesuraId id, MCFBranch branch, MCFBuild build)=0;	

		//! Sets the path and file name of the MCF for save and load
		//!
		//! @param file File name
		//!
		virtual void setFile(const char* file)=0;

		//! Overides the default worker count of 1 per core
		//! 
		//!  @param count Number of worker threads
		//!
		virtual void setWorkerCount(uint16 count)=0;

		//! Turns compression off. Use full for gamecore update MCF files.
		//!
		virtual void disableCompression()=0;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		// File processing
		/////////////////////////////////////////////////////////////////////////////////////////////////////////


		//! Parsers a folder to get the info to build a MCF
		//!
		//! @param path Folder to parse
		//! @param hashFile Produce md5 hashs of the files
		//!
		virtual void parseFolder(const char *path, bool hashFile = false, bool reportProgress = false)=0;

		//! Parsers a MCF to get file info
		//! 
		virtual void parseMCF()=0;

		//! Saves the MCF to disk. Note: Must of run parseFolder before
		//!
		//! @param oldFolderPath Folder to use to create diffs from. 
		//!
		virtual void saveMCF()=0;

		//! Saves the files to disk.
		//! 
		//! @param path Path to save files to
		//!
		virtual void saveFiles(const char* path)=0;

		//! This makes sure all the files at path match the MCF files
		//!
		//! @param path Path to installed files
		//! @param flagMissing add a flag to missing files
		//! @return True if complete, false if not
		//!
		virtual bool verifyInstall(const char* path, bool flagMissing = false, bool useDiffs = false)=0;

		//! Removes the installed files from the local computer matching the files in the MCF
		//!
		//! @param path Path to installed files
		//! @param removeNonSave Remove files that are part of the install but not saved in the MCF
		//!
		virtual void removeFiles(const char* path,  bool removeNonSave = true)=0;

		//! Forces a rehash of files from the local disk
		//!
		virtual void hashFiles()=0;

		//! Forces a rehash of files from the local disk of only files that are also in inMcf
		//! This allows for quicker comparisons for patching
		//!
		//! @param inMcf Mcf to hash files against
		//!
		virtual void hashFiles(MCFI* inMcf)=0;

		//! Checks the crcs of files in the MCF
		//!
		//! @return True if all files pass crc checks, false if not
		//!
		virtual bool crcCheck()=0;

		//! Forces all MCF files to remake crc's. Usefull for mcfs that dont have crc's
		//!
		virtual void makeCRC()=0;

		//! Removes the save flag for all incomplete files
		//!
		virtual void removeIncompleteFiles()=0;

		//! Saves the xml portion of the MCF to a file
		//! 
		//! @param file File to save xml to
		//!
		virtual void saveXml(const char* file)=0;


		//! Saves the header and xml to a blank mcf with no files
		//!
		virtual void saveBlankMcf()=0;

		//! Saves changes to the current mcf header
		//!
		virtual void saveMCFHeader()=0;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Downloading
		/////////////////////////////////////////////////////////////////////////////////////////////////////////


		//! Downloads header and file list from web using MCF service
		//!
		virtual void dlHeaderFromWeb()=0;

		//! Downloads all files from web using MCF service
		//! 
		virtual void dlFilesFromWeb()=0;

		//! Downloads header and file list from web using http
		//!
		//! @param url Url to the MCF file to download the header from
		//!
		virtual void dlHeaderFromHttp(const char* url)=0;

		//! Downloads all files from web using http
		//!
		//! @param url Url to the MCF file to download the files from
		//! @param installDir dir used to expand diffs from
		//!
		virtual void dlFilesFromHttp(const char* url, const char* installDir = nullptr)=0;


		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Threads
		/////////////////////////////////////////////////////////////////////////////////////////////////////////


		//! Pauses all threads
		//!
		virtual void pause()=0;

		//! Unpauses all threads
		//!
		virtual void unpause()=0;

		//! Stops current thread process. Undefined result. Make sure to delete MCF file
		//!
		virtual void stop()=0;

		//! Is threads currently paused
		//!
		virtual bool isPaused()=0;


		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Mcf processing
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		//! Gets the stats of creating a patch between two mcf's.
		//!
		//! @param inMcf MCF to check against
		//! @param dlSize Total download size of patch
		//! @param fileCount number of files included in patch
		//!
		virtual void getPatchStats(MCFI* inMcf, uint64* dlSize, uint32* fileCount)=0;

		//! This looks at another MCF file, and tags all duplicate files
		//!
		//! @param inMcf MCF to check against
		//!
		virtual void makePatch(MCFI* inMcf)=0;

		//! This copys a patch MCF into this and saves the full version at path
		//!
		//! @param inMcf Patch MCF 
		//! @param path path to save result MCF
		//!
		virtual void makeFullFile(MCFI* inMcf, const char* path)=0;

		//! This makes a backwards patch from two full MCF files.
		//! 
		//! @param inMcf Mcf file to compair against this one
		//! @param path Path to save result MCF
		//!
		virtual void makeBackPatchMCF(MCFI* inMcf, const char* path)=0;

		//! This makes sure all the files in the MCF are good
		//!
		//! @return True if good, false if not
		//! 
		virtual bool verifyMCF()=0;

		//! Copys missing files from the source to the current MCF
		//!
		//! @param sourceMcf Mcf to analise for new files
		//!
		virtual void copyMissingFiles(MCFI *sourceMcf)=0;


		//! This looks at another MCF file, and tags files (i.e. marked them as saved)
		//!
		//! @param inMcf MCF to check against
		//! @param tagSame tag all the same files
		//! @param tagChanged tag all the changed files
		//! @param tagDeleted tag all deleted files
		//! @param tagNew tag all new files
		//!
		virtual void markFiles(MCFI* inMcf, bool tagSame, bool tagChanged, bool tagDeleted, bool tagNew)=0;


		//! Takes the contents of this mcf and exports it to a new mcf with the newest header and crc check
		//!
		//! @param path New mcf path
		//!
		virtual void exportMcf(const char* path)=0;

		//! Marks all the files that are different to allow for easy removal. Not safe to save mcf after this!
		//!
		//! @param inMcf MCF to check against
		//!
		virtual void markChanged(MCFI* inMcf)=0;

		//! Resets all file flags to saved. For create mcf
		//!
		virtual void resetSavedFiles()=0;

		//! Verifys the mcf and verifys all files inside it. Prints errors to console.
		//! 
		//! @return Returns 0 if completed correctly. -x where x is number of files that failed. 1 if mcf header is invalid. 
		//!
		virtual int32 verifyAll(const char* tempPath)=0;

		//! Unit test only - not for external use
		//!
		virtual bool verifyUnitTest(MCFI* mcf)=0;

		//! Removed all the non saved files from the mcf
		//!
		virtual void removeNonSavedFiles()=0;

		//! Fixes the md5 and crc's of files
		//!
		virtual bool fixMD5AndCRC()=0;

		//! Moves all the changed files to the start of the mcf
		//!
		virtual void optimiseAndSaveMcf(MCFI* prevMcf, const char* path)=0;

		//! Set the mcf file with an offset
		//!
		virtual void setFile(const char* file, uint64 offset)=0;

		//! Creates a mcf to use with courgette diff
		//!
		//! @param oldMcf old version to compare against
		//! @param outPath path to save mcf with diff in it to
		//!
		virtual void createCourgetteDiffs(MCFI* oldMcf, const char* outPath)=0;

		//! Pre allocates the mcf. Usefull for large mcf's
		//!
		virtual void preAllocateFile()=0;
	};

#ifdef LINK_WITH_GMOCK
	class MCFMock : public MCFI
	{
	public:
		MOCK_METHOD0(getHeader, MCFHeaderI*());
		MOCK_METHOD0(getDLSize, uint64());
		MOCK_METHOD0(getINSize, uint64());
		MOCK_METHOD0(getFileSize, uint64());
		MOCK_METHOD0(getFileCount, uint32());
		MOCK_METHOD1(getMCFFile, MCFFileI*(uint32 index));
		MOCK_METHOD0(getFile, const char*());
		MOCK_METHOD1(setDownloadProvider, void(std::shared_ptr<Misc::DownloadProvidersI> pDownloadProviders));
		MOCK_METHOD0(getAuthInfo, std::shared_ptr<const Misc::GetFile_s>());
		MOCK_METHOD0(getProgEvent, Event<Misc::ProgressInfo>&());
		MOCK_METHOD0(getErrorEvent, Event<gcException>&());
		MOCK_METHOD0(getNewProvider, Event<Misc::DP_s>&());
		MOCK_METHOD0(isCompressed, bool());
		MOCK_METHOD0(isComplete, bool());
		MOCK_METHOD1(isComplete, bool(MCFI* exsitingFiles));
		MOCK_METHOD1(setHeader, void(MCFHeaderI* head));
		MOCK_METHOD3(setHeader, void(DesuraId id, MCFBranch branch, MCFBuild build));
		MOCK_METHOD1(setFile, void(const char* file));
		MOCK_METHOD1(setWorkerCount, void(uint16 count));
		MOCK_METHOD0(disableCompression, void());
		MOCK_METHOD3(parseFolder, void(const char *path, bool hashFile, bool reportProgress));
		MOCK_METHOD0(parseMCF, void());
		MOCK_METHOD0(saveMCF, void());
		MOCK_METHOD1(saveFiles, void(const char* path));
		MOCK_METHOD3(verifyInstall, bool(const char* path, bool flagMissing, bool useDiffs));
		MOCK_METHOD2(removeFiles, void(const char* path,  bool removeNonSave));
		MOCK_METHOD0(hashFiles, void());
		MOCK_METHOD1(hashFiles, void(MCFI* inMcf));
		MOCK_METHOD0(crcCheck, bool());
		MOCK_METHOD0(makeCRC, void());
		MOCK_METHOD0(removeIncompleteFiles, void());
		MOCK_METHOD1(saveXml, void(const char* file));
		MOCK_METHOD0(saveBlankMcf, void());
		MOCK_METHOD0(saveMCFHeader, void());
		MOCK_METHOD0(dlHeaderFromWeb, void());
		MOCK_METHOD0(dlFilesFromWeb, void());
		MOCK_METHOD1(dlHeaderFromHttp, void(const char* url));
		MOCK_METHOD2(dlFilesFromHttp, void(const char* url, const char* installDir));
		MOCK_METHOD0(pause, void());
		MOCK_METHOD0(unpause, void());
		MOCK_METHOD0(stop, void());
		MOCK_METHOD0(isPaused, bool());
		MOCK_METHOD3(getPatchStats, void(MCFI* inMcf, uint64* dlSize, uint32* fileCount));
		MOCK_METHOD1(makePatch, void(MCFI* inMcf));
		MOCK_METHOD2(makeFullFile, void(MCFI* inMcf, const char* path));
		MOCK_METHOD2(makeBackPatchMCF, void(MCFI* inMcf, const char* path));
		MOCK_METHOD0(verifyMCF, bool());
		MOCK_METHOD1(copyMissingFiles, void(MCFI *sourceMcf));
		MOCK_METHOD5(markFiles, void(MCFI* inMcf, bool tagSame, bool tagChanged, bool tagDeleted, bool tagNew));
		MOCK_METHOD1(exportMcf, void(const char* path));
		MOCK_METHOD1(markChanged, void(MCFI* inMcf));
		MOCK_METHOD0(resetSavedFiles, void());
		MOCK_METHOD1(verifyAll, int32(const char* tempPath));
		MOCK_METHOD1(verifyUnitTest, bool(MCFI* mcf));
		MOCK_METHOD0(removeNonSavedFiles, void());
		MOCK_METHOD0(fixMD5AndCRC, bool());
		MOCK_METHOD2(optimiseAndSaveMcf, void(MCFI* prevMcf, const char* path));
		MOCK_METHOD2(setFile, void(const char* file, uint64 offset));
		MOCK_METHOD2(createCourgetteDiffs, void(MCFI* oldMcf, const char* outPath));
		MOCK_METHOD0(preAllocateFile, void());
	};

#endif

}

#endif
