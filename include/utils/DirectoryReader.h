#pragma once
#ifndef DIRECTORYREADER_H
#define DIRECTORYREADER_H

#include <vector>
using std::vector;
/*
Utility class to parse Directories.
*/
class DirectoryReader
{
public:
	/*
		Allows you to get a vector of strings for files in a directory
		USAGE: getFilesInDirectory("./path/*.obj");
		RETURNS: all objs in directory path.
	*/
	static vector<std::string> getFilesInDirectory(const char* path);
	/*
		Returns the current working directory as a full path.
		See asset manager for relative path.
	*/
	static std::string getCurrentDirectory();
	/*
		Uses getCurrentDirectory and prints it.
	*/
	static void printCurrentDirectory();
	/*
		Prints items in a directory.
		USAGE: printDirectory("./path/*.obj");
		PRINTS: all objs in directory path.
	*/
	static void printDirectory(const char* path);
};

#endif // !DIRECTORYREADER_H