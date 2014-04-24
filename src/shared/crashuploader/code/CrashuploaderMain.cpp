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


#include "Common.h"
#include <time.h>
#include "XMLMacros.h"

#define DUMP_UPLOAD_URL "http://www.desura.com/api/crashupload"
#define DUMP_UPLOAD_AGENT "Desura CrashDump Reporter"



class Logger
{
public:
	Logger()
	{
#ifdef WIN32
		UTIL::FS::Path path(gcString(UTIL::OS::getAppDataPath(L"\\dumps\\")), "crash_log.txt", false);
		UTIL::FS::recMakeFolder(path);
#else
		UTIL::FS::Path path(gcString(UTIL::OS::getAppDataPath()), "crash_log.txt", false);
		UTIL::FS::recMakeFolder(path);
#endif

		printf("Saving crash report to: %s\n", path.getFullPath().c_str());

		try
		{
			fh.open(path.getFullPath().c_str(), UTIL::FS::FILE_APPEND);
		}
		catch (...)
		{
		}
	}

	void write(const char* format, ...)
	{
		if (fh.isValidFile() == false)
			return;

		size_t size = 512;
		char* temp = nullptr;

		va_list arglist;
		va_start( arglist, format );

		int res = 0;
		do
		{
			size *= 2;
			
			if (temp)
				delete [] temp;

			temp = new char[size];
			
#ifdef WIN32
			res = vsnprintf_s(temp, size, _TRUNCATE, format, arglist);
#else
			res = vsnprintf(temp, size, format, arglist);
#endif
		}
#ifdef WIN32
		while (res == -1);
#else
		while ((size_t)res > size);
#endif
		
		va_end( arglist );

		try
		{
			fh.write(temp, res);
		}
		catch (...)
		{
		}
	}

private:
	UTIL::FS::FileHandle fh;
};

Logger g_Logger;

class Color;
void LogMsg(MSG_TYPE type, std::string msg, Color* col, std::map<std::string, std::string> *mpArgs)
{
	g_Logger.write(msg.c_str());
}

#include "DesuraPrintFRedirect.h"

bool CompressFile(gcString &filePath);
bool PrepDumpForUpload(gcString &dumpFile);
bool UploadDump(const char* file, const char* user, int build, int branch, DelegateI<Prog_s&>* progress, const char* szTracer);


extern "C"
{
	CEXPORT bool UploadCrash(const char* file, const char* user, int build, int branch)
	{
		return UploadDump(file, user, build, branch, nullptr, nullptr);
	}

	CEXPORT bool UploadCrashEx(const char* file, const char* user, int build, int branch, const char* szTracer)
	{
		return UploadDump(file, user, build, branch, nullptr, szTracer);
	}

	CEXPORT bool UploadCrashProg(const char* file, const char* user, int build, int branch, DelegateI<Prog_s&>* progress)
	{
		return UploadDump(file, user, build, branch, progress, nullptr);
	}

	CEXPORT bool UploadCrashProgEx(const char* file, const char* user, int build, int branch, DelegateI<Prog_s&>* progress, const char* szTracer)
	{
		return UploadDump(file, user, build, branch, progress, szTracer);
	}
}

bool CompressFile(gcString &filePath)
{
	uint64 fileSize = UTIL::FS::getFileSize(UTIL::FS::Path(filePath, "", true));

	if (fileSize == 0)
		return false;

	gcString destPath(filePath);
	destPath += ".bz2";

	try
	{
		UTIL::FS::FileHandle fhRead(filePath.c_str(), UTIL::FS::FILE_READ);
		UTIL::FS::FileHandle fhWrite(destPath.c_str(), UTIL::FS::FILE_WRITE);

		if (fhRead.isValidFile() == false)
			return false;

		if (fhWrite.isValidFile() == false)
			return false;

		UTIL::MISC::BZ2Worker worker(UTIL::MISC::BZ2_COMPRESS);

		char buff[10*1024];

		const size_t buffsize = 10*1024;
		uint32 leftToDo = (uint32)fileSize;

		bool end = false;

		do
		{
			size_t curSize = buffsize;

			if (buffsize > leftToDo)
			{
				end = true;
				curSize = leftToDo;
			}

			fhRead.read(buff, curSize);
			leftToDo -= curSize;
			worker.write(buff, curSize, end);

			worker.doWork();
			size_t b = 0;

			do
			{
				b = buffsize;
				worker.read(buff, b);
				fhWrite.write(buff, b);
			}
			while (b > 0);
		}
		while (!end);
	}
	catch (gcException)
	{
		return false;
	}

	UTIL::FS::delFile(UTIL::FS::Path(filePath, "", true));
	filePath = destPath;

	return true;
}

