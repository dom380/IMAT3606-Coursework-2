#pragma once
#ifndef UTILITIES_H
#define UTILITIES_H
#include <vector>
#include <string>
#include <set>
using std::string;
using std::vector;
using std::set;

/*
	Class to hold static utility methods.
*/
class Utilities
{
public:
	/*
		Separates the specified file path using \ and / as  delimiters.
		const string &filePath - The file path to separate
		Returns vector<string> - A vector of the separated file path.
	*/
	static vector<string> splitFilePath(const string &filePath);
	/*
		Removes the file extension from the specified filename.
		const string &filename - The filename to remove the extension from.
		Returns string - The filename without the extension.
	*/
	static string removeExtension(const string &filename);
};

#endif // !UTILITIES_H
