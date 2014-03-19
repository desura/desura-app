#include "Common.h"
#include "boost/filesystem.hpp"


namespace bf = boost::filesystem;

namespace UTIL
{
	namespace FS
	{
		bool isValidFile(const Path& file)
		{
			try
			{
				return (bf::exists(bf::path(file.getFullPath())) && !bf::is_directory(bf::path(file.getFullPath())));
			}
			catch (bf::filesystem_error e)
			{

			}
			return false;
		}
	}
}

void PrintfMsg(const char* format, ...)
{

}

int main(int argc, char** argv)
{
	if (argc <= 1 || !argv[1][0])
	{
		fprintf(stdout, "ERROR: Expected jar file as first argument.\n\nPress any key to exit");
		getchar();
		return -1;
	}

	if (!UTIL::FS::isValidFile(argv[1]))
	{
		fprintf(stdout, "ERROR: Failed to find jar file [%s]. Please ensure jar file exists and is valid.\n\nPress any key to exit", argv[1]);
		getchar();
		return -2;
	}

	std::string cur = UTIL::WIN::getRegValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\JavaSoft\\Java Runtime Environment\\CurrentVersion");

	if (cur.empty())
		cur = UTIL::WIN::getRegValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\JavaSoft\\Java Runtime Environment\\CurrentVersion", true);

	if (cur.empty())
	{
		fprintf(stdout, "ERROR: Failed to find current java version. Please ensure java is installed.\n\nPress any key to exit", argv[1]);
		getchar();
		return -2;
	}

	gcString javaPath = UTIL::WIN::getRegValue(gcString("HKEY_LOCAL_MACHINE\\SOFTWARE\\JavaSoft\\Java Runtime Environment\\{0}\\JavaHome", cur)) + "\\bin\\java.exe";

	if (javaPath.empty() || !UTIL::FS::isValidFile(javaPath))
	{
		fprintf(stdout, "ERROR: Failed to find java path [%s]. Please ensure java is installed.\n\nPress any key to exit", javaPath.c_str());
		getchar();
		return -3;
	}

	std::string args("-jar \"");
	args += argv[1];
	args += "\"";

	fprintf(stdout, "Launching: %s %s\n", javaPath.c_str(), args.c_str());
	
	if (UTIL::WIN::launchExe(javaPath.c_str(), args.c_str()))
		return 0;

	fprintf(stdout, "ERROR: Failed to launch java: %d.\n\nPress any key to exit", GetLastError());
	getchar();
	return -4;
}