void DumpTracerToFile(const std::string &szTracer, std::function<void(const char*, uint32)> &fh);

bool PrepDumpForUpload(gcString &dumpFile)
{
	UTIL::FS::Path path(dumpFile, "", true);

	if (!UTIL::FS::isValidFile(path))
		return false;

	auto strExt = path.getFile().getFileExt();

	if (strExt == "dmp" || strExt == "log")
		CompressFile(dumpFile);

	return true;
}

bool UploadDump(const char* file, const char* user, int build, int branch, DelegateI<Prog_s&>* progress, const char* szTracer)
{
	if (build == 0)
		build = 9999;

	if (branch == 0)
		branch = BUILDID_PUBLIC;

	g_Logger.write("---------------------------------------\r\n");

	time_t ltime; /* calendar time */
	ltime=time(nullptr); /* get current cal time */

	
#if defined(WIN32) && !defined(__MINGW32__)
	char buff[255] = {0};

	struct tm t;
	localtime_s(&t, &ltime);
	asctime_s(buff, 255, &t);
#else
	struct tm *t = localtime(&ltime);
	char* buff = asctime(t);
#endif

	g_Logger.write("%s\r\n", buff);
	g_Logger.write("---------------------------------------\r\n");

	g_Logger.write("Uploaded crash dump: [%s]\r\n", file);


	gcString dump(file);
	gcString tracer(szTracer);
	gcString log(dump + ".log");

	if (PrepDumpForUpload(dump) == false)
	{
		g_Logger.write("Failed to prepare crash dump.\r\n");
		return false;
	}
	else
	{
		g_Logger.write("Prepared crash dump to: [%s]\r\n", dump.c_str());
	}

	if (!tracer.empty())
	{
		try
		{
			auto valid = false;
			UTIL::FS::FileHandle fh;

			std::function<void(const char*, uint32)> write = [&fh, &valid, log](const char* szData, uint32 nSize)
			{
				if (!valid)
					fh.open(log.c_str(), UTIL::FS::FILE_WRITE);

				valid = true;
				fh.write(szData, nSize);
			};

			DumpTracerToFile(tracer, write);

			if (!valid)
				log = "";
		}
		catch (...)
		{
			log = "";
		}

		if (!log.empty())
			PrepDumpForUpload(log);

#ifdef WIN32
		//Let desura exit now.
		gcString tracerEventName("Global\\{0}Event", tracer);

		auto hHandle = OpenEvent(EVENT_MODIFY_STATE, FALSE, tracerEventName.c_str());

		if (hHandle)
		{
			SetEvent(hHandle);
			CloseHandle(hHandle);
		}
#endif
	}

	std::string os = UTIL::OS::getOSString();

	HttpHandle hh(DUMP_UPLOAD_URL);

	if (progress)
		hh->getProgressEvent() += progress;

	hh->setUserAgent(DUMP_UPLOAD_AGENT);

	hh->cleanUp();
	hh->addPostText("os", os.c_str());
	hh->addPostText("build", build);
	hh->addPostText("appid", branch);

	if (user)
		hh->addPostText("user", user);

	hh->addPostFile("crashfile", dump.c_str());

	if (!log.empty())
		hh->addPostFile("crashlog", log.c_str());

	try
	{
		hh->postWeb();
	}
	catch (gcException &except)
	{
		g_Logger.write("Failed to upload crash: %s [%d.%d].\r\n", except.getErrMsg(), except.getErrId(), except.getSecErrId());
		return false;
	}

	XML::gcXMLDocument doc(const_cast<char*>(hh->getData()), hh->getDataSize());

	try
	{
		doc.ProcessStatus("crashupload");
		g_Logger.write("Uploaded dump\r\n");
		UTIL::FS::delFile(UTIL::FS::Path(dump, "", true));		
	}
	catch (gcException &)
	{
		g_Logger.write("Bad status returned from upload crash dump.\r\n");

		gcString res;
		res.assign(hh->getData(), hh->getDataSize());

		g_Logger.write("Result: \r\n\r\n%s\r\n\r\n", res.c_str());
		return false;	
	}

	return true;
}