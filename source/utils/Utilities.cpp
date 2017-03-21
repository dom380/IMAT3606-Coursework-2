#include "..\..\include\utils\Utilities.h"

vector<string> Utilities::splitFilePath(const string &filePath)
{
	std::vector<std::string> result;

	const char * cString = filePath.c_str();
	const char * start = cString;
	for (; *cString; ++cString)
	{
		if (*cString == '\\' || *cString == '/')
		{
			if (start != cString)
			{
				std::string str(start, cString);
				result.push_back(str);
			}
			else
			{
				result.push_back("");
			}
			start = cString + 1;
		}
	}
	result.push_back(start);
	return result;
}

string Utilities::removeExtension(const string &filename)
{
	size_t lastindex = filename.find_last_of(".");
	string rawname = filename.substr(0, lastindex);
	return rawname;
}
