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
	T parse(string& value)
	{
		auto it = map.find(value);
		if (it != map.end())
		{
			return it->second;
		}
		throw std::runtime_error("Unable to parse \""+ value +"\" into enum");
	}

	string getString(T& type)
	{
		auto it = strmap.find(type);
		if (it != strmap.end())
		{
			return it->second;
		}
		throw std::runtime_error("Unable to find type");
	}
private:
	map<T, string> strmap;
	map<string, T> map;
	
};

#endif // !ENUMPARSER_H

