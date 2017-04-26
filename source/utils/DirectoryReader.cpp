#include <utils\DirectoryReader.h>

#include <io.h>
#include <stdio.h>
#include <direct.h>
#define GetCurrentDir _getcwd

vector<std::string> DirectoryReader::getFilesInDirectory(const char* path)
{
	vector<std::string> fileNames;
	struct _finddata_t dirFile;

	intptr_t handle = 0;
	int done = 0;

	handle = _findfirst(path, &dirFile);
	while (handle && done != -1)
	{
		//printf("%s\n", dirFile.name);
		fileNames.push_back(dirFile.name);
		done = _findnext(handle, &dirFile);
	}

	return fileNames;
}

std::string DirectoryReader::getCurrentDirectory()
{
	char cCurrentPath[FILENAME_MAX];

	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
	{
		printf("error %d", errno);
		return "";
	}

	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
	return cCurrentPath;
}

void DirectoryReader::printCurrentDirectory()
{
	printf("The current working directory is %s", getCurrentDirectory());
}

void DirectoryReader::printDirectory(const char * path)
{
	struct _finddata_t dirFile;

	intptr_t handle = 0;
	int done = 0;

	handle = _findfirst(path, &dirFile);
	while (handle && done != -1)
	{
		printf("%s\n", dirFile.name);
		done = _findnext(handle, &dirFile);
	}
}
