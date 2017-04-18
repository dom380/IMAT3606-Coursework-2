#pragma once
#ifndef ENUMPARSER_H
#define ENUMPARSER_H
#include <string>
using std::string;
#include <map>
using std::map;
/*
	Maps string values to Enums.
*/
template<typename T>
class EnumParser
{
public:
	EnumParser();
	/*
		Returns the corresponding enum for the given string.
		Throws a runtime execption if parse fails.
	*/
	T parse(string& value)
	{
		auto it = map.find(value);
		if (it != map.end())
		{
			return it->second;
		}
		throw std::runtime_error("Unable to parse \""+ value +"\" into enum");
	}
	/*
		Returns the corresponding String for the given enum.
		Throws a runtime execption if parse fails.
	*/
	string parse(T& key)
	{
		for (auto it = map.begin(); it != map.end(); ++it)
		{
			if (it->second == key)
				return it->first;
		}
		throw std::runtime_error("Unable to parse key back to a string.");
	}
private:
	map<string, T> map;
};

#endif // !ENUMPARSER_H

